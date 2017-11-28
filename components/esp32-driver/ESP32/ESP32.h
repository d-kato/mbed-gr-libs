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

#include "ATParser_os.h"

#define ESP32_SOCKET_COUNT 5

/** ESP32Interface class.
    This is an interface to a ESP32 radio.
 */
class ESP32
{
public:
    /**
    * Static method to create or retrieve the single ESP32 instance
    */
    static ESP32 * getESP32Inst(PinName en, PinName io0, PinName tx, PinName rx, bool debug,
                                PinName rts, PinName cts, int baudrate);

    ESP32(PinName en, PinName io0, PinName tx, PinName rx, bool debug,
          PinName rts, PinName cts, int baudrate);

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
    * @return true only if socket opened successfully
    */
    bool open(const char *type, int id, const char* addr, int port);

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
    int32_t recv(int id, void *data, uint32_t amount);

    /**
    * Closes a socket
    *
    * @param id id of socket to close, valid only 0-4
    * @param wait_close 
    * @return true only if socket is closed successfully
    */
    bool close(int id, bool wait_close);

    /**
    * Allows timeout to be changed between commands
    *
    * @param timeout_ms timeout of the connection
    */
    void setTimeout(uint32_t timeout_ms);

    /**
    * Checks if data is available
    */
    bool readable();

    /**
    * Checks if data can be written
    */
    bool writeable();

    void attach(int id, void (*callback)(void *), void *data);
    int get_free_id();

    bool config_soft_ap(const char *ap, const char *passPhrase, uint8_t chl, uint8_t ecn);

    bool restart();
    bool get_ssid(const char *ap);
    bool cre_server(int port);
    bool del_server();
    bool accept(int * p_id);

private:
    DigitalOut wifi_en;
    DigitalOut wifi_io0;
    bool init_end;
    BufferedSerial _serial;
    ATParser_os _parser;
    struct packet {
        struct packet *next;
        int id;
        uint32_t len;
        uint32_t index;
        // data follows
    } *_packets, **_packets_end;
    int _wifi_mode;
    int _baudrate;
    PinName _rts;
    PinName _cts;
    int _flow_control;

    std::vector<int> _accept_id;
    uint32_t _id_bits;
    uint32_t _id_bits_close;
    bool _server_act;
    rtos::Mutex _lock;
    static ESP32 * instESP32;

    bool _ids[ESP32_SOCKET_COUNT];
    struct {
        void (*callback)(void *);
        void *data;
        int  Notified;
    } _cbs[ESP32_SOCKET_COUNT];

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
