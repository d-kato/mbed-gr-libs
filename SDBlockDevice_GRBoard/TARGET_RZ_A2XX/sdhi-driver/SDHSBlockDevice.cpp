/******************************************************************************
* DISCLAIMER

* This software is supplied by Renesas Electronics Corporation and is only 
* intended for use with Renesas products. No other uses are authorized.

* This software is owned by Renesas Electronics Corporation and is protected under 
* all applicable laws, including copyright laws.

* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES 
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, 
* INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
* PARTICULAR PURPOSE AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY 
* DISCLAIMED.

* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES 
* FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS 
* AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

* Renesas reserves the right, without notice, to make changes to this 
* software and to discontinue the availability of this software.  
* By using this software, you agree to the additional terms and 
* conditions found by accessing the following link:
* http://www.renesas.com/disclaimer
******************************************************************************/
/* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.  */

#include <string.h>
#include "SDHSBlockDevice.h"
#include "r_typedefs.h"
#include "r_sd_cfg.h"
#include "r_sdif.h"
#include "pinmap.h"

#include "dcache-control.h"

#define SD_SECTOR_SIZE      (512)
#define SD_RW_BUFF_SIZE     (1 * 1024)

static uint8_t sd_work_hndl_buff[SD_SIZE_OF_INIT]__attribute((section("NC_BSS"),aligned(32)));
static uint8_t sd_work_rw_buff[SD_RW_BUFF_SIZE]__attribute((section("NC_BSS"),aligned(32)));
static uint8_t sd_work_buff[SD_SECTOR_SIZE]__attribute((aligned(32)));

static const uint32_t sd_base_addr[2] = {
     SD_CFG_IP0_BASE
    ,SD_CFG_IP1_BASE
};

SDHSBlockDevice::SDHSBlockDevice(PinName cd, PinName wp) : sd_ch(-1)
{
    /* Initialize SD driver. */
    static const PinMap PinMap_SD_CD[] = {
    //   pin      ch     func
        {P6_4   , 1    , 5},
        {P5_4   , 1    , 3},
        {PC_6   , 1    , 4},
        {P5_0   , 1    , 4},
        {NC     , NC   , 0}
    };

    static const PinMap PinMap_SD_WP[] = {
    //   pin      ch     func
        {P9_7   , 1    , 5},
        {PC_7   , 1    , 4},
        {P5_1   , 1    , 4},
        {P5_5   , 1    , 3},
        {NC     , NC   , 0}
    };

    int32_t sd_cd = pinmap_peripheral(cd, PinMap_SD_CD);
    int32_t sd_wp = pinmap_peripheral(wp, PinMap_SD_WP);
    int32_t chk;

    sd_info.type = SD_MEDIA_UNKNOWN;
    sd_info.iswp = SD_WP_OFF;

    if (sd_cd == NC) {
        sd_ch = -1;
        return;
    }

    if (sd_wp != NC) {
        pinmap_pinout(wp, PinMap_SD_WP);
    }

    sd_ch = sd_cd;
    pinmap_pinout(cd, PinMap_SD_CD);
    chk = sd_init((int32_t)sd_ch, sd_base_addr[sd_ch], sd_work_hndl_buff, SD_CD_SOCKET);
    if (chk != SD_OK) {
        sd_ch = -1;
        return;
    }
    /* Set the card detect interrupt. */
    chk = sd_cd_int(sd_ch, SD_CD_INT_ENABLE, NULL);
    if (chk != SD_OK) {
        sd_ch = -1;
        return;
    }
    /* Register callback function. */
    chk = sd_set_intcallback(sd_ch, &SD_status_callback_function);
    if (chk != SD_OK){
        sd_ch = -1;
        return;
    }
    chk = sd_set_dma_intcallback(sd_ch, &SD_dma_end_callback_function);
    if (chk != SD_OK){
        sd_ch = -1;
        return;
    }
    /* Initialize SD driver work buffer. */
    chk = sd_set_buffer(sd_ch, sd_work_rw_buff, SD_RW_BUFF_SIZE);
    if (chk != SD_OK) {
        sd_ch = -1;
        return;
    }
}

SDHSBlockDevice::~SDHSBlockDevice()
{
    deinit();
}

bool SDHSBlockDevice::is_connect() {
    if (sd_check_media(sd_ch) == SD_OK) {
        return true;
    }
    return false;
}

int SDHSBlockDevice::init()
{
    int32_t     chk;
    uint16_t    type;

    lock();

    if (sd_ch < 0){
        unlock();
        return BD_ERROR_DEVICE_ERROR;
    }

    /* Initialize card information. */
    sd_info.type = SD_MEDIA_UNKNOWN;
    sd_info.iswp = SD_WP_OFF;

    /* Check if the card is inserted. */
    chk = sd_check_media(sd_ch);
    if (chk != SD_OK) {
        unlock();
        return BD_ERROR_DEVICE_ERROR;
    } else {
        /* Mount SD card. */
        chk = sd_mount(sd_ch, SD_CFG_DRIVER_MODE, SD_VOLT_3_3);
        if (chk != SD_OK) {
            unlock();
            return BD_ERROR_DEVICE_ERROR;
        }
        /* Get the card type. */
        chk = sd_get_type(sd_ch, &type, NULL, NULL);
        if (chk != SD_OK) {
            unlock();
            return BD_ERROR_DEVICE_ERROR;
        }
        sd_info.type = type;
        /* Get write protected information. */
        chk = sd_iswp(sd_ch);
        if (chk == SD_ERR) {
            unlock();
            return BD_ERROR_DEVICE_ERROR;
        }
        sd_info.iswp = chk;
    }

    unlock();
    return BD_ERROR_OK;
}

int SDHSBlockDevice::deinit()
{
    if (sd_ch < 0){
        return BD_ERROR_DEVICE_ERROR;
    }

    sd_unmount(sd_ch);
    sd_finalize(sd_ch);
    sd_info.type = SD_MEDIA_UNKNOWN;
    sd_info.iswp = SD_WP_OFF;

    return BD_ERROR_OK;
}

bd_size_t SDHSBlockDevice::get_read_size() const
{
    return SD_SECTOR_SIZE;
}

bd_size_t SDHSBlockDevice::get_program_size() const
{
    return SD_SECTOR_SIZE;
}

bd_size_t SDHSBlockDevice::size() const
{
    uint32_t size = 0;

    if (sd_ch < 0){
        return 0;
    }
    sd_get_size(sd_ch, &size, NULL);

    return (bd_size_t)(size * SD_SECTOR_SIZE);
}

const char *SDHSBlockDevice::get_type() const
{
    if (sd_info.type == SD_MEDIA_MMC) {
        return "MMC";
    } else {
        return "SD";
    }
}

int SDHSBlockDevice::read(void *b, bd_addr_t addr, bd_size_t size)
{
    MBED_ASSERT(is_valid_read(addr, size));

    int32_t  chk;
    uint32_t psn = (addr / SD_SECTOR_SIZE);
    int32_t  sec_cnt = (size / SD_SECTOR_SIZE);

    if (sd_ch < 0){
        return BD_ERROR_DEVICE_ERROR;
    }

    if (sec_cnt == 0) {
        return BD_ERROR_OK;
    }

    if (((uint32_t)b & 0x0000001F) == 0) {
        // aligned 32
        dcache_invalid((uint8_t *)b, size);
        lock();
        chk = sd_read_sect(sd_ch, (uint8_t *)b, psn, sec_cnt);
        if (chk != SD_OK) {
            unlock();
            return BD_ERROR_DEVICE_ERROR;
        }
        unlock();
    } else {
        // not aligned 32
        if (sec_cnt >= 3) {
            dcache_invalid((uint8_t *)((uint32_t)b + SD_SECTOR_SIZE), (sec_cnt - 2) * SD_SECTOR_SIZE);
        }

        lock();
        dcache_invalid(sd_work_buff, SD_SECTOR_SIZE);
        chk = sd_read_sect(sd_ch, sd_work_buff, psn, 1);
        if (chk != SD_OK) {
            unlock();
            return BD_ERROR_DEVICE_ERROR;
        }
        memcpy((uint8_t *)b, sd_work_buff, SD_SECTOR_SIZE);
        b = (void *)((uint32_t)b + SD_SECTOR_SIZE);
        psn++;
        sec_cnt--;

        if (sec_cnt >= 2) {
            chk = sd_read_sect(sd_ch, (uint8_t *)b, psn, sec_cnt - 1);
            if (chk != SD_OK) {
                unlock();
                return BD_ERROR_DEVICE_ERROR;
            }
            b = (void *)((uint32_t)b + (SD_SECTOR_SIZE * (sec_cnt - 1)));
            psn += (sec_cnt - 1);
            sec_cnt = 1;
        }

        if (sec_cnt >= 1) {
            dcache_invalid(sd_work_buff, SD_SECTOR_SIZE);
            chk = sd_read_sect(sd_ch, sd_work_buff, psn, 1);
            if (chk != SD_OK) {
                unlock();
                return BD_ERROR_DEVICE_ERROR;
            }
            memcpy((uint8_t *)b, sd_work_buff, SD_SECTOR_SIZE);
        }
        unlock();
    }

    return BD_ERROR_OK;
}

int SDHSBlockDevice::program(const void *b, bd_addr_t addr, bd_size_t size)
{
    MBED_ASSERT(is_valid_program(addr, size));

    int32_t  chk;
    uint32_t psn = (addr / SD_SECTOR_SIZE);
    int32_t  sec_cnt = (size / SD_SECTOR_SIZE);

    if (sd_ch < 0){
        return BD_ERROR_DEVICE_ERROR;
    }

    if (sd_info.iswp != SD_WP_OFF) {
        return BD_ERROR_DEVICE_ERROR;
    }

    if (sec_cnt == 0) {
        return BD_ERROR_OK;
    }

    if (((uint32_t)b & 0x0000001F) == 0) {
        // aligned 32
        dcache_clean((uint8_t *)b, size);
        lock();
        chk = sd_write_sect(sd_ch, (uint8_t *)b, psn, sec_cnt, SD_WRITE_OVERWRITE);
        if (chk != SD_OK) {
            unlock();
            return BD_ERROR_DEVICE_ERROR;
        }
        unlock();
    } else {
        // not aligned 32
        if (sec_cnt >= 3) {
            dcache_clean((uint8_t *)((uint32_t)b + SD_SECTOR_SIZE), (sec_cnt - 2) * SD_SECTOR_SIZE);
        }

        lock();
        memcpy(sd_work_buff, (uint8_t *)b, SD_SECTOR_SIZE);
        dcache_clean(sd_work_buff, SD_SECTOR_SIZE);
        chk = sd_write_sect(sd_ch, sd_work_buff, psn, 1, SD_WRITE_OVERWRITE);
        if (chk != SD_OK) {
            unlock();
            return BD_ERROR_DEVICE_ERROR;
        }
        b = (void *)((uint32_t)b + SD_SECTOR_SIZE);
        psn++;
        sec_cnt--;

        if (sec_cnt >= 2) {
            chk = sd_write_sect(sd_ch, (uint8_t *)b, psn, sec_cnt - 1, SD_WRITE_OVERWRITE);
            if (chk != SD_OK) {
                unlock();
                return BD_ERROR_DEVICE_ERROR;
            }
            b = (void *)((uint32_t)b + (SD_SECTOR_SIZE * (sec_cnt - 1)));
            psn += (sec_cnt - 1);
            sec_cnt = 1;
        }

        if (sec_cnt >= 1) {
            memcpy(sd_work_buff, (uint8_t *)b, SD_SECTOR_SIZE);
            dcache_clean(sd_work_buff, SD_SECTOR_SIZE);
            chk = sd_write_sect(sd_ch, sd_work_buff, psn, 1, SD_WRITE_OVERWRITE);
            if (chk != SD_OK) {
                unlock();
                return BD_ERROR_DEVICE_ERROR;
            }
        }
        unlock();
    }

    return BD_ERROR_OK;
}

