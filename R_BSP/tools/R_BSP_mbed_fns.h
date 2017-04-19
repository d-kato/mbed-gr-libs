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


#ifndef R_BSP_MBED_FNS_H
#define R_BSP_MBED_FNS_H

/******************************************************************************
Includes <System Includes> , "Project Includes"
******************************************************************************/
#include "r_typedefs.h"
#include "ioif_aio.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef struct {
    void *(*initialise_one)(const int_t channel, const void* const config_data, int32_t* const p_errno);
    int_t (*uninitialise_one)(const int_t channel, const void* const driver_instance, int32_t* const p_errno);
    int_t (*open)(void *driver_instance, const char_t *pathname, int_t flags, int_t mode, int32_t* p_errno);
    int_t (*close)(void *fd, int32_t* p_errno);
    int_t (*ioctl)(void *fd, int_t request, void *buf, int32_t* p_errno);
    int_t (*read_a)(void *fd, struct aiocb *p_aio, int32_t* p_errno);
    int_t (*write_a)(void *fd, struct aiocb *p_aio, int32_t* p_errno);
    int_t (*cancel)(void *fd, struct aiocb *p_aio, int32_t* p_errno);
} RBSP_MBED_FNS;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* R_BSP_MBED_FNS_H */
