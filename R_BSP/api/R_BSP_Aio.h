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
/**************************************************************************//**
* @file          R_BSP_Aio.h
* @brief         R_BSP_Aio API
******************************************************************************/

#ifndef R_BSP_AIO_H
#define R_BSP_AIO_H

#include <stdint.h>
#include "rtos.h"

/** Callback function type 
  *
  * @param p_data Location of the data.
  * @param result Number of bytes transmit on success. negative number on error.
  * @param p_app_data User definition data.
  */
typedef void (*rbsp_notify_func_t)(void * p_data, int32_t result, void * p_app_data);

/** Asynchronous control block structure */
typedef struct {
    rbsp_notify_func_t  p_notify_func;  /**< Callback function type. */
    void *              p_app_data;     /**< User definition data. */
} rbsp_data_conf_t;

/**
 * A class to communicate a R_BSP_Aio
 */
class R_BSP_Aio {

public:

    /** Write count bytes to the file associated
     *
     * @param p_data Location of the data.
     * @param data_size Number of bytes to write.
     * @param p_data_conf Asynchronous control block structure.
     * @return Number of bytes written on success. negative number on error.
     */
    int32_t write(void * const p_data, uint32_t data_size, const rbsp_data_conf_t * const p_data_conf = NULL);

    /** Read count bytes to the file associated
     *
     * @param p_data Location of the data.
     * @param data_size Number of bytes to read.
     * @param p_data_conf Asynchronous control block structure.
     * @return Number of bytes read on success. negative number on error.
     */
    int32_t read(void * const p_data, uint32_t data_size, const rbsp_data_conf_t * const p_data_conf = NULL);

protected:

    /** Constructor
     *
     */
    R_BSP_Aio();

    /** Destructor
     *
     */
    virtual ~R_BSP_Aio();

    /** Write init
     *
     * @param handle channel handle.
     * @param p_func_a Pointer of write function.
     * @param max_buff_num The upper limit of write buffer.
     */
    void write_init(void * handle, void * p_func_a, int32_t max_buff_num = 16) {
        init(&write_ctl, handle, p_func_a, max_buff_num);
    };

    /** Read init
     *
     * @param handle channel handle.
     * @param p_func_a Pointer of read function.
     * @param max_buff_num The upper limit of read buffer.
     */
    void read_init(void * handle, void * p_func_a, int32_t max_buff_num = 16) {
        init(&read_ctl, handle, p_func_a, max_buff_num);
    };
private:
    typedef struct {
        rbsp_notify_func_t  p_cb_func;
        void *              p_cb_data;
        Semaphore *         p_sem;
        void *              p_aio;
    } rbsp_sival_t;

    typedef struct {
        void *              ch_handle;
        void *              p_async_func;
        void *              p_aio_top;
        int32_t             index;
        rbsp_sival_t *      p_sival_top;
        Semaphore *         p_sem_ctl;
        int32_t             MaxNum;
    } rbsp_serial_ctl_t;

    typedef struct {
        Semaphore *         p_sem;
        int32_t             result;
    } rbsp_sync_t;

    void init(rbsp_serial_ctl_t * p_ctl, void * handle, void * p_func_a, int32_t max_buff_num);
    static int32_t sync_trans(rbsp_serial_ctl_t * p_ctl, void * const p_data, uint32_t data_size);
    static void callback_sync_trans(void * p_data, int32_t result, void * p_app_data);
    static int32_t aio_trans(rbsp_serial_ctl_t * const p_ctl, void * const p_data, uint32_t data_size,
                             const rbsp_data_conf_t * const p_data_conf);
    static void callback_aio_trans(union sigval signo);

    rbsp_serial_ctl_t write_ctl;
    rbsp_serial_ctl_t read_ctl;
};
#endif
