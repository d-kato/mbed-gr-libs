
#include <string.h>
#include "ESP32InterfaceAP.h"

// ESP32InterfaceAP implementation
ESP32InterfaceAP::ESP32InterfaceAP(PinName en, PinName io0, PinName tx, PinName rx, bool debug,
    PinName rts, PinName cts, int baudrate)
    : ESP32Stack(en, io0, tx, rx, debug, rts, cts, baudrate)
{
    own_ch = 1;
}

int ESP32InterfaceAP::connect(const char *ssid, const char *pass, nsapi_security_t security,
                                        uint8_t channel)
{
    int ret;

    ret = set_credentials(ssid, pass, security);
    if (ret != 0) {
        return ret;
    }

    ret = set_channel(channel);
    if (ret != 0) {
        return ret;
    }

    return connect();
}

int ESP32InterfaceAP::connect()
{
    if (!_esp->set_mode(3)) {
        return NSAPI_ERROR_DEVICE_ERROR;
    }

    if (!_esp->dhcp(true, 1)) {
        return NSAPI_ERROR_DHCP_FAILURE;
    }

    if (!_esp->config_soft_ap(own_ssid, own_pass, own_ch, (uint8_t)own_sec)) {
        return NSAPI_ERROR_DEVICE_ERROR;
    }

    return NSAPI_ERROR_OK;
}

int ESP32InterfaceAP::set_credentials(const char *ssid, const char *pass, nsapi_security_t security)
{
    switch (security) {
        case NSAPI_SECURITY_NONE:
        case NSAPI_SECURITY_WPA:
        case NSAPI_SECURITY_WPA2:
        case NSAPI_SECURITY_WPA_WPA2:
            own_sec = security;
            break;
        case NSAPI_SECURITY_UNKNOWN:
        case NSAPI_SECURITY_WEP:
        default:
            return NSAPI_ERROR_UNSUPPORTED;
    }

    memset(own_ssid, 0, sizeof(own_ssid));
    strncpy(own_ssid, ssid, sizeof(own_ssid));

    memset(own_pass, 0, sizeof(own_pass));
    strncpy(own_pass, pass, sizeof(own_pass));

    return 0;
}

int ESP32InterfaceAP::set_channel(uint8_t channel)
{
    if (channel != 0) {
        own_ch = channel;
    }

    return 0;
}

int ESP32InterfaceAP::disconnect()
{
    if (!_esp->set_mode(1)) {
        return NSAPI_ERROR_DEVICE_ERROR;
    }

    return NSAPI_ERROR_OK;
}

const char *ESP32InterfaceAP::get_ip_address()
{
    return _esp->getIPAddress_ap();
}

const char *ESP32InterfaceAP::get_mac_address()
{
    return _esp->getMACAddress_ap();
}

const char *ESP32InterfaceAP::get_gateway()
{
    return _esp->getGateway_ap();
}

const char *ESP32InterfaceAP::get_netmask()
{
    return _esp->getNetmask_ap();
}

int8_t ESP32InterfaceAP::get_rssi()
{
    return 0;
}

int ESP32InterfaceAP::scan(WiFiAccessPoint *res, unsigned count)
{
    return NSAPI_ERROR_UNSUPPORTED;
}

