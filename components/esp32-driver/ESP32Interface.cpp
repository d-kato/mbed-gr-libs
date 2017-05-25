/* ESP32 implementation of NetworkInterfaceAPI
 * Copyright (c) 2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h>
#include "ESP32Interface.h"

// ESP32Interface implementation
ESP32Interface::ESP32Interface(PinName en, PinName io0, PinName tx, PinName rx, bool debug)
    : wifi_en(en), wifi_io0(io0), _esp(tx, rx, debug), server_connect(false), init_end(false), recv_waiting(false)
{
    memset(_ids, 0, sizeof(_ids));
    memset(_cbs, 0, sizeof(_cbs));

    _esp.attach(this, &ESP32Interface::event);
}

int ESP32Interface::connect(const char *ssid, const char *pass, nsapi_security_t security,
                                        uint8_t channel)
{
    if (channel != 0) {
        return NSAPI_ERROR_UNSUPPORTED;
    }

    set_credentials(ssid, pass, security);
    return connect();
}

int ESP32Interface::connect()
{
    char wk_ssid[33];
    size_t len;

    if (chk_init() < 0) {
        return NSAPI_ERROR_DEVICE_ERROR;
    }

    if (!_esp.dhcp(true, 1)) {
        return NSAPI_ERROR_DHCP_FAILURE;
    }

    if (!_esp.connect(ap_ssid, ap_pass)) {
        if (!_esp.startup_retry(3)) {
            return NSAPI_ERROR_DEVICE_ERROR;
        }

        if (!_esp.dhcp(true, 1)) {
            return NSAPI_ERROR_DHCP_FAILURE;
        }

        for (int i = 0; i < 5; i++) {
            if (_esp.get_ssid(wk_ssid)) {
                len = strlen(wk_ssid);
                if ((strlen(ap_ssid) == len) && (memcmp(ap_ssid, wk_ssid, len) == 0)) {
                    break;
                } else {
                    return NSAPI_ERROR_NO_CONNECTION;
                }
            } else {
                Thread::wait(1000);
            }
        }
    }

    if (!_esp.getIPAddress()) {
        return NSAPI_ERROR_DHCP_FAILURE;
    }

    return NSAPI_ERROR_OK;
}

int ESP32Interface::set_credentials(const char *ssid, const char *pass, nsapi_security_t security)
{
    memset(ap_ssid, 0, sizeof(ap_ssid));
    strncpy(ap_ssid, ssid, sizeof(ap_ssid));

    memset(ap_pass, 0, sizeof(ap_pass));
    strncpy(ap_pass, pass, sizeof(ap_pass));

    ap_sec = security;

    return 0;
}

int ESP32Interface::set_channel(uint8_t channel)
{
    return NSAPI_ERROR_UNSUPPORTED;
}

int ESP32Interface::disconnect()
{
    if (chk_init() < 0) {
        return NSAPI_ERROR_DEVICE_ERROR;
    }

    if (!_esp.disconnect()) {
        return NSAPI_ERROR_DEVICE_ERROR;
    }

    return NSAPI_ERROR_OK;
}

const char *ESP32Interface::get_ip_address()
{
    return _esp.getIPAddress();
}

const char *ESP32Interface::get_mac_address()
{
    return _esp.getMACAddress();
}

const char *ESP32Interface::get_gateway()
{
    return _esp.getGateway();
}

const char *ESP32Interface::get_netmask()
{
    return _esp.getNetmask();
}

int8_t ESP32Interface::get_rssi()
{
    return _esp.getRSSI();
}

int ESP32Interface::scan(WiFiAccessPoint *res, unsigned count)
{
    int ret = chk_init();
    if (ret < 0) {
        return NSAPI_ERROR_DEVICE_ERROR;
    } else if (ret == 1) {
        Thread::wait(1500);
    } else {
        // do nothing
    }
    return _esp.scan(res, count);
}

struct esp32_socket {
    int id;
    nsapi_protocol_t proto;
    bool connected;
    SocketAddress addr;
    bool accept_id;
    bool tcp_server;
};

int ESP32Interface::socket_open(void **handle, nsapi_protocol_t proto)
{
    // Look for an unused socket
    int id = -1;

    for (int i = 0; i < ESP32_SOCKET_COUNT; i++) {
        if (!_ids[i]) {
            id = i;
            _ids[i] = true;
            break;
        }
    }

    if (id == -1) {
        return NSAPI_ERROR_NO_SOCKET;
    }

    struct esp32_socket *socket = new struct esp32_socket;
    if (!socket) {
        return NSAPI_ERROR_NO_SOCKET;
    }

    socket->id = id;
    socket->proto = proto;
    socket->connected = false;
    socket->accept_id = false;
    socket->tcp_server = false;
    *handle = socket;
    return 0;
}

int ESP32Interface::socket_close(void *handle)
{
    struct esp32_socket *socket = (struct esp32_socket *)handle;
    int err = 0;

    if (!socket->accept_id) {
        if (!_esp.close(socket->id)) {
            err = NSAPI_ERROR_DEVICE_ERROR;
        }
    }

    if (socket->tcp_server) {
        _esp.del_server();
    }

    _ids[socket->id] = false;
    delete socket;
    return err;
}

int ESP32Interface::socket_bind(void *handle, const SocketAddress &address)
{
    struct esp32_socket *socket = (struct esp32_socket *)handle;

    socket->addr = address;
    return 0;
}

int ESP32Interface::socket_listen(void *handle, int backlog)
{
    struct esp32_socket *socket = (struct esp32_socket *)handle;

    (void)backlog;

    if (server_connect) {
        return NSAPI_ERROR_DEVICE_ERROR;
    }

    if (socket->proto != NSAPI_TCP) {
        return NSAPI_ERROR_UNSUPPORTED;
    }

    if (!_esp.cre_server(socket->addr.get_port())) {
        return NSAPI_ERROR_DEVICE_ERROR;
    }

    socket->tcp_server = true;
    server_connect = true;
    return 0;
}

int ESP32Interface::socket_connect(void *handle, const SocketAddress &addr)
{
    struct esp32_socket *socket = (struct esp32_socket *)handle;

    const char *proto = (socket->proto == NSAPI_UDP) ? "UDP" : "TCP";
    if (!_esp.open(proto, socket->id, addr.get_ip_address(), addr.get_port())) {
        return NSAPI_ERROR_DEVICE_ERROR;
    }

    socket->connected = true;
    return 0;
}

int ESP32Interface::socket_accept(void *server, void **socket, SocketAddress *addr)
{
    int id;

    if (!_esp.accept(&id)) {
        return NSAPI_ERROR_NO_SOCKET;
    }

    struct esp32_socket *socket_new = new struct esp32_socket;
    if (!socket_new) {
        return NSAPI_ERROR_NO_SOCKET;
    }

    socket_new->id = id;
    socket_new->proto = NSAPI_TCP;
    socket_new->connected = true;
    socket_new->accept_id = true;
    socket_new->tcp_server = false;
    *socket = socket_new;

    return 0;
}

int ESP32Interface::socket_send(void *handle, const void *data, unsigned size)
{
    struct esp32_socket *socket = (struct esp32_socket *)handle;

    if (!_esp.send(socket->id, data, size)) {
        return NSAPI_ERROR_DEVICE_ERROR;
    }

    return size;
}

int ESP32Interface::socket_recv(void *handle, void *data, unsigned size)
{
    struct esp32_socket *socket = (struct esp32_socket *)handle;

    int32_t recv = _esp.recv(socket->id, data, size);
    if (recv == -1) {
        recv_waiting = true;
        return NSAPI_ERROR_WOULD_BLOCK;
    } else if (recv < 0) {
        return NSAPI_ERROR_NO_SOCKET;
    } else {
        // do nothing
    }

    return recv;
}

int ESP32Interface::socket_sendto(void *handle, const SocketAddress &addr, const void *data, unsigned size)
{
    struct esp32_socket *socket = (struct esp32_socket *)handle;

    if (socket->connected && socket->addr != addr) {
        if (!_esp.close(socket->id)) {
            return NSAPI_ERROR_DEVICE_ERROR;
        }
        socket->connected = false;
    }

    if (!socket->connected) {
        int err = socket_connect(socket, addr);
        if (err < 0) {
            return err;
        }
        socket->addr = addr;
    }
    
    return socket_send(socket, data, size);
}

int ESP32Interface::socket_recvfrom(void *handle, SocketAddress *addr, void *data, unsigned size)
{
    struct esp32_socket *socket = (struct esp32_socket *)handle;
    int ret = socket_recv(socket, data, size);
    if (ret >= 0 && addr) {
        *addr = socket->addr;
    }

    return ret;
}

void ESP32Interface::socket_attach(void *handle, void (*callback)(void *), void *data)
{
    struct esp32_socket *socket = (struct esp32_socket *)handle;
    _cbs[socket->id].callback = callback;
    _cbs[socket->id].data = data;
}

void ESP32Interface::event() {
    if (!recv_waiting) {
        return;
    }
    recv_waiting = false;

    for (int i = 0; i < ESP32_SOCKET_COUNT; i++) {
        if (_cbs[i].callback) {
            _cbs[i].callback(_cbs[i].data);
        }
    }
}

void ESP32Interface::chip_reset()
{
    wifi_io0 = 1;
    wifi_en = 0;
    Thread::wait(10);
    wifi_en = 1;
}

int ESP32Interface::chk_init()
{
    int ret = 0;

    if (!init_end) {
        chip_reset();
        if (!_esp.startup(3)) {
            ret = -1;
        } else {
            _esp.disconnect();
            init_end = true;
            ret = 1;
        }
    }

    return ret;
}

