/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer*
* Copyright (C) 2015 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/

#include "r_bsp_cmn.h"
#include "R_BSP_SerialFamily.h"
#include "posix_types.h"
#include "r_errno.h"

#define PATH_NAME_LENGTH            (3)      /* Path name length */
#define PATH_TOP_PART               (0)      /* Path top part */
#define CH_ID_PART                  (1)      /* Channel identifier part */
#define TERM_CHAR_PART              (2)      /* Termination character part */
#define PATH_SEPARATOR              ('\\')   /* Path separator */
#define CHARACTER_0                 (0x30)   /* character 0 */

R_BSP_SerialFamily::~R_BSP_SerialFamily() {
    if (functions != NULL) {
        if ((ch_handle != NULL) && (functions->close != NULL)) {
            (void)functions->close(ch_handle, NULL);
        }
        if ((ch_no != -1) && (instance != NULL) && (functions->uninitialise_one != NULL)) {
            (void)functions->uninitialise_one(ch_no, instance, NULL);
        }
    }
}

bool R_BSP_SerialFamily::init_channel(RBSP_MBED_FNS * function_list, int channel, void * const config_data,
                                      int32_t max_write_num, int32_t max_read_num) {
    bool    ret = false;
    int32_t wk_errno;
    char_t  path_name[PATH_NAME_LENGTH];
    int_t   flags;

    if ((max_write_num > 0) && (max_read_num > 0)) {
        flags = O_RDWR_RBSP;
    } else if (max_write_num > 0) {
        flags = O_WRONLY_RBSP;
    } else if (max_read_num > 0) {
        flags = O_RDONLY_RBSP;
    } else {
        flags = 0;
    }

    if ((channel >= 0) && (channel <= 9) && (function_list != NULL) && (flags != 0)) {
        ch_no     = channel;
        functions = function_list;

        if ((functions->initialise_one != NULL) && (functions->open != NULL)) {
            R_BSP_CMN_Init();
            instance = functions->initialise_one(ch_no, config_data, &wk_errno);
            if ((int32_t)instance != EERROR) {
                path_name[PATH_TOP_PART]  = PATH_SEPARATOR;
                path_name[CH_ID_PART]     = CHARACTER_0 + ch_no;
                path_name[TERM_CHAR_PART] = '\0';
                ch_handle = (void *)functions->open(instance, path_name, flags, 0, &wk_errno);
                if ((int32_t)ch_handle != EERROR) {
                    if (functions->write_a != NULL) {
                        write_init(ch_handle, (void *)functions->write_a, max_write_num);
                    }
                    if (functions->read_a != NULL) {
                        read_init(ch_handle, (void *)functions->read_a, max_read_num);
                    }
                    ret = true;
                } else {
                    (void)((RBSP_MBED_FNS *)functions)->uninitialise_one(ch_no, instance, NULL);
                }
            }
        }
    }

    if (ret == false) {
        functions = NULL;
        instance  = NULL;
        ch_handle = NULL;
        ch_no     = -1;
    }

    return ret;
}

bool R_BSP_SerialFamily::ioctl(int request, ...) {
    bool    ret = true;
    int32_t wk_errno;
    void *buf;
    va_list ap;

    if (functions == NULL) {
        ret = false;
    } else if (functions->ioctl == NULL) {
        ret = false;
    } else {
        va_start(ap, request);
        buf = (void *)va_arg(ap, void*);
        functions->ioctl(ch_handle, request, buf, &wk_errno);
        va_end(ap);
        if (wk_errno != ESUCCESS) {
            ret = false;
        }
    }

    return ret;
}
