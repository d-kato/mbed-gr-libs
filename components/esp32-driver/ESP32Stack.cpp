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
#include "ESP32Stack.h"

// ESP32Stack implementation
ESP32Stack::ESP32Stack(PinName en, PinName io0, PinName tx, PinName rx, bool debug,
    PinName rts, PinName cts, int baudrate)
{
    _esp = ESP32::getESP32Inst(en, io0, tx, rx, debug, rts, cts, baudrate);
}

struct esp32_socket {
    int id;
    nsapi_protocol_t proto;
    bool connected;
    SocketAddress addr;
    bool accept_id;
    bool tcp_server;
};

int ESP32Stack::socket_open(void **handle, nsapi_protocol_t proto)
{
    // Look for an unused socket
    int id = _esp->get_free_id();

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

int ESP32Stack::socket_close(void *handle)
{
    struct esp32_socket *socket = (struct esp32_socket *)handle;
    int err = 0;

    if (!_esp->close(socket->id, socket->accept_id)) {
        err = NSAPI_ERROR_DEVICE_ERROR;
    }

    if (socket->tcp_server) {
        _esp->del_server();
    }

    delete socket;
    return err;
}

int ESP32Stack::socket_bind(void *handle, const SocketAddress &address)
{
    struct esp32_socket *socket = (struct esp32_socket *)handle;

    socket->addr = address;
    return 0;
}

int ESP32Stack::socket_listen(void *handle, int backlog)
{
    struct esp32_socket *socket = (struct esp32_socket *)handle;

    (void)backlog;

    if (socket->proto != NSAPI_TCP) {
        return NSAPI_ERROR_UNSUPPORTED;
    }

    if (!_esp->cre_server(socket->addr.get_port())) {
        return NSAPI_ERROR_DEVICE_ERROR;
    }

    socket->tcp_server = true;
    return 0;
}

int ESP32Stack::socket_connect(void *handle, const SocketAddress &addr)
{
    struct esp32_socket *socket = (struct esp32_socket *)handle;

    const char *proto = (socket->proto == NSAPI_UDP) ? "UDP" : "TCP";
    if (!_esp->open(proto, socket->id, addr.get_ip_address(), addr.get_port())) {
        return NSAPI_ERROR_DEVICE_ERROR;
    }

    socket->connected = true;
    return 0;
}

int ESP32Stack::socket_accept(void *server, void **socket, SocketAddress *addr)
{
    int id;

    if (!_esp->accept(&id)) {
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

int ESP32Stack::socket_send(void *handle, const void *data, unsigned size)
{
    struct esp32_socket *socket = (struct esp32_socket *)handle;

    if (!_esp->send(socket->id, data, size)) {
        return NSAPI_ERROR_DEVICE_ERROR;
    }

    return size;
}

int ESP32Stack::socket_recv(void *handle, void *data, unsigned size)
{
    struct esp32_socket *socket = (struct esp32_socket *)handle;

    int32_t recv = _esp->recv(socket->id, data, size);
    if (recv == -1) {
        return NSAPI_ERROR_WOULD_BLOCK;
    } else if (recv < 0) {
        return NSAPI_ERROR_NO_SOCKET;
    } else {
        // do nothing
    }

    return recv;
}

int ESP32Stack::socket_sendto(void *handle, const SocketAddress &addr, const void *data, unsigned size)
{
    struct esp32_socket *socket = (struct esp32_socket *)handle;

    if (socket->connected && socket->addr != addr) {
        if (!_esp->close(socket->id, socket->accept_id)) {
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

int ESP32Stack::socket_recvfrom(void *handle, SocketAddress *addr, void *data, unsigned size)
{
    struct esp32_socket *socket = (struct esp32_socket *)handle;
    int ret = socket_recv(socket, data, size);
    if (ret >= 0 && addr) {
        *addr = socket->addr;
    }

    return ret;
}

void ESP32Stack::socket_attach(void *handle, void (*callback)(void *), void *data)
{
    struct esp32_socket *socket = (struct esp32_socket *)handle;
    _esp->attach(socket->id, callback, data);
}

