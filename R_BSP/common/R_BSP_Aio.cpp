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

#include "cmsis_os.h"
#include "r_typedefs.h"
#include "r_errno.h"
#include "aioif.h"
#include "R_BSP_Aio.h"

typedef int32_t (*rbsp_read_write_a_func_t)(void* const p_fd, AIOCB* const p_aio, int32_t* const p_errno);

R_BSP_Aio::R_BSP_Aio() {
}

R_BSP_Aio::~R_BSP_Aio() {
    if (write_ctl.MaxNum != 0) {
        delete [] (rbsp_sival_t *)write_ctl.p_sival_top;
        delete [] (AIOCB *)write_ctl.p_aio_top;
        delete write_ctl.p_sem_ctl;
    }
    if (read_ctl.MaxNum != 0) {
        delete [] (rbsp_sival_t *)read_ctl.p_sival_top;
        delete [] (AIOCB *)read_ctl.p_aio_top;
        delete read_ctl.p_sem_ctl;
    }
}

void R_BSP_Aio::init(rbsp_serial_ctl_t * p_ctl, void * handle, void * p_func_a, int32_t max_buff_num) {
    if (handle != NULL) {
        p_ctl->ch_handle    = handle;
        p_ctl->MaxNum       = max_buff_num;
        p_ctl->p_aio_top    = NULL;
        p_ctl->index        = 0;
        p_ctl->p_async_func = p_func_a;
        if (p_ctl->MaxNum != 0) {
            p_ctl->p_sem_ctl    = new Semaphore(p_ctl->MaxNum);
            p_ctl->p_aio_top    = new AIOCB[p_ctl->MaxNum];
            p_ctl->p_sival_top  = new rbsp_sival_t[p_ctl->MaxNum];
        }
    }
}

int32_t R_BSP_Aio::write(void * const p_data, uint32_t data_size, const rbsp_data_conf_t * const p_data_conf) {
    if (p_data_conf == NULL) {
        return sync_trans(&write_ctl, p_data, data_size);
    } else {
        return aio_trans(&write_ctl, p_data, data_size, p_data_conf);
    }
}

int32_t R_BSP_Aio::read(void * const p_data, uint32_t data_size, const rbsp_data_conf_t * const p_data_conf) {
    if (p_data_conf == NULL) {
        return sync_trans(&read_ctl, p_data, data_size);
    } else {
        return aio_trans(&read_ctl, p_data, data_size, p_data_conf);
    }
}

/* static */ int32_t R_BSP_Aio::sync_trans(rbsp_serial_ctl_t * p_ctl, void * const p_data, uint32_t data_size) {
    rbsp_sync_t sync_info;
    rbsp_data_conf_t data_conf;
    Semaphore * p_sem_wait = new Semaphore(0);

    sync_info.result = -1;
    if (p_sem_wait != NULL) {
        sync_info.p_sem  = p_sem_wait;

        data_conf.p_notify_func = &callback_sync_trans;
        data_conf.p_app_data    = &sync_info;

        if (aio_trans(p_ctl, p_data, data_size, &data_conf) == ESUCCESS) {
            p_sem_wait->wait(osWaitForever);
        }
        delete p_sem_wait;
    }

    return sync_info.result;
}

/* static */ void R_BSP_Aio::callback_sync_trans(void * p_data, int32_t result, void * p_app_data) {
    rbsp_sync_t * p_sync_info = (rbsp_sync_t *)p_app_data;

    p_sync_info->result = result;
    p_sync_info->p_sem->release();
}

/* static */ int32_t R_BSP_Aio::aio_trans(rbsp_serial_ctl_t * const p_ctl,
               void * const p_data, uint32_t data_size, const rbsp_data_conf_t * const p_data_conf) {
    int32_t wk_errno;
    AIOCB * p_rbsp_aio;
    rbsp_sival_t * p_sival;
    rbsp_read_write_a_func_t p_func = (rbsp_read_write_a_func_t)p_ctl->p_async_func;

    if ((p_data_conf == NULL) || (p_data == NULL)) {
        wk_errno = ENOSPC_RBSP;
    } else if (p_ctl->p_sem_ctl->wait(osWaitForever) == -1) {
        wk_errno = EIO_RBSP;
    } else {
        p_rbsp_aio = (AIOCB *)p_ctl->p_aio_top + p_ctl->index;
        p_sival    = p_ctl->p_sival_top + p_ctl->index;

        p_sival->p_cb_func     = p_data_conf->p_notify_func;
        p_sival->p_cb_data     = p_data_conf->p_app_data;
        p_sival->p_sem         = p_ctl->p_sem_ctl;
        p_sival->p_aio         = p_rbsp_aio;

        p_rbsp_aio->aio_fildes = 0;
        p_rbsp_aio->aio_buf    = p_data;
        p_rbsp_aio->aio_nbytes = data_size;
        p_rbsp_aio->aio_offset = 0;
        p_rbsp_aio->aio_sigevent.sigev_notify = SIGEV_THREAD;
        p_rbsp_aio->aio_sigevent.sigev_value.sival_ptr = (void*)p_sival;
        p_rbsp_aio->aio_sigevent.sigev_notify_function = &callback_aio_trans;
        p_func(p_ctl->ch_handle, p_rbsp_aio, &wk_errno);

        if (wk_errno == ESUCCESS) {
            if ((p_ctl->index + 1) >= p_ctl->MaxNum) {
                p_ctl->index = 0;
            } else {
                p_ctl->index++;
            }
        } else {
            p_ctl->p_sem_ctl->release();
        }
    }

    return wk_errno;
}

/* static */ void R_BSP_Aio::callback_aio_trans(union sigval signo) {
    rbsp_sival_t * p_sival = (rbsp_sival_t *)signo.sival_ptr;
    AIOCB * p_aio_result = (AIOCB *)p_sival->p_aio;

    if ((p_sival->p_cb_func != NULL) && (p_aio_result != NULL)) {
        p_sival->p_cb_func((void *)p_aio_result->aio_buf, p_aio_result->aio_return, p_sival->p_cb_data);
    }
    p_sival->p_sem->release();
}
