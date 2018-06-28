/* ESP32Interface Example
 * Copyright (c) 2015 ARM Limited
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

#ifndef ESP32_H
#define ESP32_H

#include <vector>
#include "ATCmdParser.h"

#ifndef ESP32_CONNECT_TIMEOUT
#define ESP32_CONNECT_TIMEOUT 15000
#endif
#ifndef ESP32_RECV_TIMEOUT
#define ESP32_RECV_TIMEOUT    2000
#endif
#ifndef ESP32_MISC_TIMEOUT
#define ESP32_MISC_TIMEOUT    2000
#endif

/** ESP32Interface class.
    This is an interface to a ESP32 radio.
 */
class ESP32
{
public:
    /**
    * Static method to create or retrieve the single ESP32 instance
    */
    static ESP32 * getESP32Inst(PinName en, PinName io0, PinName tx, PinName rx, bool debug, int baudrate);

    ESP32(PinName en, PinName io0, PinName tx, PinName rx, bool debug, int baudrate);

    /**
    * Check firmware version of ESP8266
    *
    * @return integer firmware version or -1 if firmware query command gives outdated response
    */
    int get_firmware_version(void);

    /**
    * Sets the Wi-Fi Mode
    *
    * @param mode mode of WIFI 1-client, 2-host, 3-both
    * @return true only if ESP32 was setup correctly
    */
    bool set_mode(int mode);

    /**
    * Enable/Disable DHCP
    *
    * @param enabled DHCP enabled when true
    * @param mode mode of DHCP 0-softAP, 1-station, 2-both
    * @return true only if ESP32 enables/disables DHCP successfully
    */
    bool dhcp(bool enabled, int mode);

    /**
    * Connect ESP32 to AP
    *
    * @param ap the name of the AP
    * @param passPhrase the password of AP
    * @return true only if ESP32 is connected successfully
    */
    bool connect(const char *ap, const char *passPhrase);

    /**
    * Disconnect ESP32 from AP
    *
    * @return true only if ESP32 is disconnected successfully
    */
    bool disconnect(void);

    /**
    * Get the IP address of ESP32
    *
    * @return null-teriminated IP address or null if no IP address is assigned
    */
    const char *getIPAddress(void);
    const char *getIPAddress_ap(void);

    /**
    * Get the MAC address of ESP32
    *
    * @return null-terminated MAC address or null if no MAC address is assigned
    */
    const char *getMACAddress(void);
    const char *getMACAddress_ap(void);

     /** Get the local gateway
     *
     *  @return         Null-terminated representation of the local gateway
     *                  or null if no network mask has been recieved
     */
    const char *getGateway();
    const char *getGateway_ap();

    /** Get the local network mask
     *
     *  @return         Null-terminated representation of the local network mask 
     *                  or null if no network mask has been recieved
     */
    const char *getNetmask();
    const char *getNetmask_ap();

    /* Return RSSI for active connection
     *
     * @return      Measured RSSI
     */
    int8_t getRSSI();

    /**
    * Check if ESP32 is conenected
    *
    * @return true only if the chip has an IP address
    */
    bool isConnected(void);

    /** Scan for available networks
     *
     * @param  ap    Pointer to allocated array to store discovered AP
     * @param  limit Size of allocated @a res array, or 0 to only count available AP
     * @return       Number of entries in @a res, or if @a count was 0 number of available networks, negative on error
     *               see @a nsapi_error
     */
    int scan(WiFiAccessPoint *res, unsigned limit);

    /**
    * Open a socketed connection
    *
    * @param type the type of socket to open "UDP" or "TCP"
    * @param id id to give the new socket, valid 0-4
    * @param port port to open connection with
    * @param addr the IP address of the destination
    * @param addr the IP address of the destination
    * @param opt  type=" UDP" : UDP socket's local port, zero means any
    *             type=" TCP" : TCP connection's keep alive time, zero means disabled
    * @return true only if socket opened successfully
    */
    bool open(const char *type, int id, const char* addr, int port, int opt = 0);

    /**
    * Sends data to an open socket
    *
    * @param id id of socket to send to
    * @param data data to be sent
    * @param amount amount of data to be sent - max 1024
    * @return true only if data sent successfully
    */
    bool send(int id, const void *data, uint32_t amount);

    /**
    * Receives data from an open socket
    *
    * @param id id to receive from
    * @param data placeholder for returned information
    * @param amount number of bytes to be received
    * @return the number of bytes received
    */
    int32_t recv(int id, void *data, uint32_t amount, uint32_t timeout = ESP32_RECV_TIMEOUT);

    /**
    * Closes a socket
    *
    * @param id id of socket to close, valid only 0-4
    * @param wait_close 
    * @return true only if socket is closed successfully
    */
    bool close(int id, bool wait_close = false);

    /**
    * Allows timeout to be changed between commands
    *
    * @param timeout_ms timeout of the connection
    */
    void setTimeout(uint32_t timeout_ms = ESP32_MISC_TIMEOUT);

    /**
    * Checks if data is available
    */
    bool readable();

    /**
    * Checks if data can be written
    */
    bool writeable();

    void socket_attach(int id, void (*callback)(void *), void *data);
    int get_free_id();

    bool config_soft_ap(const char *ap, const char *passPhrase, uint8_t chl, uint8_t ecn);

    bool restart();
    bool get_ssid(char *ap);
    bool cre_server(int port);
    bool del_server();
    bool accept(int * p_id);

    bool set_network(const char *ip_address, const char *netmask, const char *gateway);
    bool set_network_ap(const char *ip_address, const char *netmask, const char *gateway);

    /**
    * Attach a function to call whenever network state has changed
    *
    * @param func A pointer to a void function, or 0 to set as none
    */
    void attach_wifi_status(mbed::Callback<void(int8_t)> status_cb);

    /** Get the connection status
     *
     *  @return         The connection status according to ConnectionStatusType
     */
    int8_t get_wifi_status() const;

    static const int8_t WIFIMODE_STATION = 1;
    static const int8_t WIFIMODE_SOFTAP = 2;
    static const int8_t WIFIMODE_STATION_SOFTAP = 3;
    static const int8_t SOCKET_COUNT = 5;

    static const int8_t STATUS_DISCONNECTED = 0;
    static const int8_t STATUS_CONNECTED = 1;
    static const int8_t STATUS_GOT_IP = 2;

private:
    DigitalOut * _p_wifi_en;
    DigitalOut * _p_wifi_io0;
    bool init_end;
    UARTSerial _serial;
    ATCmdParser _parser;
    struct packet {
        struct packet *next;
        int id;
        uint32_t len;
        uint32_t index;
        // data follows
    } *_packets, **_packets_end;
    int _wifi_mode;
    int _baudrate;

    std::vector<int> _accept_id;
    uint32_t _id_bits;
    uint32_t _id_bits_close;
    bool _server_act;
    rtos::Mutex _smutex; // Protect serial port access
    static ESP32 * instESP32;
    int8_t _wifi_status;
    Callback<void(int8_t)> _wifi_status_cb;

    bool _ids[SOCKET_COUNT];
    struct {
        void (*callback)(void *);
        void *data;
        int  Notified;
    } _cbs[SOCKET_COUNT];

    bool startup();
    bool reset(void);
    void debugOn(bool debug);
    void socket_handler(bool connect, int id);
    void _connect_handler_0();
    void _connect_handler_1();
    void _connect_handler_2();
    void _connect_handler_3();
    void _connect_handler_4();
    void _closed_handler_0();
    void _closed_handler_1();
    void _closed_handler_2();
    void _closed_handler_3();
    void _closed_handler_4();
    void _connection_status_handler();
    void _packet_handler();
    void event();
    bool recv_ap(nsapi_wifi_ap_t *ap);

    char _ip_buffer[16];
    char _gateway_buffer[16];
    char _netmask_buffer[16];
    char _mac_buffer[18];

    char _ip_buffer_ap[16];
    char _gateway_buffer_ap[16];
    char _netmask_buffer_ap[16];
    char _mac_buffer_ap[18];
};

#endif
