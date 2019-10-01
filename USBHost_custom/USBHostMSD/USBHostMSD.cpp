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

#include "USBHostMSD.h"

#if USBHOST_MSD

#include "dbg.h"

#define CBW_SIGNATURE   0x43425355
#define CSW_SIGNATURE   0x53425355

#define DEVICE_TO_HOST  0x80
#define HOST_TO_DEVICE  0x00

#define GET_MAX_LUN             (0xFE)
#define BO_MASS_STORAGE_RESET   (0xFF)

USBHostMSD::USBHostMSD()
{
    host = USBHost::getHostInst();
#if defined(TARGET_RZ_A2XX)
    trans_buf = (uint8_t *)AllocNonCacheMem(512);
    result    = (uint8_t *)AllocNonCacheMem(36);
    p_cbw     = (CBW *)AllocNonCacheMem(sizeof(CBW));
    p_csw     = (CSW *)AllocNonCacheMem(sizeof(CSW));
#endif
    msd_init();
}

USBHostMSD::~USBHostMSD()
{
    if (_is_initialized) {
        deinit();
    }
#if defined(TARGET_RZ_A2XX)
    FreeNonCacheMem(trans_buf);
    FreeNonCacheMem(p_cbw);
    FreeNonCacheMem(p_csw);
#endif
}

void USBHostMSD::msd_init() {
    dev_connected = false;
    dev = NULL;
    bulk_in = NULL;
    bulk_out = NULL;
    dev_connected = false;
    blockSize = 0;
    blockCount = 0;
    msd_intf = -1;
    msd_device_found = false;
    _is_initialized = false;
    dev_connected = false;
    nb_ep = 0;
}


bool USBHostMSD::connected()
{
    return dev_connected;
}

bool USBHostMSD::connect()
{

    if (dev_connected) {
        return true;
    }

    for (uint8_t i = 0; i < MAX_DEVICE_CONNECTED; i++) {
        if ((dev = host->getDevice(i)) != NULL) {

            USB_DBG("Trying to connect MSD device\r\n");

            if(host->enumerate(dev, this))
                break;

            if (msd_device_found) {
                bulk_in = dev->getEndpoint(msd_intf, BULK_ENDPOINT, IN);
                bulk_out = dev->getEndpoint(msd_intf, BULK_ENDPOINT, OUT);

                if (!bulk_in || !bulk_out)
                    continue;

                USB_INFO("New MSD device: VID:%04x PID:%04x [dev: %p - intf: %d]", dev->getVid(), dev->getPid(), dev, msd_intf);
                dev->setName("MSD", msd_intf);
                host->registerDriver(dev, msd_intf, this, &USBHostMSD::msd_init);

                dev_connected = true;
                return true;
            }
        } //if()
    } //for()
    msd_init();
    return false;
}

/*virtual*/ void USBHostMSD::setVidPid(uint16_t vid, uint16_t pid)
{
    // we don't check VID/PID for MSD driver
}

/*virtual*/ bool USBHostMSD::parseInterface(uint8_t intf_nb, uint8_t intf_class, uint8_t intf_subclass, uint8_t intf_protocol) //Must return true if the interface should be parsed
{
    if ((msd_intf == -1) &&
        (intf_class == MSD_CLASS) &&
        (intf_subclass == 0x06) &&
        (intf_protocol == 0x50)) {
        msd_intf = intf_nb;
        return true;
    }
    return false;
}

/*virtual*/ bool USBHostMSD::useEndpoint(uint8_t intf_nb, ENDPOINT_TYPE type, ENDPOINT_DIRECTION dir) //Must return true if the endpoint will be used
{
    if (intf_nb == msd_intf) {
        if (type == BULK_ENDPOINT) {
            nb_ep++;
            if (nb_ep == 2)
                msd_device_found = true;
            return true;
        }
    }
    return false;
}


int USBHostMSD::testUnitReady() {
    USB_DBG("Test unit ready");
    return SCSITransfer(NULL, 6, DEVICE_TO_HOST, 0, 0);
}


int USBHostMSD::readCapacity() {
    USB_DBG("Read capacity");
    uint8_t cmd[10] = {0x25,0,0,0,0,0,0,0,0,0};
#if defined(TARGET_RZ_A2XX)
#else
    uint8_t result[8];
#endif
    int status = SCSITransfer(cmd, 10, DEVICE_TO_HOST, result, 8);
    if (status == 0) {
        blockCount = (result[0] << 24) | (result[1] << 16) | (result[2] << 8) | result[3];
        blockSize = (result[4] << 24) | (result[5] << 16) | (result[6] << 8) | result[7];
        USB_INFO("MSD [dev: %p] - blockCount: %ld, blockSize: %d, Capacity: %ld\r\n", dev, blockCount, blockSize, blockCount*blockSize);
    }
    return status;
}


int USBHostMSD::SCSIRequestSense() {
    USB_DBG("Request sense");
    uint8_t cmd[6] = {0x03,0,0,0,18,0};
#if defined(TARGET_RZ_A2XX)
#else
    uint8_t result[18];
#endif
    int status = SCSITransfer(cmd, 6, DEVICE_TO_HOST, result, 18);
    return status;
}


int USBHostMSD::inquiry(uint8_t lun, uint8_t page_code) {
    USB_DBG("Inquiry");
    uint8_t evpd = (page_code == 0) ? 0 : 1;
    uint8_t cmd[6] = {0x12, uint8_t((lun << 5) | evpd), page_code, 0, 36, 0};
#if defined(TARGET_RZ_A2XX)
#else
    uint8_t result[36];
#endif
    int status = SCSITransfer(cmd, 6, DEVICE_TO_HOST, result, 36);
    if (status == 0) {
        char vid_pid[17];
        memcpy(vid_pid, &result[8], 8);
        vid_pid[8] = 0;
        USB_INFO("MSD [dev: %p] - Vendor ID: %s", dev, vid_pid);

        memcpy(vid_pid, &result[16], 16);
        vid_pid[16] = 0;
        USB_INFO("MSD [dev: %p] - Product ID: %s", dev, vid_pid);

        memcpy(vid_pid, &result[32], 4);
        vid_pid[4] = 0;
        USB_INFO("MSD [dev: %p] - Product rev: %s", dev, vid_pid);
    }
    return status;
}

int USBHostMSD::checkResult(uint8_t res, USBEndpoint * ep) {
    // if ep stalled: send clear feature
    if (res == USB_TYPE_STALL_ERROR) {
        res = host->controlWrite(   dev,
                                    USB_RECIPIENT_ENDPOINT | USB_HOST_TO_DEVICE | USB_REQUEST_TYPE_STANDARD,
                                    CLEAR_FEATURE,
                                    0, ep->getAddress(), NULL, 0);
        // set state to IDLE if clear feature successful
        if (res == USB_TYPE_OK) {
            ep->setState(USB_TYPE_IDLE);
        }
    }

    if (res != USB_TYPE_OK)
        return -1;

    return 0;
}


int USBHostMSD::SCSITransfer(uint8_t * cmd, uint8_t cmd_len, int flags, uint8_t * data, uint32_t transfer_len) {

    int res = 0;

#if defined(TARGET_RZ_A2XX)
    p_cbw->Signature = CBW_SIGNATURE;
    p_cbw->Tag = 0;
    p_cbw->DataLength = transfer_len;
    p_cbw->Flags = flags;
    p_cbw->LUN = 0;
    p_cbw->CBLength = cmd_len;
    memset(p_cbw->CB,0,sizeof(p_cbw->CB));
    if (cmd) {
        memcpy(p_cbw->CB,cmd,cmd_len);
    }

    // send the cbw
    USB_DBG("Send CBW");
    res = host->bulkWrite(dev, bulk_out,(uint8_t *)p_cbw, 31);
#else
    cbw.Signature = CBW_SIGNATURE;
    cbw.Tag = 0;
    cbw.DataLength = transfer_len;
    cbw.Flags = flags;
    cbw.LUN = 0;
    cbw.CBLength = cmd_len;
    memset(cbw.CB,0,sizeof(cbw.CB));
    if (cmd) {
        memcpy(cbw.CB,cmd,cmd_len);
    }

    // send the cbw
    USB_DBG("Send CBW");
    res = host->bulkWrite(dev, bulk_out,(uint8_t *)&cbw, 31);
#endif
    if (checkResult(res, bulk_out))
        return -1;

    // data stage if needed
    if (data) {
        USB_DBG("data stage");
        if (flags == HOST_TO_DEVICE) {

            res = host->bulkWrite(dev, bulk_out, data, transfer_len);
            if (checkResult(res, bulk_out))
                return -1;

        } else if (flags == DEVICE_TO_HOST) {

            res = host->bulkRead(dev, bulk_in, data, transfer_len);
            if (checkResult(res, bulk_in))
                return -1;
        }
    }

    // status stage
    uint8_t wk_cmd0 = 0;
    if (cmd) {
        wk_cmd0 = cmd[0];
    }
#if defined(TARGET_RZ_A2XX)
    p_csw->Signature = 0;
    USB_DBG("Read CSW");
    res = host->bulkRead(dev, bulk_in,(uint8_t *)p_csw, 13);
    if (checkResult(res, bulk_in))
        return -1;

    if (p_csw->Signature != CSW_SIGNATURE) {
        return -1;
    }

    USB_DBG("recv csw: status: %d", p_csw->Status);

    // ModeSense?
    if ((p_csw->Status == 1) && (wk_cmd0 != 0x03)) {
        USB_DBG("request mode sense");
        return SCSIRequestSense();
    }

    // perform reset recovery
    if ((p_csw->Status == 2) && (wk_cmd0 != 0x03)) {

        // send Bulk-Only Mass Storage Reset request
        res = host->controlWrite(   dev,
                                    USB_RECIPIENT_INTERFACE | USB_HOST_TO_DEVICE | USB_REQUEST_TYPE_CLASS,
                                    BO_MASS_STORAGE_RESET,
                                    0, msd_intf, NULL, 0);

        // unstall both endpoints
        res = host->controlWrite(   dev,
                                    USB_RECIPIENT_ENDPOINT | USB_HOST_TO_DEVICE | USB_REQUEST_TYPE_STANDARD,
                                    CLEAR_FEATURE,
                                    0, bulk_in->getAddress(), NULL, 0);

        res = host->controlWrite(   dev,
                                    USB_RECIPIENT_ENDPOINT | USB_HOST_TO_DEVICE | USB_REQUEST_TYPE_STANDARD,
                                    CLEAR_FEATURE,
                                    0, bulk_out->getAddress(), NULL, 0);

    }

    return p_csw->Status;
#else
    csw.Signature = 0;
    USB_DBG("Read CSW");
    res = host->bulkRead(dev, bulk_in,(uint8_t *)&csw, 13);
    if (checkResult(res, bulk_in))
        return -1;

    if (csw.Signature != CSW_SIGNATURE) {
        return -1;
    }

    USB_DBG("recv csw: status: %d", csw.Status);

    // ModeSense?
    if ((csw.Status == 1) && (wk_cmd0 != 0x03)) {
        USB_DBG("request mode sense");
        return SCSIRequestSense();
    }

    // perform reset recovery
    if ((csw.Status == 2) && (wk_cmd0 != 0x03)) {

        // send Bulk-Only Mass Storage Reset request
        res = host->controlWrite(   dev,
                                    USB_RECIPIENT_INTERFACE | USB_HOST_TO_DEVICE | USB_REQUEST_TYPE_CLASS,
                                    BO_MASS_STORAGE_RESET,
                                    0, msd_intf, NULL, 0);

        // unstall both endpoints
        res = host->controlWrite(   dev,
                                    USB_RECIPIENT_ENDPOINT | USB_HOST_TO_DEVICE | USB_REQUEST_TYPE_STANDARD,
                                    CLEAR_FEATURE,
                                    0, bulk_in->getAddress(), NULL, 0);

        res = host->controlWrite(   dev,
                                    USB_RECIPIENT_ENDPOINT | USB_HOST_TO_DEVICE | USB_REQUEST_TYPE_STANDARD,
                                    CLEAR_FEATURE,
                                    0, bulk_out->getAddress(), NULL, 0);

    }

    return csw.Status;
#endif
}


int USBHostMSD::dataTransfer(uint8_t * buf, uint32_t block, uint8_t nbBlock, int direction) {
    uint8_t cmd[10];
    memset(cmd,0,10);
    cmd[0] = (direction == DEVICE_TO_HOST) ? 0x28 : 0x2A;

    cmd[2] = (block >> 24) & 0xff;
    cmd[3] = (block >> 16) & 0xff;
    cmd[4] = (block >> 8) & 0xff;
    cmd[5] =  block & 0xff;

    cmd[7] = (nbBlock >> 8) & 0xff;
    cmd[8] = nbBlock & 0xff;

    return SCSITransfer(cmd, 10, direction, buf, blockSize*nbBlock);
}

int USBHostMSD::getMaxLun() {
#if defined(TARGET_RZ_A2XX)
    uint8_t res;
    res = host->controlRead(    dev, USB_RECIPIENT_INTERFACE | USB_DEVICE_TO_HOST | USB_REQUEST_TYPE_CLASS,
                                0xfe, 0, msd_intf, trans_buf, 1);
    USB_DBG("max lun: %d", trans_buf[0]);
#else
    uint8_t buf[1], res;
    res = host->controlRead(    dev, USB_RECIPIENT_INTERFACE | USB_DEVICE_TO_HOST | USB_REQUEST_TYPE_CLASS,
                                0xfe, 0, msd_intf, buf, 1);
    USB_DBG("max lun: %d", buf[0]);
#endif
    return res;
}

#define USB_BLOCK_DEVICE_ERROR_WOULD_BLOCK        -5001 /*!< operation would block */
#define USB_BLOCK_DEVICE_ERROR_UNSUPPORTED        -5002 /*!< unsupported operation */
#define USB_BLOCK_DEVICE_ERROR_PARAMETER          -5003 /*!< invalid parameter */
#define USB_BLOCK_DEVICE_ERROR_NO_INIT            -5004 /*!< uninitialized */
#define USB_BLOCK_DEVICE_ERROR_NO_DEVICE          -5005 /*!< device is missing or not connected */
#define USB_BLOCK_DEVICE_ERROR_WRITE_PROTECTED    -5006 /*!< write protected */

int USBHostMSD::init() {
    uint16_t i, timeout = 10;

    _lock.lock();
    getMaxLun();

    for (i = 0; i < timeout; i++) {
        ThisThread::sleep_for(100);
        if (!testUnitReady())
            break;
    }

    if (i == timeout) {
        return BD_ERROR_DEVICE_ERROR;
    }

    inquiry(0, 0);
    if (readCapacity() != 0) {
        _lock.unlock();
        return BD_ERROR_DEVICE_ERROR;
    }
    _is_initialized = true;
    _lock.unlock();

    return BD_ERROR_OK;
}

int USBHostMSD::deinit() {
    return 0;
}

int USBHostMSD::program(const void *b, bd_addr_t addr, bd_size_t size)
{
    if (!is_valid_program(addr, size)) {
        return USB_BLOCK_DEVICE_ERROR_PARAMETER;
    }

    _lock.lock();
    if (!_is_initialized) {
        _lock.unlock();
        return USB_BLOCK_DEVICE_ERROR_NO_INIT;
    }

    const uint8_t *buffer = static_cast<const uint8_t*>(b);
    while (size > 0) {
        bd_addr_t block = addr / 512;

        // send the data block
#if defined(TARGET_RZ_A2XX)
        (void)memcpy(trans_buf, buffer, 512);
        if (dataTransfer((uint8_t*)trans_buf, block, 1, HOST_TO_DEVICE)) {
#else
        if (dataTransfer((uint8_t*)buffer, block, 1, HOST_TO_DEVICE)) {
#endif
            _lock.unlock();
            return BD_ERROR_DEVICE_ERROR;
        }

        buffer += 512;
        addr += 512;
        size -= 512;
    }
    _lock.unlock();
    return 0;
}

int USBHostMSD::read(void *b, bd_addr_t addr, bd_size_t size)
{
    if (!is_valid_read(addr, size)) {
        return USB_BLOCK_DEVICE_ERROR_PARAMETER;
    }

    _lock.lock();
    if (!_is_initialized) {
        _lock.unlock();
        return USB_BLOCK_DEVICE_ERROR_PARAMETER;
    }

    uint8_t *buffer = static_cast<uint8_t *>(b);
    while (size > 0) {
        bd_addr_t block = addr / 512;

        // receive the data
#if defined(TARGET_RZ_A2XX)
        if (dataTransfer((uint8_t*)trans_buf, block, 1, DEVICE_TO_HOST)) {
            _lock.unlock();
            return BD_ERROR_DEVICE_ERROR;
        }
        (void)memcpy(buffer, trans_buf, 512);
#else
        if (dataTransfer((uint8_t*)buffer, block, 1, DEVICE_TO_HOST)) {
            _lock.unlock();
            return BD_ERROR_DEVICE_ERROR;
        }
#endif
        buffer += 512;
        addr += 512;
        size -= 512;
    }
    _lock.unlock();
    return 0;
}

int USBHostMSD::erase(bd_addr_t addr, bd_size_t size)
{
    return 0;
}

bd_size_t USBHostMSD::get_read_size() const
{
    return 512;
}

bd_size_t USBHostMSD::get_program_size() const
{
    return 512;
}

bd_size_t USBHostMSD::get_erase_size() const
{
    return 512;
}

bd_size_t USBHostMSD::size() const
{
    bd_size_t sectors = 0;
    if(_is_initialized) {
        sectors = blockCount;
    }
    return 512*sectors;
}

const char *USBHostMSD::get_type() const
{
    return "MSD";
}

void USBHostMSD::debug(bool dbg)
{
//    _dbg = dbg;
}

#endif

