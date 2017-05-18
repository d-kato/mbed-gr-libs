
#ifndef ESP32_INTERFACE_H
#define ESP32_INTERFACE_H

#include "ESP8266Interface.h"

/** ESP32Interface class
 *  Implementation of the NetworkStack for the ESP32
 */
class ESP32Interface : public ESP8266Interface
{
public:
    /** ESP32Interface lifetime
     * @param en        EN pin
     * @param io0       IO0 pin
     * @param tx        TX pin
     * @param rx        RX pin
     * @param debug     Enable debugging
     */
    ESP32Interface(PinName en, PinName io0, PinName tx, PinName rx, bool debug = false) :
      wifi_en(en), wifi_io0(io0), ESP8266Interface(tx, rx, debug), chip_reset_done(false) {}

    /** ESP32 chip reset
     *
     */
    void chip_reset() {
        chip_reset_done = true;
        wifi_io0 = 1;
        wifi_en = 0;
        Thread::wait(10);
        wifi_en = 1;
        Thread::wait(800);  // Wait a little after reset of ESP32.
    }

    /** Start the interface
     *
     *  Attempts to connect to a WiFi network. Requires ssid and passphrase to be set.
     *  If passphrase is invalid, NSAPI_ERROR_AUTH_ERROR is returned.
     *
     *  @return         0 on success, negative error code on failure
     */
    virtual int connect() {
        chip_reset_check();
        return ESP8266Interface::connect();
    }

    /** Start the interface
     *
     *  Attempts to connect to a WiFi network.
     *
     *  @param ssid      Name of the network to connect to
     *  @param pass      Security passphrase to connect to the network
     *  @param security  Type of encryption for connection (Default: NSAPI_SECURITY_NONE)
     *  @param channel   This parameter is not supported, setting it to anything else than 0 will result in NSAPI_ERROR_UNSUPPORTED
     *  @return          0 on success, or error code on failure
     */
    virtual int connect(const char *ssid, const char *pass, nsapi_security_t security = NSAPI_SECURITY_NONE,
                                  uint8_t channel = 0) {
        chip_reset_check();
        return ESP8266Interface::connect(ssid, pass, security, channel);
    }

    /** Stop the interface
     *  @return             0 on success, negative on failure
     */
    virtual int disconnect() {
        chip_reset_check();
        return ESP8266Interface::disconnect();
    }

    /** Scan for available networks
     *
     * This function will block.
     *
     * @param  ap       Pointer to allocated array to store discovered AP
     * @param  count    Size of allocated @a res array, or 0 to only count available AP
     * @param  timeout  Timeout in milliseconds; 0 for no timeout (Default: 0)
     * @return          Number of entries in @a, or if @a count was 0 number of available networks, negative on error
     *                  see @a nsapi_error
     */
    virtual int scan(WiFiAccessPoint *res, unsigned count) {
        chip_reset_check();
        return ESP8266Interface::scan(res, count);
    }

private:
    DigitalOut wifi_en;
    DigitalOut wifi_io0;
    bool chip_reset_done;

    void chip_reset_check(void) {
        if (!chip_reset_done) {
            chip_reset();
        }
    }
};

#endif
