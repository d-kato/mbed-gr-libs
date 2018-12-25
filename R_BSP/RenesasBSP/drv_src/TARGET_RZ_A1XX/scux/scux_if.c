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
* http://www.renesas.com/disclaimer
* Copyright (C) 2013-2014 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/

/**************************************************************************//**
* @file         scux_if.c
* $Rev: $
* $Date::                           $
* @brief        SCUX Driver interface functions

*******************************************************************************/

/*******************************************************************************
Includes <System Includes>, "Project Includes"
*******************************************************************************/

#include "scux.h"
#include "bsp_util.h"
#include "mbed_critical.h"

/******************************************************************************
Macro definitions
******************************************************************************/

#define SCUX_PATH_CH0 "\\0"    /**< channel 0 pathname */
#define SCUX_PATH_CH1 "\\1"    /**< channel 1 pathname */
#define SCUX_PATH_CH2 "\\2"    /**< channel 2 pathname */
#define SCUX_PATH_CH3 "\\3"    /**< channel 3 pathname */
#define MAX_PATH_SEARCH_LEN (32U)

/******************************************************************************
Private global tables
******************************************************************************/

#if(1) /* mbed */
#else  /* not mbed */
static IOIF_DRV_API gb_ioif_scux_cb_table;
#endif /* end mbed */

static const char_t* const p_gb_device_name[SCUX_CH_NUM] =
{
    SCUX_PATH_CH0,
    SCUX_PATH_CH1,
    SCUX_PATH_CH2,
    SCUX_PATH_CH3
};

/******************************************************************************
Private global driver semaphore informationn
******************************************************************************/

/******************************************************************************
 Function prototypes
 *****************************************************************************/
#if(1) /* mbed */
static void *R_SCUX_InitOne(const int_t channel, const void * const p_config_data, int32_t * const p_errno); 
static int_t R_SCUX_UnInitOne(const int_t channel, const void* const p_driver_instance, int32_t * const p_errno);
#else
static void* R_SCUX_Init(void * p_config_data, int32_t * const p_errno);
static int_t R_SCUX_UnInit(void *p_driver_instance, int32_t * const p_errno);
#endif /* end mbed */
static int_t R_SCUX_Open(void * const p_driver_instance, const char_t * p_path_name, const int_t flags, int_t mode, int32_t * const p_errno);
static int_t R_SCUX_Close(void * const p_fd, int32_t * const p_errno);
static int_t R_SCUX_Ioctl(void * const p_fd, const int_t request, void * const p_buf, int32_t * const p_errno);
static int_t R_SCUX_WriteAsync(void * const p_fd, AIOCB * const p_aio, int32_t * const p_errno);
static int_t R_SCUX_ReadAsync(void * const p_fd, AIOCB * const p_aio, int32_t * const p_errno);
static int_t R_SCUX_Cancel(void * const p_fd, AIOCB * const p_aio, int32_t * const p_errno);
static void  SCUX_SetErrCode(const int_t error_code, int32_t * const p_errno);

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/

#if(1) /* mbed */
/**************************************************************************//**
* Function Name: R_SCUX_MakeCbTbl_mbed
* @brief         Returns the SCUX driver callback function table.
*
*                Description:<br>
*
* @param         none
* @retval        pointer of sample driver callback table
******************************************************************************/
/* ->IPA M1.1.1 If this function is the whole system, it will be called. */
RBSP_MBED_FNS* R_SCUX_MakeCbTbl_mbed(void)
/* <-IPA M1.1.1 */
{
    static RBSP_MBED_FNS scux_apitbl_mbed;

    scux_apitbl_mbed.initialise_one   = &R_SCUX_InitOne;
    scux_apitbl_mbed.uninitialise_one = &R_SCUX_UnInitOne;
    scux_apitbl_mbed.open             = &R_SCUX_Open;
    scux_apitbl_mbed.close            = &R_SCUX_Close;
    scux_apitbl_mbed.ioctl            = &R_SCUX_Ioctl;
    scux_apitbl_mbed.write_a          = &R_SCUX_WriteAsync;
    scux_apitbl_mbed.read_a           = &R_SCUX_ReadAsync;
    scux_apitbl_mbed.cancel           = &R_SCUX_Cancel;

    return &scux_apitbl_mbed;
}
/******************************************************************************
End of function R_SCUX_MakeCbTbl_mbed
******************************************************************************/
#else
/**************************************************************************//**
* Function Name: R_SCUX_MakeCbTbl
* @brief         Returns the SCUX driver callback function table.
*
*                Description:<br>
*
* @param         none
* @retval        pointer of sample driver callback table
******************************************************************************/
/* ->IPA M1.1.1 If this function is the whole system, it will be called. */
IOIF_DRV_API* R_SCUX_MakeCbTbl(void)
/* <-IPA M1.1.1 */
{
    IOIF_DRV_API* const p_api = &gb_ioif_scux_cb_table;

    /* ->MISRA 16.4, IPA M4.5.1 This description is based on the way to fill out OS defined. */
    p_api->family = IOIF_SERIAL_FAMILY;
    p_api->fns.serial.initialise   = &R_SCUX_Init;
    p_api->fns.serial.uninitialise = &R_SCUX_UnInit;
    p_api->fns.serial.open         = &R_SCUX_Open;
    p_api->fns.serial.close        = &R_SCUX_Close;
    p_api->fns.serial.ioctl        = &R_SCUX_Ioctl;
    p_api->fns.serial.write_a      = &R_SCUX_WriteAsync;
    p_api->fns.serial.read_a       = &R_SCUX_ReadAsync;
    p_api->fns.serial.cancel       = &R_SCUX_Cancel;
    /* <-MISRA 16.4, IPA M4.5.1 */

    return p_api;
}

/******************************************************************************
End of function R_SCUX_MakeCbTbl
******************************************************************************/
#endif /* end mbed */

/**************************************************************************//**
* Function Name: SCUX_SetErrCode
* @brief         Set error code to error code pointer.
*
*                Description:<br>
*                If error code pointer is NULL, nothing is done.
* @param[in]     error_code :Error code.
* @param[in,out] p_errno    :Pointer of set error code.
* @retval        none
******************************************************************************/
static void SCUX_SetErrCode(int_t error_code, int32_t * const p_errno)
{
    if (NULL != p_errno)
    {
        *p_errno = error_code;
    }

    return;
}

/******************************************************************************
End of function R_SCUX_SetErrCode
******************************************************************************/

/******************************************************************************
Private functions
******************************************************************************/

#if(1) /* mbed */
/**************************************************************************//**
* Function Name: R_SCUX_InitOne
* @brief         Init SCUX driver.
*
*                Description:<br>
*
* @param[in]     channel    :Initialize channel number.
* @param[in]     p_config_data :pointer of several parameters array per channels
* @param[in,out] p_errno    :pointer of error code
*                            error code -
*                            ENOMEM_RBSP : Making semaphore failed.
*                            EBUSY_RBSP  : SCUX driver has been initialized already.
*                            EFAULT_RBSP : p_config_data is NULL.
*                            EFAULT_RBSP : Internal error is occured.
* @retval        other than (-1)
*                  Operation successful.
*                EERROR -
*                  Error occured.
******************************************************************************/
static void *R_SCUX_InitOne(const int_t channel, const void * const p_config_data, int32_t * const p_errno) 
{
    int_t         ercd;
    void          *ret;
    scux_info_drv_t * const p_info_drv = SCUX_GetDrvInstance();
    scux_info_ch_t  * p_info_ch;

    if (NULL == p_info_drv)
    {
        ercd = EFAULT_RBSP;
    }
    else if ((SCUX_CH_0 > channel) || ( SCUX_CH_NUM <= channel))
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
        p_info_ch = SCUX_GetDrvChInfo(channel);

        if (NULL == p_info_ch)
        {
            ercd = EFAULT_RBSP;
        }
        else if (false != p_info_ch->enabled)
        {
            ercd = EBUSY_RBSP;
        }
        else
        {
            if (NULL == p_config_data)
            {
                /* set error return value */
                ercd = EFAULT_RBSP;
            }
            else
            {
                /* init SCUX driver */
                ercd = SCUX_InitializeOne(channel, (const scux_channel_cfg_t *)p_config_data);
            }
        }
    }

    if (ESUCCESS == ercd)
    {
        ret = (void *)p_info_drv;
    }
    else
    {
        ret = (void *)(EERROR);
    }

    SCUX_SetErrCode(ercd, p_errno);

    return ret;
}
/******************************************************************************
End of function R_SCUX_InitOne
******************************************************************************/

/**************************************************************************//**
* Function Name: R_SCUX_UnInitOne
* @brief         Uninit SCUX driver.
*
*                Description:<br>
*                
* @param[in]     channel    :Uninitialize channel number.
* @param[in]     p_driver_instance :which was returned by R_SCUX_Init
* @param[in,out] p_errno    :pointer of error code
*                            error code -
*                            EBADF_RBSP : Driver status isn't SCUX_DRV_INIT.
* @retval        ESUCCESS -
*                  Operation successful.
*                EERROR -
*                  Error occured.
******************************************************************************/
/* ->MISRA 16.7 'p_driver_instance' is based on the way to fill out OS defined. */
/* ->IPA M1.11.1 */
static int_t R_SCUX_UnInitOne(const int_t channel, const void* const p_driver_instance, int32_t * const p_errno)
/* <-IPA M1.11.1 */
/* <-MISRA 16.7 */
{
    int_t            retval = ESUCCESS;
    int_t            ercd = ESUCCESS;
    scux_info_drv_t  * const p_info_drv = SCUX_GetDrvInstance();
    scux_info_ch_t   * p_info_ch;

    UNUSED_ARG(p_driver_instance);
    
    if (NULL == p_info_drv)
    {
        ercd = EFAULT_RBSP;
    }
    else if ((SCUX_CH_0 > channel) || ( SCUX_CH_NUM <= channel))
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
        if (SCUX_DRV_INIT != p_info_drv->drv_stat)
        {
            ercd = EBADF_RBSP;
        }
        else
        {
            p_info_ch = SCUX_GetDrvChInfo(channel);

            if (NULL == p_info_ch)
            {
                ercd = EFAULT_RBSP;
            }
            else if (false == p_info_ch->enabled)
            {
                ercd = EBADF_RBSP;
            }
            else
            {
                SCUX_UnInitializeOne(channel);
            }
        }
    }

    if (ESUCCESS != ercd)
    {
        retval = EERROR;
    }
    SCUX_SetErrCode(ercd, p_errno);

    return retval;
}

/******************************************************************************
End of function R_SCUX_UnInitOne
******************************************************************************/
#else  /* not mbed */
/**************************************************************************//**
* Function Name: R_SCUX_Init
* @brief         Init SCUX driver.
*
*                Description:<br>
*
* @param[in]     p_config_data :pointer of several parameters array per channels
* @param[in,out] p_errno    :pointer of error code
*                            error code -
*                            ENOMEM : Making semaphore failed.
*                            EBUSY  : SCUX driver has been initialized already.
*                            EFAULT : p_config_data is NULL.
*                            EFAULT : Internal error is occured.
* @retval        other than (-1)
*                  Operation successful.
*                EERROR -
*                  Error occured.
******************************************************************************/
/* ->MISRA 16.7 'p_config_data' is based on the way to fill out OS defined. */
/* ->IPA M1.11.1 */
static void *R_SCUX_Init(void * p_config_data, int32_t * const p_errno)
/* <-IPA M1.11.1 */
/* <-MISRA 16.7 */
{
    int_t         ercd;
    void          *ret;
    scux_info_drv_t * const p_info_drv = SCUX_GetDrvInstance();

    if (NULL == p_info_drv)
    {
        ercd = EFAULT;
    }
    else
    {
        if (SCUX_DRV_UNINIT != p_info_drv->drv_stat)
        {
            ercd = EBUSY;
        }
        else
        {
            if (NULL == p_config_data)
            {
                /* set error return value */
                ercd = EFAULT;
            }
            else
            {
                /* init SCUX driver */
                ercd = SCUX_Initialize((scux_channel_cfg_t *)p_config_data);
            }
        }
    }

    if (ESUCCESS == ercd)
    {
        ret = (void *)p_info_drv;
    }
    else
    {
        ret = (void *)(EERROR);
    }

    SCUX_SetErrCode(ercd, p_errno);

    return ret;
}

/******************************************************************************
End of function R_SCUX_Init
******************************************************************************/

/**************************************************************************//**
* Function Name: R_SCUX_UnInit
* @brief         Uninit SCUX driver.
*
*                Description:<br>
*
* @param[in]     p_driver_instance :which was returned by R_SCUX_Init
* @param[in,out] p_errno    :pointer of error code
*                            error code -
*                            EBADF : Driver status isn't SCUX_DRV_INIT.
* @retval        ESUCCESS -
*                  Operation successful.
*                EERROR -
*                  Error occured.
******************************************************************************/
/* ->MISRA 16.7 'p_driver_instance' is based on the way to fill out OS defined. */
/* ->IPA M1.11.1 */
static int_t R_SCUX_UnInit(void* p_driver_instance, int32_t * const p_errno)
/* <-IPA M1.11.1 */
/* <-MISRA 16.7 */
{
    int_t            retval = ESUCCESS;
    int_t            ercd = ESUCCESS;
    scux_info_drv_t  * const p_info_drv = SCUX_GetDrvInstance();

    UNUSED_ARG(p_driver_instance);

    if (NULL == p_info_drv)
    {
        ercd = EFAULT;
    }
    else
    {
        if (SCUX_DRV_INIT != p_info_drv->drv_stat)
        {
            ercd = EBADF;
        }
        else
        {
            SCUX_UnInitialize();
        }
    }

    if (ESUCCESS != ercd)
    {
        retval = EERROR;
    }
    SCUX_SetErrCode(ercd, p_errno);

    return retval;
}
#endif /* end mbed */

/******************************************************************************
End of function R_SCUX_UnInit
******************************************************************************/

/**************************************************************************//**
* Function Name: R_SCUX_Open
* @brief         Open SCUX Channel.
*
*                Description:<br>
*
* @param[in]     p_driver_instance:which was returned by R_SCUX_Init()
* @param[in]     p_path_name   :device Name
* @param[in]     flags         :specifies the access mode whether the channel is
*                               opened for a read or a write
* @param[in]     mode          :specifies the permissions to use in case a new file
*                               is created (not used for serial family driver)
* @param[in,out] p_errno       :pointer of error code
*                               error code -
*                               ENOMEM_RBSP : Craeaton of IOIF queue is failed.
*                               ENOENT_RBSP : Pathname is incorrect length.
*                               ENOENT_RBSP : Channel information is NULL.
*                               EACCES_RBSP : Setting to flag is other than O_WRONLY_RBSP or O_RDWR_RBSP
*                               EMFILE_RBSP : Allocation of write DMA channel is failed.
*                               ENOTSUP_RBSP : Channel is not support.
*                               EFAULT_RBSP : Internal error is occured.
*
* @retval        Except ERROR -
*                  Operation successful.
*                EERROR -
*                  Error occured.
******************************************************************************/
/* ->IPA M1.11.1 This description is based on the way to fill out OS defined. */
static int_t R_SCUX_Open(void * const p_driver_instance, const char_t * p_path_name, const int_t flags, int_t mode , int32_t * const p_errno)
/* <-IPA M1.11.1 */
{
    int_t ercd = ESUCCESS;
    osStatus sem_ercd;
    int32_t  sem_wait_ercd;
    void* p_channel_handle;
    scux_info_drv_t * const p_info_drv = (scux_info_drv_t *)p_driver_instance;
    scux_info_ch_t *p_info_ch = NULL;
    uint32_t open_channel;
    size_t pathname_len;
    size_t compare_len;

    UNUSED_ARG(mode);

    /* check driver instance */
    if (NULL == p_driver_instance)
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
        if (NULL == p_info_drv)
        {
            ercd = EFAULT_RBSP;
        }
        else
        {
            if (SCUX_DRV_INIT != p_info_drv->drv_stat)
            {
                ercd = EFAULT_RBSP;
            }
        }
    }

    /* check path name */
    if (ESUCCESS == ercd)
    {
        if (NULL == p_info_drv)
        {
            ercd = EFAULT_RBSP;
        }
        else
        {
            if (NULL == p_path_name)
            {
                ercd = EFAULT_RBSP;
            }
            else
            {
                /* check length of path name */
                pathname_len = SCUX_StrNLen(p_path_name, MAX_PATH_SEARCH_LEN);
                if ( (0U == pathname_len) || (MAX_PATH_SEARCH_LEN == pathname_len) )
                {
                    ercd = ENOENT_RBSP;
                }
                else
                {
                    for (open_channel = 0; ((open_channel < SCUX_CH_NUM) && (NULL == p_info_ch)); open_channel++)
                    {
                        /* Avoid reading beyond the end of the shortest of the two strings */
                        compare_len = SCUX_StrNLen(p_gb_device_name[open_channel], MAX_PATH_SEARCH_LEN);
                        if (pathname_len < compare_len)
                        {
                            compare_len = pathname_len;
                        }

                        if (0 == strncmp(p_path_name, p_gb_device_name[open_channel], compare_len))
                        {
                            if (NULL != p_info_drv->info_ch)
                            {
                                p_info_ch = &p_info_drv->info_ch[open_channel]; /* found a match */
                            }
                        }
                    }

                    if (NULL == p_info_ch)
                    {
                        ercd = ENOENT_RBSP; /* Pathname not recognised */
                    }
                }
            }
        }
    }

    if (NULL != p_info_ch)
    {
        if (ESUCCESS == ercd)
        {
            if ((O_WRONLY_RBSP != flags) && (O_RDWR_RBSP != flags))
            {
                ercd = EACCES_RBSP;
            }

            /* ->MISRA 1.2 It is confirming in advance whether to be NULL or not. */
            /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
            sem_wait_ercd = osSemaphoreAcquire(p_info_ch->sem_ch_scux_access, osWaitForever);
            /* <-MISRA 10.6 */
            /* <-MISRA 1.2 */
            /* semaphore error check */
            if ((-1) == sem_wait_ercd)
            {
                /* set error return value */
                ercd = EFAULT_RBSP;
            }
            else
            {
                if (false == p_info_ch->enabled)
                {
                    ercd = ENOTSUP_RBSP;
                }

                if (ESUCCESS == ercd)
                {
                    if (SCUX_CH_INIT != p_info_ch->ch_stat)
                    {
                        ercd = EBUSY_RBSP;
                    }
                }

                if (ESUCCESS == ercd)
                {
                    ercd = SCUX_OpenChannel(p_info_ch->channel, flags);
                }
            }
            sem_ercd = osSemaphoreRelease(p_info_ch->sem_ch_scux_access);
            /* semaphore error check */
            if (osOK != sem_ercd)
            {
                /* set error return value */
                ercd = EFAULT_RBSP;
            }
        }
    }

    if (ESUCCESS == ercd)
    {
        p_channel_handle = (void*)p_info_ch;
    }
    else
    {
        p_channel_handle = (void*)EERROR;
    }
    SCUX_SetErrCode(ercd, p_errno);

    return (int_t)p_channel_handle;
}

/******************************************************************************
End of function R_SCUX_Open
******************************************************************************/

/**************************************************************************//**
* Function Name: R_SCUX_Close
* @brief         Close SCUX Channel.
*
*                Description:<br>
*
* @param[in]     p_fd:which was returned by R_SCUX_Init()
* @param[in,out] p_errno:pointer of error code
*                        error code -
*                           EFAULT_RBSP : Internal error is occured.
*
* @retval        ESUCCESS -
*                  Operation successful.
*                EERROR -
*                  Error occured.
******************************************************************************/
static int_t R_SCUX_Close(void * const p_fd, int32_t * const p_errno)
{
    int_t retval = ESUCCESS;
    int_t ercd = ESUCCESS;
    int32_t sem_wait_ercd;
    osStatus sem_ercd;
    scux_info_ch_t * const p_info_ch = p_fd;
    scux_info_drv_t * const p_info_drv = SCUX_GetDrvInstance();

    if ((NULL == p_info_ch) || (NULL == p_info_drv))
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
        if (SCUX_DRV_INIT != p_info_drv->drv_stat)
        {
            ercd = EFAULT_RBSP;
        }

        if (ESUCCESS == ercd)
        {
            /* ->MISRA 1.2 It is confirming in advance whether to be NULL or not. */
            /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
            sem_wait_ercd = osSemaphoreAcquire(p_info_ch->sem_ch_scux_access, osWaitForever);
            /* <-MISRA 10.6 */
            /* <-MISRA 1.2 */
            /* semaphore error check */
            if ((-1) == sem_wait_ercd)
            {
                /* set error return value */
                ercd = EFAULT_RBSP;
            }

            if (ESUCCESS == ercd)
            {
                if (false == p_info_ch->enabled)
                {
                    ercd = EFAULT_RBSP;
                }

                if (ESUCCESS == ercd)
                {
                    if ((SCUX_CH_UNINIT == p_info_ch->ch_stat) ||
                        (SCUX_CH_INIT == p_info_ch->ch_stat))
                    {
                        ercd = EFAULT_RBSP;
                    }
                }

                if (ESUCCESS == ercd)
                {
                    /* close channel */
                    ercd = SCUX_CloseChannel(p_info_ch->channel);
                }
            }
            sem_ercd = osSemaphoreRelease(p_info_ch->sem_ch_scux_access);
            /* semaphore error check */
            if (osOK != sem_ercd)
            {
                /* set error return value */
                    ercd = EFAULT_RBSP;
            }

        }
    }

    if (ESUCCESS != ercd)
    {
        retval = EERROR;
    }
    SCUX_SetErrCode(ercd, p_errno);

    return retval;
}

/******************************************************************************
End of function R_SCUX_Close
******************************************************************************/

/**************************************************************************//**
* Function Name: R_SCUX_Ioctl
* @brief         SCUX driver IOCTL function.
*
*                Description:<br>
*
* @param[in]     p_fd: pointer of SCUX handler
* @param[in]     request:IOCTL request code
* @param[in]     p_buf  :Data buffer for IOCTL request code.
* @param[in,out] p_errno:pointer of error code
*                        error code -
*                           EINVAL_RBSP : IOCTL request code is unexpected value.
*                           EFAULT_RBSP : Internal error is occured.
*                           other value : The value depending on IOCTL request code.
*                                         Refer to the function of scux_ioctl.c for those meanings.
*
* @retval        ESUCCESS -
*                  Operation successful.
*                EERROR -
*                  Error occured.
******************************************************************************/
static int_t R_SCUX_Ioctl(void * const p_fd, const int_t request, void * const p_buf, int32_t * const p_errno)
{
    int_t retval = ESUCCESS;
    int_t ercd = ESUCCESS;
    int32_t sem_wait_ercd;
    osStatus sem_ercd;
    scux_info_ch_t * const p_info_ch = p_fd;
    scux_info_drv_t * const p_info_drv = SCUX_GetDrvInstance();

    if ((NULL == p_info_ch) || (NULL == p_info_drv))
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
        if (SCUX_DRV_INIT != p_info_drv->drv_stat)
        {
            ercd = EFAULT_RBSP;
        }

        if (ESUCCESS == ercd)
        {
            /* ->MISRA 1.2 It is confirming in advance whether to be NULL or not. */
            /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
            if (0 == R_ExceptionalMode())
            {
                sem_wait_ercd = osSemaphoreAcquire(p_info_ch->sem_ch_scux_access, osWaitForever);
                /* <-MISRA 10.6 */
                /* <-MISRA 1.2 */
                /* semaphore error check */
                if ((-1) == sem_wait_ercd)
                {
                    /* set error return value */
                    ercd = EFAULT_RBSP;
                }
            }

            if (ESUCCESS == ercd)
            {
                if (false == p_info_ch->enabled)
                {
                    ercd = EFAULT_RBSP;
                }

                if (ESUCCESS == ercd)
                {
                    if ((SCUX_CH_UNINIT == p_info_ch->ch_stat) ||
                        (SCUX_CH_INIT == p_info_ch->ch_stat))
                    {
                        ercd = EFAULT_RBSP;
                    }
                }

            }

            if (0 == R_ExceptionalMode())
            {
                sem_ercd = osSemaphoreRelease(p_info_ch->sem_ch_scux_access);
                /* semaphore error check */
                if (osOK != sem_ercd)
                {
                    /* set error return value */
                    ercd = EFAULT_RBSP;
                }
            }

            if (ESUCCESS == ercd)
            {
                switch (request)
                {
                    case SCUX_IOCTL_SET_START:

                        /* ->MISRA 1.2 It is confirming in advance whether to be NULL or not. */
                        /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                        sem_wait_ercd = osSemaphoreAcquire(p_info_ch->sem_ch_scux_access, osWaitForever);
                        /* <-MISRA 10.6 */
                        /* <-MISRA 1.2 */
                        /* semaphore error check */
                        if ((-1) == sem_wait_ercd)
                        {
                            /* set error return value */
                            ercd = EFAULT_RBSP;
                        }
                        else
                        {
                            if (SCUX_CH_STOP != p_info_ch->ch_stat)
                            {
                                ercd = EBUSY_RBSP;
                            }
                            else
                            {
                                ercd = SCUX_IoctlTransStart(p_info_ch->channel);
                            }
                        }

                        sem_ercd = osSemaphoreRelease(p_info_ch->sem_ch_scux_access);
                        /* semaphore error check */
                        if (osOK != sem_ercd)
                        {
                            /* set error return value */
                            ercd = EFAULT_RBSP;
                        }

                    break;

                    case SCUX_IOCTL_SET_FLUSH_STOP:

                        /* ->MISRA 1.2 It is confirming in advance whether to be NULL or not. */
                        /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                        sem_wait_ercd = osSemaphoreAcquire(p_info_ch->sem_ch_scux_access, osWaitForever);
                        /* <-MISRA 10.6 */
                        /* <-MISRA 1.2 */
                        /* semaphore error check */
                        if ((-1) == sem_wait_ercd)
                        {
                            /* set error return value */
                            ercd = EFAULT_RBSP;
                        }
                        else
                        {
                            /* check p_Buf */
                            if (NULL == p_buf)
                            {
                                ercd = EFAULT_RBSP;
                            }
                            else
                            {
                                core_util_critical_section_enter();
                                if (SCUX_CH_STOP == p_info_ch->ch_stat)
                                {
                                    ercd = EBUSY_RBSP;
                                }

                                /* The mesure to MISRA 1.1 , SEC P1.1.1 */
                                if (ESUCCESS != ercd)
                                {
                                    /* enable all irq when ch_stat is SCUX_CH_STOP */
                                    core_util_critical_section_exit();
                                }

                                if (ESUCCESS == ercd)
                                {
                                    /* ->MISRA 11.1 Since it is necessary to register a callback function,
                                    the cast from a void pointer is performed intentionally. */
                                    ercd = SCUX_IoctlFlushStop(p_info_ch->channel, (void ( *)(int_t))p_buf);
                                    /* <-MISRA 11.1 */
                                }

                            }
                        }
                        sem_ercd = osSemaphoreRelease(p_info_ch->sem_ch_scux_access);
                        /* semaphore error check */
                        if (osOK != sem_ercd)
                        {
                            /* set error return value */
                            ercd = EFAULT_RBSP;
                        }

                    break;

                    case SCUX_IOCTL_SET_CLEAR_STOP:

                        /* ->MISRA 1.2 It is confirming in advance whether to be NULL or not. */
                        /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                        sem_wait_ercd = osSemaphoreAcquire(p_info_ch->sem_ch_scux_access, osWaitForever);
                        /* <-MISRA 10.6 */
                        /* <-MISRA 1.2 */
                        /* semaphore error check */

                        if ((-1) == sem_wait_ercd)
                        {
                            /* set error return value */
                            ercd = EFAULT_RBSP;
                        }
                        else
                        {
                            core_util_critical_section_enter();
                            if (SCUX_CH_STOP == p_info_ch->ch_stat)
                            {
                                ercd = EBUSY_RBSP;

                                core_util_critical_section_exit();
                            }
                            else
                            {
                                /* This exclusive access control ends in the SCUX_IoctlClearStop */
                                ercd = SCUX_IoctlClearStop(p_info_ch->channel);
                            }
                        }

                        sem_ercd = osSemaphoreRelease(p_info_ch->sem_ch_scux_access);
                        /* semaphore error check */
                        if (osOK != sem_ercd)
                        {
                            /* set error return value */
                            ercd = EFAULT_RBSP;
                        }

                    break;

                    case SCUX_IOCTL_SET_ROUTE:

                        /* ->MISRA 1.2 It is confirming in advance whether to be NULL or not. */
                        /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                        sem_wait_ercd = osSemaphoreAcquire(p_info_ch->sem_ch_scux_access, osWaitForever);
                        /* <-MISRA 10.6 */
                        /* <-MISRA 1.2 */
                        /* semaphore error check */
                        if ((-1) == sem_wait_ercd)
                        {
                            /* set error return value */
                            ercd = EFAULT_RBSP;
                        }
                        else
                        {
                            if (SCUX_CH_STOP != p_info_ch->ch_stat)
                            {
                                ercd = EBUSY_RBSP;
                            }
                            else
                            {
                                if (NULL == p_buf)
                                {
                                    ercd = EFAULT_RBSP;
                                }
                                else
                                {
                                    SCUX_IoctlSetRoute(p_info_ch->channel, *(scux_route_t *)p_buf);
                                }
                            }
                        }

                        sem_ercd = osSemaphoreRelease(p_info_ch->sem_ch_scux_access);
                        /* semaphore error check */
                        if (osOK != sem_ercd)
                        {
                            /* set error return value */
                            ercd = EFAULT_RBSP;
                        }

                    break;

                    case SCUX_IOCTL_SET_PIN_CLK:

                        /* ->MISRA 1.2 It is confirming in advance whether to be NULL or not. */
                        /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                        sem_wait_ercd = osSemaphoreAcquire(p_info_ch->sem_ch_scux_access, osWaitForever);
                        /* <-MISRA 10.6 */
                        /* <-MISRA 1.2 */
                        /* semaphore error check */
                        if ((-1) == sem_wait_ercd)
                        {
                            /* set error return value */
                            ercd = EFAULT_RBSP;
                        }
                        else
                        {
                            if (SCUX_CH_STOP != p_info_ch->ch_stat)
                            {
                                ercd = EBUSY_RBSP;
                            }
                            else
                            {
                                if (NULL == p_buf)
                                {
                                    ercd = EFAULT_RBSP;
                                }
                                else
                                {
                                    ercd = SCUX_IoctlSetPinClk(p_info_ch->channel, (scux_ssif_pin_clk_t *)p_buf);
                                }
                            }
                        }

                        sem_ercd = osSemaphoreRelease(p_info_ch->sem_ch_scux_access);
                        /* semaphore error check */
                        if (osOK != sem_ercd)
                        {
                            /* set error return value */
                            ercd = EFAULT_RBSP;
                        }

                    break;

                    case SCUX_IOCTL_SET_PIN_MODE:

                        /* ->MISRA 1.2 It is confirming in advance whether to be NULL or not. */
                        /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                        sem_wait_ercd = osSemaphoreAcquire(p_info_ch->sem_ch_scux_access, osWaitForever);
                        /* <-MISRA 10.6 */
                        /* <-MISRA 1.2 */
                        /* semaphore error check */
                        if ((-1) == sem_wait_ercd)
                        {
                            /* set error return value */
                            ercd = EFAULT_RBSP;
                        }
                        else
                        {
                            if (SCUX_CH_STOP != p_info_ch->ch_stat)
                            {
                                ercd = EBUSY_RBSP;
                            }
                            else
                            {
                                if (NULL == p_buf)
                                {
                                    ercd = EFAULT_RBSP;
                                }
                                else
                                {
                                    ercd = SCUX_IoctlSetPinMode(p_info_ch->channel, (scux_ssif_pin_mode_t *)p_buf);
                                }
                            }
                        }

                        sem_ercd = osSemaphoreRelease(p_info_ch->sem_ch_scux_access);
                        /* semaphore error check */
                        if (osOK != sem_ercd)
                        {
                            /* set error return value */
                            ercd = EFAULT_RBSP;
                        }

                    break;

                    case SCUX_IOCTL_SET_SRC_CFG:

                        /* ->MISRA 1.2 It is confirming in advance whether to be NULL or not. */
                        /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                        sem_wait_ercd = osSemaphoreAcquire(p_info_ch->sem_ch_scux_access, osWaitForever);
                        /* <-MISRA 10.6 */
                        /* <-MISRA 1.2 */
                        /* semaphore error check */
                        if ((-1) == sem_wait_ercd)
                        {
                            /* set error return value */
                            ercd = EFAULT_RBSP;
                        }
                        else
                        {
                            if (SCUX_CH_STOP != p_info_ch->ch_stat)
                            {
                                ercd = EBUSY_RBSP;
                            }
                            else
                            {
                                if (NULL == p_buf)
                                {
                                    ercd = EFAULT_RBSP;
                                }
                                else
                                {
                                    SCUX_IoctlSetSrcCfg(p_info_ch->channel, (scux_src_cfg_t *)p_buf);
                                }
                            }
                        }

                        sem_ercd = osSemaphoreRelease(p_info_ch->sem_ch_scux_access);
                        /* semaphore error check */
                        if (osOK != sem_ercd)
                        {
                            /* set error return value */
                            ercd = EFAULT_RBSP;
                        }

                    break;

                    case SCUX_IOCTL_SET_DVU_CFG:

                        /* ->MISRA 1.2 It is confirming in advance whether to be NULL or not. */
                        /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                        sem_wait_ercd = osSemaphoreAcquire(p_info_ch->sem_ch_scux_access, osWaitForever);
                        /* <-MISRA 10.6 */
                        /* <-MISRA 1.2 */
                        /* semaphore error check */
                        if ((-1) == sem_wait_ercd)
                        {
                            /* set error return value */
                            ercd = EFAULT_RBSP;
                        }
                        else
                        {
                            if (SCUX_CH_STOP != p_info_ch->ch_stat)
                            {
                                ercd = EBUSY_RBSP;
                            }
                            else
                            {
                                if (NULL == p_buf)
                                {
                                    ercd = EFAULT_RBSP;
                                }
                                else
                                {
                                    SCUX_IoctlSetDvuCfg(p_info_ch->channel, (scux_dvu_cfg_t *)p_buf);
                                }
                            }
                        }

                        sem_ercd = osSemaphoreRelease(p_info_ch->sem_ch_scux_access);
                        /* semaphore error check */
                        if (osOK != sem_ercd)
                        {
                            /* set error return value */
                            ercd = EFAULT_RBSP;
                        }

                    break;

                    case SCUX_IOCTL_SET_DVU_DIGI_VOL:

                        if (NULL == p_buf)
                        {
                            ercd = EFAULT_RBSP;
                        }
                        else
                        {
                            ercd = SCUX_IoctlSetDvuDigiVol(p_info_ch->channel, (scux_dvu_digi_vol_t *)p_buf);
                        }

                    break;

                    case SCUX_IOCTL_SET_DVU_RAMP_VOL:

                        if (NULL == p_buf)
                        {
                            ercd = EFAULT_RBSP;
                        }
                        else
                        {
                            ercd = SCUX_IoctlSetDvuRampVol(p_info_ch->channel, (scux_dvu_ramp_vol_t *)p_buf);
                        }

                    break;

                    case SCUX_IOCTL_SET_ZEROCROSS_MUTE:

                        if (NULL == p_buf)
                        {
                            ercd = EFAULT_RBSP;
                        }
                        else
                        {
                            ercd = SCUX_IoctlSetDvuZerocrossMute(p_info_ch->channel, (scux_zc_mute_t *)p_buf);
                        }

                    break;

                    case SCUX_IOCTL_SET_STOP_MUTE:

                        if (NULL == p_buf)
                        {
                            ercd = EFAULT_RBSP;
                        }
                        else
                        {
                            ercd = SCUX_IoctlSetStopMute(p_info_ch->channel, *(uint32_t *)p_buf);
                        }

                    break;

                    case SCUX_IOCTL_SET_MIX_CFG:

                        if (NULL == p_buf)
                        {
                            ercd = EFAULT_RBSP;
                        }
                        else
                        {
                            ercd = SCUX_IoctlSetMixCfg((scux_mix_cfg_t *)p_buf);
                        }

                    break;

                    case SCUX_IOCTL_SET_MIX_VOL:

                        if (NULL == p_buf)
                        {
                            ercd = EFAULT_RBSP;
                        }
                        else
                        {
                            ercd = SCUX_IoctlSetMixVol(p_info_ch->channel, *(uint32_t *)p_buf);
                        }

                    break;

                    case SCUX_IOCTL_SET_SSIF_CFG:

                        /* ->MISRA 1.2 It is confirming in advance whether to be NULL or not. */
                        /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
                        sem_wait_ercd = osSemaphoreAcquire(p_info_ch->sem_ch_scux_access, osWaitForever);
                        /* <-MISRA 10.6 */
                        /* <-MISRA 1.2 */
                        /* semaphore error check */
                        if ((-1) == sem_wait_ercd)
                        {
                            /* set error return value */
                            ercd = EFAULT_RBSP;
                        }
                        else
                        {
                            if (SCUX_CH_STOP != p_info_ch->ch_stat)
                            {
                                ercd = EBUSY_RBSP;
                            }
                            else
                            {
                                if (NULL == p_buf)
                                {
                                    ercd = EFAULT_RBSP;
                                }
                                else
                                {
                                    ercd = SCUX_IoctlSetSsifCfg((scux_ssif_cfg_t *)p_buf);
                                }
                            }
                        }

                        sem_ercd = osSemaphoreRelease(p_info_ch->sem_ch_scux_access);
                        /* semaphore error check */
                        if (osOK != sem_ercd)
                        {
                            /* set error return value */
                            ercd = EFAULT_RBSP;
                        }

                    break;

                    case SCUX_IOCTL_GET_WRITE_STAT:

                        if (NULL == p_buf)
                        {
                            ercd = EFAULT_RBSP;
                        }
                        else
                        {
                            SCUX_IoctlGetWriteStat(p_info_ch->channel, (uint32_t *)p_buf);
                        }

                    break;

                    case SCUX_IOCTL_GET_READ_STAT:

                        if (NULL == p_buf)
                        {
                            ercd = EFAULT_RBSP;
                        }
                        else
                        {
                            SCUX_IoctlGetReadStat(p_info_ch->channel, (uint32_t *)p_buf);
                        }

                    break;

                    case SCUX_IOCTL_GET_DVU_STAT:

                        if (NULL == p_buf)
                        {
                            ercd = EFAULT_RBSP;
                        }
                        else
                        {
                            SCUX_IoctlGetDvuStat(p_info_ch->channel, (uint32_t *)p_buf);
                        }

                    break;

                    case SCUX_IOCTL_GET_MUTE_STAT:

                        if (NULL == p_buf)
                        {
                            ercd = EFAULT_RBSP;
                        }
                        else
                        {
                            SCUX_IoctlGetMuteStat(p_info_ch->channel, (uint32_t *)p_buf);
                        }

                    break;

                    case SCUX_IOCTL_GET_MIX_STAT:

                        if (NULL == p_buf)
                        {
                            ercd = EFAULT_RBSP;
                        }
                        else
                        {
                            SCUX_IoctlGetMixStat(p_info_ch->channel, (uint32_t *)p_buf);
                        }

                    break;

                    default:
                        ercd = EINVAL_RBSP;
                    break;
                }
            }
        }
    }

    if (ESUCCESS != ercd)
    {
        retval = EERROR;
    }
    SCUX_SetErrCode(ercd, p_errno);

    return retval;

}

/******************************************************************************
End of function R_SCUX_Ioctl
******************************************************************************/

/**************************************************************************//**
* Function Name: R_SCUX_WriteAsync
* @brief         SCUX driver asynchronous write.
*
*                Description:<br>
*
* @param[in]     p_fd   :channel handle which was returned by R_SCUX_Open
* @param[in]     p_aio  :aio control block.
* @param[in,out] p_errno:pointer of error code
*                               error code -
*                               EBADF_RBSP : Channel status isn't SCUX_CH_STOP or SCUX_CH_STOP_WAIT.
*                               EINVAL_RBSP : p_fd is NULL.
*                               EINVAL_RBSP : write size is 0.
*                               EFAULT_RBSP : Internal error is occured.
*
* @retval        ESUCCESS -
*                  Operation successful.
*                EERROR -
*                  Error occured.
******************************************************************************/
static int_t R_SCUX_WriteAsync(void * const p_fd, AIOCB * const p_aio, int32_t * const p_errno)
{
    int_t retval = ESUCCESS;
    int_t ercd = ESUCCESS;
    int_t sem_ercd;
    int32_t  sem_wait_ercd;
    scux_info_ch_t * const p_info_ch = p_fd;
    scux_info_drv_t * const p_info_drv = SCUX_GetDrvInstance();

    if ((NULL == p_info_ch) || (NULL == p_aio) || (NULL == p_info_drv))
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
        if (0U == p_aio->aio_nbytes)
        {
            ercd = EINVAL_RBSP;
        }

        if (ESUCCESS == ercd)
        {
            if (SCUX_DRV_INIT != p_info_drv->drv_stat)
            {
                ercd = EFAULT_RBSP;
            }
        }

        if (ESUCCESS == ercd)
        {
            /* ->MISRA 1.2 It is confirming in advance whether to be NULL or not. */
            /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
            sem_wait_ercd = osSemaphoreAcquire(p_info_ch->sem_ch_scux_access, osWaitForever);
            /* <-MISRA 10.6 */
            /* <-MISRA 1.2 */
            /* semaphore error check */
            if ((-1) == sem_wait_ercd)
            {
                /* set error return value */
                ercd = EFAULT_RBSP;
            }
            else
            {
                if (false == p_info_ch->enabled)
                {
                    ercd = EFAULT_RBSP;
                }
            }

            if (ESUCCESS == ercd)
            {
                core_util_critical_section_enter();

                if (ESUCCESS != p_info_ch->err_stat_backup)
                {
                    /* set error code */
                    ercd = p_info_ch->err_stat_backup;
                }
                else
                {
                    switch (p_info_ch->ch_stat)
                    {
                        case SCUX_CH_UNINIT :
                            /* fall through */
                        case SCUX_CH_INIT :
                            ercd = EFAULT_RBSP;
                        break;

                        case SCUX_CH_STOP :
                            ercd = EBADF_RBSP;
                        break;

                        case SCUX_CH_TRANS_IDLE :
                            /* fall through */
                        case SCUX_CH_TRANS_RD :
                            if (SCUX_ROUTE_MEM_TO_MEM != (p_info_ch->route_set & SCUX_GET_ROUTE_MASK))
                            {
                                ercd = SCUX_DirectWriteStart(p_info_ch, p_aio);
                            }
                            else
                            {
                                ercd = SCUX_CopyWriteStart(p_info_ch, p_aio);
                            }
                        break;

                        case SCUX_CH_TRANS_WR :
                            /* fall through */
                        case SCUX_CH_TRANS_RDWR :
                            ahf_addtail(&p_info_ch->tx_que, p_aio);
                        break;

                        case SCUX_CH_STOP_WAIT :
                            /* fall through */
                        case SCUX_CH_STOP_WAIT_IDLE :
                            ercd = EBADF_RBSP;
                        break;

                        default :
                            ercd = EFAULT_RBSP;
                        break;
                    }
                }

                core_util_critical_section_exit();
            }
            sem_ercd = osSemaphoreRelease(p_info_ch->sem_ch_scux_access);
            /* semaphore error check */
            if (osOK != sem_ercd)
            {
                /* set error return value */
                ercd = EFAULT_RBSP;
            }
        }
    }

    if (ESUCCESS != ercd)
    {
        retval = EERROR;
    }
    SCUX_SetErrCode(ercd, p_errno);

    return retval;

}

/******************************************************************************
End of function R_SCUX_WriteAsync
******************************************************************************/
/**************************************************************************//**
* Function Name: R_SCUX_ReadAsync
* @brief         SCUX driver asynchronous read.
*
*                Description:<br>
*
* @param[in]     p_fd   :channel handle which was returned by R_SCUX_open
* @param[in]     p_aio  :aio control block.
* @param[in,out] p_errno:pointer of error code
*                               error code -
*                               EBADF_RBSP : Channel status isn't SCUX_CH_STOP or SCUX_CH_STOP_WAIT.
*                               EINVAL_RBSP : read size is 0.
*                               EACCES_RBSP : Request for write only mode channel.
*                               EACCES_RBSP : Route setting is unexpected.
*                               EFAULT_RBSP : Internal error is occured.
*
* @retval        ESUCCESS -
*                  Operation successful.
*                EERROR -
*                  Error occured.
******************************************************************************/
static int_t R_SCUX_ReadAsync(void * const p_fd, AIOCB * const p_aio, int32_t * const p_errno)
{
    int_t retval = ESUCCESS;
    int_t ercd = ESUCCESS;
    osStatus sem_ercd;
    int32_t sem_wait_ercd;
    scux_info_ch_t * const p_info_ch = p_fd;
    scux_info_drv_t * const p_info_drv = SCUX_GetDrvInstance();

    if ((NULL == p_info_ch) || (NULL == p_info_drv) || (NULL == p_aio))
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
        if (0U == p_aio->aio_nbytes)
        {
            ercd = EINVAL_RBSP;
        }

        if (ESUCCESS == ercd)
        {
            if (SCUX_DRV_INIT != p_info_drv->drv_stat)
            {
                ercd = EFAULT_RBSP;
            }
        }

        if (ESUCCESS == ercd)
        {
            /* ->MISRA 1.2 It is confirming in advance whether to be NULL or not. */
            /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
            sem_wait_ercd = osSemaphoreAcquire(p_info_ch->sem_ch_scux_access, osWaitForever);
            /* <-MISRA 10.6 */
            /* <-MISRA 1.2 */
            /* semaphore error check */
            if ((-1) == sem_wait_ercd)
            {
                /* set error return value */
                ercd = EFAULT_RBSP;
            }
            else
            {
                if (false == p_info_ch->enabled)
                {
                    ercd = EFAULT_RBSP;
                }
            }

            if (ESUCCESS == ercd)
            {
                if (O_WRONLY_RBSP == (p_info_ch->open_flags & O_ACCMODE_RBSP))
                {
                    ercd = EACCES_RBSP;
                }
            }

            if (ESUCCESS == ercd)
            {

                if (SCUX_ROUTE_MEM_TO_MEM != (p_info_ch->route_set & SCUX_GET_ROUTE_MASK))
                {
                    ercd = EACCES_RBSP;
                }

                if (ESUCCESS == ercd)
                {
                    core_util_critical_section_enter();

                    if (ESUCCESS != p_info_ch->err_stat_backup)
                    {
                        /* set error code */
                        ercd = p_info_ch->err_stat_backup;
                    }
                    else
                    {
                        switch (p_info_ch->ch_stat)
                        {
                            case SCUX_CH_UNINIT :
                                /* fall through */
                            case SCUX_CH_INIT :
                                ercd = EFAULT_RBSP;
                            break;

                            case SCUX_CH_STOP :
                                ercd = EBADF_RBSP;
                            break;

                            case SCUX_CH_TRANS_IDLE :
                                ercd = SCUX_CopyReadStart(p_info_ch, p_aio);
                            break;

                            case SCUX_CH_TRANS_RD :
                                ahf_addtail(&p_info_ch->rx_que, p_aio);
                            break;

                            case SCUX_CH_TRANS_WR :
                                ercd = SCUX_CopyReadStart(p_info_ch, p_aio);
                            break;

                            case SCUX_CH_TRANS_RDWR :
                                /* fall through */
                            case SCUX_CH_STOP_WAIT :
                                ahf_addtail(&p_info_ch->rx_que, p_aio);
                            break;

                            case SCUX_CH_STOP_WAIT_IDLE :
                                ercd = SCUX_CopyReadStart(p_info_ch, p_aio);
                            break;

                            default :
                                ercd = EFAULT_RBSP;
                            break;
                        }
                    }

                    core_util_critical_section_exit();
                }
            }
            sem_ercd = osSemaphoreRelease(p_info_ch->sem_ch_scux_access);
            /* semaphore error check */
            if (osOK != sem_ercd)
            {
                /* set error return value */
                ercd = EFAULT_RBSP;
            }
        }
    }

    if (ESUCCESS != ercd)
    {
        retval = EERROR;
    }
    SCUX_SetErrCode(ercd, p_errno);

    return retval;

}

/******************************************************************************
End of function R_SCUX_ReadAsync
******************************************************************************/

/**************************************************************************//**
* Function Name: R_SCUX_Cancel
* @brief         Request cancel.
*
*                Description:<br>
*
* @param[in]     p_fd   :channel handle which was returned by R_SCUX_open
* @param[in]     p_aio  :aio control block.
* @param[in,out] p_errno:pointer of error code
*                               error code -
*                               EFAULT_RBSP : Internal error is occured.
*
* @retval        ESUCCESS -
*                  Operation successful.
*                EERROR -
*                  Error occured.
******************************************************************************/
static int_t R_SCUX_Cancel(void * const p_fd, AIOCB * const p_aio, int32_t * const p_errno)
{
    int_t retval = ESUCCESS;
    int_t ercd = ESUCCESS;
    osStatus sem_ercd;
    int32_t sem_wait_ercd;
    scux_info_ch_t * const p_info_ch = p_fd;
    scux_info_drv_t * const p_info_drv = SCUX_GetDrvInstance();

    if ((NULL == p_info_ch) || (NULL == p_info_drv))
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
        if (SCUX_DRV_INIT != p_info_drv->drv_stat)
        {
            ercd = EFAULT_RBSP;
        }

        if (ESUCCESS == ercd)
        {
            /* ->MISRA 1.2 It is confirming in advance whether to be NULL or not. */
            /* ->MISRA 10.6, osWaitForever is defined by the header got from related section*/
            sem_wait_ercd = osSemaphoreAcquire(p_info_ch->sem_ch_scux_access, osWaitForever);
            /* <-MISRA 10.6 */
            /* <-MISRA 1.2 */
            /* semaphore error check */
            if ((-1) == sem_wait_ercd)
            {
                /* set error return value */
                ercd = EFAULT_RBSP;
            }

            if (ESUCCESS == ercd)
            {
                if (false == p_info_ch->enabled)
                {
                    ercd = EFAULT_RBSP;
                }

                if (ESUCCESS == ercd)
                {
                    if ((SCUX_CH_INIT == p_info_ch->ch_stat) ||
                        (SCUX_CH_UNINIT == p_info_ch->ch_stat))
                    {
                        /* It should not becomde status */
                        ercd = EFAULT_RBSP;
                    }
                    else if ((SCUX_CH_STOP_WAIT == p_info_ch->ch_stat) ||
                              (SCUX_CH_STOP_WAIT_IDLE == p_info_ch->ch_stat) ||
                              (SCUX_CH_STOP == p_info_ch->ch_stat))
                    {
                        /* busy error on flush waiting */
                        ercd = EBADF_RBSP;
                    }
                    else
                    {
                        if (NULL == p_aio)
                        {
                            if (SCUX_ROUTE_MEM_TO_MEM != (p_info_ch->route_set & SCUX_GET_ROUTE_MASK))
                            {
                                ercd = SCUX_DirectCancelAll(p_info_ch);
                            }
                            else
                            {
                                ercd = SCUX_CopyCancelAll(p_info_ch);
                            }
                        }
                        else
                        {
                            if (SCUX_ROUTE_MEM_TO_MEM != (p_info_ch->route_set & SCUX_GET_ROUTE_MASK))
                            {
                                ercd = SCUX_DirectCancelSpecific(p_info_ch, p_aio);
                            }
                            else
                            {
                                ercd = SCUX_CopyCancelSpecific(p_info_ch, p_aio);
                            }
                        }
                    }
                }
            }
            sem_ercd = osSemaphoreRelease(p_info_ch->sem_ch_scux_access);
            /* semaphore error check */
            if (osOK != sem_ercd)
            {
                /* set error return value */
                ercd = EFAULT_RBSP;
            }
        }
    }

    if (ESUCCESS != ercd)
    {
        retval = EERROR;
    }
    SCUX_SetErrCode(ercd, p_errno);

    return retval;
}

/******************************************************************************
End of function R_SCUX_Cancel
******************************************************************************/

