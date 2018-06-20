/* ESP32 implementation of NetworkInterfaceAPI
 * Copyright (c) 2017 Renesas Electronics Corporation
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
ESP32Interface::ESP32Interface(PinName en, PinName io0, PinName tx, PinName rx, bool debug,
    PinName rts, PinName cts, int baudrate)
    : ESP32Stack(en, io0, tx, rx, debug, rts, cts, baudrate)
{
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

    if (!_esp->dhcp(true, 1)) {
        return NSAPI_ERROR_DHCP_FAILURE;
    }

    _esp->disconnect();
    if (!_esp->connect(ap_ssid, ap_pass)) {
        if (!_esp->restart()) {
            return NSAPI_ERROR_DEVICE_ERROR;
        }

        if (!_esp->dhcp(true, 1)) {
            return NSAPI_ERROR_DHCP_FAILURE;
        }

        for (int i = 0; i < 10; i++) {
            if (_esp->get_ssid(wk_ssid)) {
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

    if (!_esp->getIPAddress()) {
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
    if (!_esp->disconnect()) {
        return NSAPI_ERROR_DEVICE_ERROR;
    }

    return NSAPI_ERROR_OK;
}

const char *ESP32Interface::get_ip_address()
{
    return _esp->getIPAddress();
}

const char *ESP32Interface::get_mac_address()
{
    return _esp->getMACAddress();
}

const char *ESP32Interface::get_gateway()
{
    return _esp->getGateway();
}

const char *ESP32Interface::get_netmask()
{
    return _esp->getNetmask();
}

int8_t ESP32Interface::get_rssi()
{
    return _esp->getRSSI();
}

int ESP32Interface::scan(WiFiAccessPoint *res, unsigned count)
{
    return _esp->scan(res, count);
}
