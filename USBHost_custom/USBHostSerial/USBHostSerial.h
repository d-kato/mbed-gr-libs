/* mbed USBHost Library
 * Copyright (c) 2006-2013 ARM Limited
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

#ifndef USBHOSTSERIAL_H
#define USBHOSTSERIAL_H

#include "USBHostConf.h"

#if USBHOST_SERIAL

#include "USBHost.h"
#include "Stream.h"
#include "CircBufferHostSerial.h"
#include "Callback.h"

/**
 * A class to communicate a USB virtual serial port
 */
class USBHostSerialPort : public Stream {
public:
    /**
    * Constructor
    */
    USBHostSerialPort(uint32_t buf_size);

    /**
     * Destructor
     */
    virtual ~USBHostSerialPort();

    enum IrqType {
        RxIrq,
        TxIrq,

        IrqCnt
    };

    enum Parity {
        None = 0,
        Odd,
        Even,
        Mark,
        Space
    };

    void connect(USBHost* _host, USBDeviceConnected * _dev,
        uint8_t _serial_intf, USBEndpoint* _bulk_in, USBEndpoint* _bulk_out);

    bool connected();

    /**
    * Check the number of bytes available.
    *
    * @returns the number of bytes available
    */
    uint32_t available();

    /**
     *  Attach a member function to call when a packet is received.
     *
     *  @param obj pointer to the object to call the member function on
     *  @param method pointer to the member function to be called
     *  @param type Which serial interrupt to attach the member function to (Seriall::RxIrq for receive, TxIrq for transmit buffer empty)
     */
    template<typename T>
    void attach(T *obj, void (T::*method)(), IrqType type=RxIrq) {
        attach(callback(obj, method), type);
    }

    /** Attach a function to call whenever a serial interrupt is generated
     *
     *  @param func A pointer to a void function, or 0 to set as none
     *  @param type Which serial interrupt to attach the member function to (Seriall::RxIrq for receive, TxIrq for transmit buffer empty)
     */
    void attach(Callback<void()> func, IrqType type=RxIrq) {
        if (func) {
            _irq[type] = func;
        }
    }

    /** Set the baud rate of the serial port
     *
     *  @param baudrate The baudrate of the serial port (default = 9600).
     */
    void baud(int baudrate = 9600);

    /** Set the transmission format used by the Serial port
     *
     *  @param bits The number of bits in a word (default = 8)
     *  @param parity The parity used (USBHostSerialPort::None, USBHostSerialPort::Odd, USBHostSerialPort::Even, USBHostSerialPort::Mark, USBHostSerialPort::Space; default = USBHostSerialPort::None)
     *  @param stop The number of stop bits (1 or 2; default = 1)
     */
    void format(int bits = 8, Parity parity = USBHostSerialPort::None, int stop_bits = 1);
    virtual int writeBuf(const char* b, int s);
    virtual int readBuf(char* b, int s, int timeout = -1);

//protected:
    virtual int _getc();
    virtual int _putc(int c);

private:
    USBHost * host;
    USBDeviceConnected * dev;

    USBEndpoint * bulk_in;
    USBEndpoint * bulk_out;
    uint32_t size_bulk_in;
    uint32_t size_bulk_out;

    void init();

    CircBufferHostSerial<uint8_t> * p_circ_buf;

    uint8_t * p_buf;

    typedef struct {
        uint32_t baudrate;
        uint8_t stop_bits;
        uint8_t parity;
        uint8_t data_bits;
    } PACKED LINE_CODING;

#if defined(TARGET_RZ_A2XX)
    LINE_CODING * p_line_coding;
    uint8_t * p_buf_out;
    uint8_t * p_buf_out_c;
#else
    LINE_CODING line_coding;
#endif

    void rxHandler();
    void txHandler();
    Callback<void()> _irq[IrqCnt];

    uint8_t serial_intf;
    bool dev_connected;
};

#if (USBHOST_SERIAL <= 1)

class USBHostSerial : public IUSBEnumerator, public USBHostSerialPort
{
public:
    USBHostSerial(uint32_t buf_size = (1024 * 32));

    /**
     * Try to connect a serial device
     *
     * @return true if connection was successful
     */
    bool connect();

    void disconnect();

    uint16_t get_vid() {
        return _vid;
    }

    uint16_t get_pid() {
        return _pid;
    }

protected:
    USBHost* host;
    USBDeviceConnected* dev;
    uint8_t port_intf;
    int ports_found;
    uint16_t _vid;
    uint16_t _pid;

    //From IUSBEnumerator
    virtual void setVidPid(uint16_t vid, uint16_t pid);
    virtual bool parseInterface(uint8_t intf_nb, uint8_t intf_class, uint8_t intf_subclass, uint8_t intf_protocol); //Must return true if the interface should be parsed
    virtual bool useEndpoint(uint8_t intf_nb, ENDPOINT_TYPE type, ENDPOINT_DIRECTION dir); //Must return true if the endpoint will be used

};

#else // (USBHOST_SERIAL > 1)

class USBHostMultiSerial : public IUSBEnumerator {
public:
    USBHostMultiSerial(uint32_t buf_size = (1024 * 32));
    virtual ~USBHostMultiSerial();

    USBHostSerialPort* getPort(int port)
    {
        return port < USBHOST_SERIAL ? ports[port] : NULL;
    }

    /**
     * Try to connect a serial device
     *
     * @return true if connection was successful
     */
    bool connect();

    void disconnect();

    /**
    * Check if a any serial port is connected
    *
    * @returns true if a serial device is connected
    */
    bool connected();

protected:
    USBHost* host;
    USBDeviceConnected* dev;
    USBHostSerialPort* ports[USBHOST_SERIAL];
    uint8_t port_intf[USBHOST_SERIAL];
    int ports_found;
    uint32_t _buf_size;

    //From IUSBEnumerator
    virtual void setVidPid(uint16_t vid, uint16_t pid);
    virtual bool parseInterface(uint8_t intf_nb, uint8_t intf_class, uint8_t intf_subclass, uint8_t intf_protocol); //Must return true if the interface should be parsed
    virtual bool useEndpoint(uint8_t intf_nb, ENDPOINT_TYPE type, ENDPOINT_DIRECTION dir); //Must return true if the endpoint will be used

private:
    bool dev_connected;
};
#endif // (USBHOST_SERIAL <= 1)

#endif

#endif
