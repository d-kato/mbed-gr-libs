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

#include "USBHostSerial.h"

#if USBHOST_SERIAL

#include "dbg.h"

#define CHECK_INTERFACE(cls,subcls,proto) \
        (((cls == 0xFF)         && (subcls == 0xFF) && (proto == 0xFF)) /* QUALCOM CDC */  || \
         ((cls == SERIAL_CLASS) && (subcls == 0x00) && (proto == 0x00)) /* STANDARD CDC */ )

#if (USBHOST_SERIAL <= 1)

USBHostSerial::USBHostSerial(uint32_t buf_size) : USBHostSerialPort(buf_size)
{
    host = USBHost::getHostInst();
    ports_found = 0;
}

void USBHostSerial::disconnect(void)
{
    ports_found = 0;
    dev = NULL;
}

bool USBHostSerial::connect() {

    if (dev) {
        for (uint8_t i = 0; i < MAX_DEVICE_CONNECTED; i++) {
            USBDeviceConnected* d = host->getDevice(i);
            if (dev == d)
                return true;
        }
        disconnect();
    }
    for (uint8_t i = 0; i < MAX_DEVICE_CONNECTED; i++) {
        USBDeviceConnected* d = host->getDevice(i);
        if (d != NULL) {

            USB_DBG("Trying to connect serial device \r\n");
            if(host->enumerate(d, this))
                break;

            USBEndpoint* bulk_in  = d->getEndpoint(port_intf, BULK_ENDPOINT, IN);
            USBEndpoint* bulk_out = d->getEndpoint(port_intf, BULK_ENDPOINT, OUT);
            if (bulk_in && bulk_out)
            {
                USBHostSerialPort::connect(host,d,port_intf,bulk_in, bulk_out);
                dev = d;
            }
        }
    }
    return dev != NULL;
}

/*virtual*/ void USBHostSerial::setVidPid(uint16_t vid, uint16_t pid)
{
    _vid = vid;
    _pid = pid;
}

/*virtual*/ bool USBHostSerial::parseInterface(uint8_t intf_nb, uint8_t intf_class, uint8_t intf_subclass, uint8_t intf_protocol) //Must return true if the interface should be parsed
{
    if (!ports_found &&
        CHECK_INTERFACE(intf_class, intf_subclass, intf_protocol)) {
        port_intf = intf_nb;
        ports_found = true;
        return true;
    }
    return false;
}

/*virtual*/ bool USBHostSerial::useEndpoint(uint8_t intf_nb, ENDPOINT_TYPE type, ENDPOINT_DIRECTION dir) //Must return true if the endpoint will be used
{
    if (ports_found && (intf_nb == port_intf)) {
        if (type == BULK_ENDPOINT)
            return true;
    }
    return false;
}

#else // (USBHOST_SERIAL > 1)

//------------------------------------------------------------------------------

USBHostMultiSerial::USBHostMultiSerial(uint32_t buf_size)
{
    host = USBHost::getHostInst();
    dev = NULL;
    memset(ports, NULL, sizeof(ports));
    ports_found = 0;
    _buf_size = buf_size;
}

USBHostMultiSerial::~USBHostMultiSerial()
{
    disconnect();
}

bool USBHostMultiSerial::connected()
{
    for (int port = 0; port < USBHOST_SERIAL; port++) {
        if (ports[port]->connected()) {
            return true;
        }
    }
    return false;
}

void USBHostMultiSerial::disconnect(void)
{
    for (int port = 0; port < USBHOST_SERIAL; port ++)
    {
        if (ports[port])
        {
            delete ports[port];
            ports[port] = NULL;
        }
    }
    ports_found = 0;
    dev = NULL;
}

bool USBHostMultiSerial::connect() {

    if (dev)
    {
        for (uint8_t i = 0; i < MAX_DEVICE_CONNECTED; i++)
        {
            USBDeviceConnected* d = host->getDevice(i);
            if (dev == d)
                return true;
        }
        disconnect();
    }
    for (uint8_t i = 0; i < MAX_DEVICE_CONNECTED; i++)
    {
        USBDeviceConnected* d = host->getDevice(i);
        if (d != NULL) {

            USB_DBG("Trying to connect serial device \r\n");
            if(host->enumerate(d, this))
                break;

            for (int port = 0; port < ports_found; port ++)
            {
                USBEndpoint* bulk_in  = d->getEndpoint(port_intf[port], BULK_ENDPOINT, IN);
                USBEndpoint* bulk_out = d->getEndpoint(port_intf[port], BULK_ENDPOINT, OUT);
                if (bulk_in && bulk_out)
                {
                    ports[port] = new USBHostSerialPort(_buf_size);
                    if (ports[port])
                    {
                        ports[port]->connect(host,d,port_intf[port],bulk_in, bulk_out);
                        dev = d;
                    }
                }
            }
        }
    }
    return dev != NULL;
}

/*virtual*/ void USBHostMultiSerial::setVidPid(uint16_t vid, uint16_t pid)
{
    // we don't check VID/PID for MSD driver
}

/*virtual*/ bool USBHostMultiSerial::parseInterface(uint8_t intf_nb, uint8_t intf_class, uint8_t intf_subclass, uint8_t intf_protocol) //Must return true if the interface should be parsed
{
    if ((ports_found < USBHOST_SERIAL) &&
        CHECK_INTERFACE(intf_class, intf_subclass, intf_protocol)) {
        port_intf[ports_found++] = intf_nb;
        return true;
    }
    return false;
}

/*virtual*/ bool USBHostMultiSerial::useEndpoint(uint8_t intf_nb, ENDPOINT_TYPE type, ENDPOINT_DIRECTION dir) //Must return true if the endpoint will be used
{
    if ((ports_found > 0) && (intf_nb == port_intf[ports_found-1])) {
        if (type == BULK_ENDPOINT)
            return true;
    }
    return false;
}

#endif

//------------------------------------------------------------------------------

#define SET_LINE_CODING 0x20
#define SET_CONTROL_LINE_STATE 0x22

USBHostSerialPort::USBHostSerialPort(uint32_t buf_size)
{
    p_circ_buf = new CircBufferHostSerial<uint8_t>(buf_size);
#if defined(TARGET_RZ_A2XX)
    p_buf       = (uint8_t *)AllocNonCacheMem(512);
    p_buf_out   = (uint8_t *)AllocNonCacheMem(512);
    p_buf_out_c = (uint8_t *)AllocNonCacheMem(1);
    p_line_coding = (LINE_CODING *)AllocNonCacheMem(sizeof(LINE_CODING));
#else
    p_buf = new uint8_t[512];
#endif
    init();
}

USBHostSerialPort::~USBHostSerialPort() {
    delete p_circ_buf;
#if defined(TARGET_RZ_A2XX)
    FreeNonCacheMem(p_buf);
    FreeNonCacheMem(p_buf_out);
    FreeNonCacheMem(p_buf_out_c);
#else
    delete [] p_buf;
#endif
}

void USBHostSerialPort::init(void)
{
    dev_connected = false;
    host = NULL;
    dev = NULL;
    serial_intf = 0;
    size_bulk_in = 0;
    size_bulk_out = 0;
    bulk_in = NULL;
    bulk_out = NULL;
#if defined(TARGET_RZ_A2XX)
    p_line_coding->baudrate = 9600;
    p_line_coding->data_bits = 8;
    p_line_coding->parity = None;
    p_line_coding->stop_bits = 1;
#else
    line_coding.baudrate = 9600;
    line_coding.data_bits = 8;
    line_coding.parity = None;
    line_coding.stop_bits = 1;
#endif
    p_circ_buf->flush();
}

void USBHostSerialPort::connect(USBHost* _host, USBDeviceConnected * _dev,
        uint8_t _serial_intf, USBEndpoint* _bulk_in, USBEndpoint* _bulk_out)
{
    host = _host;
    dev = _dev;
    serial_intf = _serial_intf;
    bulk_in = _bulk_in;
    bulk_out = _bulk_out;

    USB_INFO("New Serial device: VID:%04x PID:%04x [dev: %p - intf: %d]", dev->getVid(), dev->getPid(), dev, serial_intf);
    dev->setName("Serial", serial_intf);
    host->registerDriver(dev, serial_intf, this, &USBHostSerialPort::init);
    baud(9600);
    size_bulk_in = bulk_in->getSize();
    size_bulk_out = bulk_out->getSize();
#if defined(TARGET_RZ_A2XX)
    if (size_bulk_in > 512) {
        size_bulk_in = 512;
    }
    if (size_bulk_out > 512) {
        size_bulk_out = 512;
    }
#endif
    bulk_in->attach(this, &USBHostSerialPort::rxHandler);
    bulk_out->attach(this, &USBHostSerialPort::txHandler);
    host->bulkRead(dev, bulk_in, p_buf, size_bulk_in, false);
    if ((dev->getVid() == 0x1f00) && (dev->getPid() == 0x2012)) {
        host->controlWrite( dev,
                            USB_RECIPIENT_INTERFACE | USB_HOST_TO_DEVICE | USB_REQUEST_TYPE_CLASS,
                            SET_CONTROL_LINE_STATE,
                            1, serial_intf, NULL, 0);
    }
    dev_connected = true;
}

bool USBHostSerialPort::connected() {
    return dev_connected;
}

void USBHostSerialPort::rxHandler() {
    if (bulk_in) {
        int len = bulk_in->getLengthTransferred();
        if (bulk_in->getState() == USB_TYPE_IDLE) {
            for (int i = 0; i < len; i++) {
                while (p_circ_buf->isFull()) {
                    ThisThread::sleep_for(1);
                }
                p_circ_buf->queue(p_buf[i]);
            }
            if (_irq[RxIrq]) {
                _irq[RxIrq].call();
            }
            host->bulkRead(dev, bulk_in, p_buf, size_bulk_in, false);
        }
    }
}

void USBHostSerialPort::txHandler() {
    if (bulk_out) {
        if (bulk_out->getState() == USB_TYPE_IDLE) {
            if (_irq[TxIrq]) {
                _irq[TxIrq].call();
            }
        }
    }
}

int USBHostSerialPort::_putc(int c) {
    if (bulk_out) {
#if defined(TARGET_RZ_A2XX)
        *p_buf_out_c = c;
        if (host->bulkWrite(dev, bulk_out, p_buf_out_c, 1) == USB_TYPE_OK) {
#else
        if (host->bulkWrite(dev, bulk_out, (uint8_t *)&c, 1) == USB_TYPE_OK) {
#endif
            return 1;
        }
    }
    return -1;
}

void USBHostSerialPort::baud(int baudrate) {
#if defined(TARGET_RZ_A2XX)
    p_line_coding->baudrate = baudrate;
    format(p_line_coding->data_bits, (Parity)p_line_coding->parity, p_line_coding->stop_bits);
#else
    line_coding.baudrate = baudrate;
    format(line_coding.data_bits, (Parity)line_coding.parity, line_coding.stop_bits);
#endif
}

void USBHostSerialPort::format(int bits, Parity parity, int stop_bits) {
#if defined(TARGET_RZ_A2XX)
    p_line_coding->data_bits = bits;
    p_line_coding->parity = parity;
    p_line_coding->stop_bits = (stop_bits == 1) ? 0 : 2;

    // set line coding
    host->controlWrite( dev,
                        USB_RECIPIENT_INTERFACE | USB_HOST_TO_DEVICE | USB_REQUEST_TYPE_CLASS,
                        SET_LINE_CODING,
                        0, serial_intf, (uint8_t *)p_line_coding, 7);
#else
    line_coding.data_bits = bits;
    line_coding.parity = parity;
    line_coding.stop_bits = (stop_bits == 1) ? 0 : 2;

    // set line coding
    host->controlWrite( dev,
                        USB_RECIPIENT_INTERFACE | USB_HOST_TO_DEVICE | USB_REQUEST_TYPE_CLASS,
                        SET_LINE_CODING,
                        0, serial_intf, (uint8_t *)&line_coding, 7);
#endif
}

int USBHostSerialPort::_getc() {
    uint8_t c = 0;
    if (bulk_in == NULL) {
        init();
        return -1;
    }
    while (p_circ_buf->isEmpty()) {
        if (dev_connected == false) {
            return -1;
        }
        ThisThread::sleep_for(1);
    }
    p_circ_buf->dequeue(&c);
    return c;
}

int USBHostSerialPort::writeBuf(const char* b, int s) {
    int i;
    int c = 0;
    if (bulk_out) {
        while (s > 0) {
            if (dev_connected == false) {
                break;
            }
            i = ((uint32_t)s < size_bulk_out) ? s : size_bulk_out;
#if defined(TARGET_RZ_A2XX)
            memcpy(p_buf_out, (uint8_t *)(b+c), i);
            host->bulkWrite(dev, bulk_out, p_buf_out, i);
#else
            host->bulkWrite(dev, bulk_out, (uint8_t *)(b+c), i);
#endif
            c += i;
            s -= i;
        }
    }
    return c;
}

int USBHostSerialPort::readBuf(char* b, int s, int timeout) {
    int i = 0;

    if (bulk_in) {
        for (i = 0; i < s; i++) {
            while ((p_circ_buf->isEmpty()) && (dev_connected)) {
                if (timeout == 0) {
                    break;
                } else {
                    if (timeout > 0) {
                        timeout--;
                    }
                    ThisThread::sleep_for(1);
                }
            }
            if (!p_circ_buf->dequeue((uint8_t *)&b[i])) {
                break;
            }
        }
    }
    return i;
}

uint32_t USBHostSerialPort::available() {
    return p_circ_buf->available();
}

#endif


