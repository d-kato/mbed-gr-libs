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
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/

/*******************************************************************************
* File Name   : spdif_if.c
* $Rev: $
* $Date::                           $
* Description : SPDIF driver interface functions
******************************************************************************/

/*******************************************************************************
Includes <System Includes>, "Project Includes"
*******************************************************************************/
#include "cmsis_os.h"
#if(1) /* mbed */
#include "r_bsp_cmn.h"
#else  /* not mbed */
#include "ioif_public.h"
#endif /* end mbed */
#include "spdif.h"

/*******************************************************************************
Typedef definitions
*******************************************************************************/


/*******************************************************************************
Macro definitions
*******************************************************************************/


/*******************************************************************************
Exported global variables (to be accessed by other files)
*******************************************************************************/


/*******************************************************************************
Private global variables and functions
*******************************************************************************/

#if(1) /* mbed */
static spdif_drv_stat_t ch_drv_stat[SPDIF_NUM_CHANS] = {SPDIF_DRVSTS_UNINIT};

static void* R_SPDIF_InitOne(const int_t channel, const void* const config_data, int32_t* const p_errno);
static int_t R_SPDIF_UnInitOne(const int_t channel, const void* const driver_instance, int32_t* const p_errno);
#else  /* not mbed */
static void* R_SPDIF_Init(void* const config_data, int32_t* const p_errno);
static int_t R_SPDIF_UnInit(void* const driver_instance, int32_t* const p_errno);
#endif /* end mbed */
static int_t R_SPDIF_Open(void* const p_driver_instance, const char_t* const p_path_name, const int_t flags, const int_t mode, int32_t* const p_errno);
static int_t R_SPDIF_Close(void* const p_fd, int32_t* const p_errno);
static int_t R_SPDIF_Ioctl(void* const p_fd, const int_t request, void* const p_buf, int32_t* const p_errno);
static int_t R_SPDIF_WriteAsync(void* const p_fd, AIOCB* const p_aio, int32_t* const p_errno);
static int_t R_SPDIF_ReadAsync(void* const p_fd, AIOCB* const p_aio, int32_t* const p_errno);
static int_t R_SPDIF_Cancel(void* const p_fd, AIOCB* p_aio, int32_t* const p_errno);

static size_t SPDIF_StrnLen(const char_t p_str[], const size_t maxlen);
static int32_t SPDIF_Strncmp(const char_t p_str1[], const char_t p_str2[], const uint32_t maxlen);
static void   SPDIF_SetErrCode(const int_t error_code, int32_t* const p_errno);

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/

#if(1) /* mbed */
/******************************************************************************
* Function Name: R_SPDIF_MakeCbTbl_mbed
* @brief         Make the SPDIF driver function callback table
*
*                Description:<br>
*                
* @param         none
* @retval        pointer of SPDIF driver function callback table
******************************************************************************/
/* ->IPA M1.1.1 : This is liblary funciotn that is called from other module. */
RBSP_MBED_FNS* R_SPDIF_MakeCbTbl_mbed(void)
/* <-IPA M1.1.1 */
{
    static RBSP_MBED_FNS spdif_apitbl_mbed;

    spdif_apitbl_mbed.initialise_one   = &R_SPDIF_InitOne;
    spdif_apitbl_mbed.uninitialise_one = &R_SPDIF_UnInitOne;
    spdif_apitbl_mbed.open             = &R_SPDIF_Open;
    spdif_apitbl_mbed.close            = &R_SPDIF_Close;
    spdif_apitbl_mbed.ioctl            = &R_SPDIF_Ioctl;
    spdif_apitbl_mbed.read_a           = &R_SPDIF_ReadAsync;
    spdif_apitbl_mbed.write_a          = &R_SPDIF_WriteAsync;
    spdif_apitbl_mbed.cancel           = &R_SPDIF_Cancel;

    return &spdif_apitbl_mbed;
}
#else  /* not mbed */
/******************************************************************************
* Function Name: R_SPDIF_MakeCbTbl
* @brief         Make the SPDIF driver function callback table
*
*                Description:<br>
*
* @param         none
* @retval        pointer of SPDIF driver function callback table
******************************************************************************/
/* ->IPA M1.1.1 : This is liblary funciotn that is called from other module. */
IOIF_DRV_API* R_SPDIF_MakeCbTbl(void)
/* <-IPA M1.1.1 */
{
    static IOIF_DRV_API spdif_apitbl;

    /* ->MISRA 16.4, IPA M4.5.1 : This is IOIF library API type definitnon that can't be modified. */
    spdif_apitbl.family = IOIF_SERIAL_FAMILY;
    spdif_apitbl.fns.serial.initialise   = &R_SPDIF_Init;
    spdif_apitbl.fns.serial.uninitialise = &R_SPDIF_UnInit;
    spdif_apitbl.fns.serial.open         = &R_SPDIF_Open;
    spdif_apitbl.fns.serial.close        = &R_SPDIF_Close;
    spdif_apitbl.fns.serial.ioctl        = &R_SPDIF_Ioctl;
    spdif_apitbl.fns.serial.read_a       = &R_SPDIF_ReadAsync;
    spdif_apitbl.fns.serial.write_a      = &R_SPDIF_WriteAsync;
    spdif_apitbl.fns.serial.cancel       = &R_SPDIF_Cancel;
    /* <-MISRA 16.4, IPA M4.5.1 */

    return &spdif_apitbl;
}
#endif /* end mbed */


/******************************************************************************
Private functions
******************************************************************************/

#if(1) /* mbed */
/******************************************************************************
* Function Name: R_SPDIF_InitOne
* @brief         Initialise the SPDIF driver.
*
*                Description:<br>
*                
* @param[in]     channel     :channel number
* @param[in]     config_data :pointer of several parameters array per channels
* @param[in,out] p_errno     :pointer of error code
* @retval        not ERRROR  :driver instance.
* @retval        EERROR      :Failure.
******************************************************************************/
/* ->MISRA 16.7, IPA M1.11.1 : This is IOIF library API type definitnon that can't be modified. */
static void* R_SPDIF_InitOne(const int_t channel, const void* const config_data, int32_t* const p_errno)
/* <-MISRA 16.7, IPA M1.11.1 */
{
    int_t ercd;
    void* p_ret = (void*)EERROR;

    if (NULL == config_data)
    {
        ercd = EFAULT_RBSP;
    }
#if(1) /* mbed */
    else if ((uint32_t)channel >= SPDIF_NUM_CHANS)
#else
    else if (channel >= SPDIF_NUM_CHANS)
#endif
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
#if(1) /* mbed */
        spdif_info_drv_t* const p_info_drv = SPDIF_GetDrvInstanc();
        p_info_drv->drv_stat = SPDIF_DRVSTS_INIT;
#else
        g_spdif_info_drv.drv_stat = SPDIF_DRVSTS_INIT;
#endif /* end mbed */
        ch_drv_stat[channel] = SPDIF_DRVSTS_INIT;

        ercd = SPDIF_InitialiseOne(channel, (const spdif_channel_cfg_t*)config_data);

        if (ESUCCESS == ercd)
        {
#if(1) /* mbed */
            p_ret = (void*)p_info_drv;
#else
            p_ret = (void*)&g_spdif_info_drv;
#endif /* end mbed */
        }
        else
        {
            ch_drv_stat[channel] = SPDIF_DRVSTS_UNINIT;
        }
    }

    SPDIF_SetErrCode(ercd, p_errno);

    return p_ret;
}

/******************************************************************************
* Function Name: R_SPDIF_UnInitOne
* @brief         Uninitialise the SPDIF deiver.
*
*                Description:<br>
*                
* @param[in]     channel         :channel number
* @param[in,out] driver_instance :driver instance which was returned by<br>
                                  R_SPDIF_Init
* @param[in,out] p_errno         :pointer of error code
* @retval        ESUCCESS        :Success.
* @retval        EERROR          :Failure.
******************************************************************************/
static int_t R_SPDIF_UnInitOne(const int_t channel, const void* const driver_instance, int32_t* const p_errno)
{
    int_t   ercd;
    int_t   ret = ESUCCESS;

    if (NULL == driver_instance)
    {
        ercd = EFAULT_RBSP;
    }
#if(1) /* mbed */
    else if ((uint32_t)channel >= SPDIF_NUM_CHANS)
#else
    else if (channel >= SPDIF_NUM_CHANS)
#endif
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
        if (SPDIF_DRVSTS_INIT != ch_drv_stat[channel])
        {
            ercd = EFAULT_RBSP;
        }
        else
        {
            ercd = SPDIF_UnInitialiseOne(channel);
            ch_drv_stat[channel] = SPDIF_DRVSTS_UNINIT;
        }
    }

    if (ESUCCESS != ercd)
    {
        ret = EERROR;
    }
    SPDIF_SetErrCode(ercd, p_errno);

    return ret;
}

#else  /* not mbed */

/******************************************************************************
* Function Name: R_SPDIF_Init
* @brief         Initialise the SPDIF driver.
*
*                Description:<br>
*
* @param[in]     config_data :pointer of several parameters array per channels
* @param[in,out] p_errno     :pointer of error code
* @retval        not ERRROR  :driver instance.
* @retval        EERROR      :Failure.
******************************************************************************/
/* ->MISRA 16.7, IPA M1.11.1 : This is IOIF library API type definitnon that can't be modified. */
static void* R_SPDIF_Init(void* const config_data, int32_t* const p_errno)
/* <-MISRA 16.7, IPA M1.11.1 */
{
    int_t ercd;
    void* p_ret = (void*)EERROR;

    if (NULL == config_data)
    {
        ercd = EFAULT;
    }
    else if (SPDIF_DRVSTS_UNINIT != g_spdif_info_drv.drv_stat)
    {
        ercd = EBUSY;
    }
    else
    {
        g_spdif_info_drv.drv_stat = SPDIF_DRVSTS_INIT;

        ercd = SPDIF_Initialise((spdif_channel_cfg_t*)config_data);

        if (ESUCCESS == ercd)
        {
            p_ret = (void*)&g_spdif_info_drv;
        }
        else
        {
            g_spdif_info_drv.drv_stat = SPDIF_DRVSTS_UNINIT;
        }
    }

    SPDIF_SetErrCode(ercd, p_errno);

    return p_ret;
}

/******************************************************************************
* Function Name: R_SPDIF_UnInit
* @brief         Uninitialise the SPDIF deiver.
*
*                Description:<br>
*
* @param[in,out] driver_instance :driver instance which was returned by<br>
                                  R_SPDIF_Init
* @param[in,out] p_errno         :pointer of error code
* @retval        ESUCCESS        :Success.
* @retval        EERROR          :Failure.
******************************************************************************/
static int_t R_SPDIF_UnInit(void* const driver_instance, int32_t* const p_errno)
{
    int_t   ercd;
    int_t   ret = ESUCCESS;
    spdif_info_drv_t* const p_info_drv   = driver_instance;

    if (NULL == p_info_drv)
    {
        ercd = EFAULT;
    }
    else
    {
        if (SPDIF_DRVSTS_INIT != p_info_drv->drv_stat)
        {
            ercd = EFAULT;
        }
        else
        {
            ercd = SPDIF_UnInitialise();
            p_info_drv->drv_stat = SPDIF_DRVSTS_UNINIT;
        }
    }

    if (ESUCCESS != ercd)
    {
        ret = EERROR;
    }
    SPDIF_SetErrCode(ercd, p_errno);

    return ret;
}
#endif /* end mbed */

/******************************************************************************
* Function Name: R_SPDIF_Open
* @brief         Open an SPDIF channel
*
*                Description:<br>
*
* @param[in,out] p_drv_instance :driver instance which was returned by<br>
                                 R_SPDIF_Init
* @param[in]     p_path_name    :string of channel
* @param[in]     flags          :access mode whether the channel is opened<br>
                                 for a read or a write
* @param[in]     mode           :not used
* @param[in,out] p_errno        :pointer of error code
* @retval        not EERROR     :channel handle
* @retval        EERROR         :Failure.
******************************************************************************/
static int_t R_SPDIF_Open(void* const p_driver_instance, const char_t* const p_path_name, const int_t flags, const int_t mode, int32_t* const p_errno)
{
    void* p_channelHandle;
    size_t len;
    size_t req_path_len;
    spdif_info_drv_t* const p_info_drv = p_driver_instance;
    spdif_info_ch_t* p_info_ch = NULL;
    int_t ret;
    int_t ercd = ESUCCESS;
    osStatus os_ercd;
    int32_t os_ret;

    UNUSED_ARG(mode);

    if ((NULL == p_info_drv) || (NULL == p_path_name))
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
        req_path_len = strlen(p_path_name);
        if (0u == req_path_len)
        {
            ercd = ENOENT_RBSP;
        }

        if (ESUCCESS == ercd)
        {
            if (SPDIF_DRVSTS_INIT != p_info_drv->drv_stat)
            {
                ercd = EFAULT_RBSP;
            }
        }

        if (ESUCCESS == ercd)
        {
            len = SPDIF_StrnLen(SPDIF_CHSTR_0, SPDIF_MAX_PATH_LEN);

            if (req_path_len < len)
            {
                len = req_path_len;
            }

            if (0 == SPDIF_Strncmp(p_path_name, SPDIF_CHSTR_0, len))
            {
                /* found a match */
                p_info_ch = &p_info_drv->info_ch;
            }
        }
    }

    if (NULL == p_info_ch)
    {
        ercd = ENOENT_RBSP;
    }
    else
    {
        if (ESUCCESS == ercd)
        {
            if (false == p_info_ch->enabled)
            {
                ercd = ENOTSUP_RBSP;
            }
        }

        if (ESUCCESS == ercd)
        {
            if (SPDIF_CHSTS_INIT != p_info_ch->ch_stat)
            {
                ercd = EBADF_RBSP;
            }
        }

        if (ESUCCESS == ercd)
        {
            /* ->MISRA 10.6 : This macro is defined by CMSIS-RTOS that can't be modified. */
            os_ret = osSemaphoreAcquire(p_info_ch->sem_access, osWaitForever);
            /* <-MISRA 10.6 */

            if ((-1) == os_ret)
            {
                ercd = EFAULT_RBSP;
            }
            else
            {
                p_info_ch->openflag = flags;
                p_info_ch->p_aio_tx_curr = NULL;
                p_info_ch->p_aio_rx_curr = NULL;

                ercd = SPDIF_EnableChannel(p_info_ch);

                if (ESUCCESS == ercd)
                {
                    p_info_ch->ch_stat = SPDIF_CHSTS_OPEN;
                }
            }
            os_ercd = osSemaphoreRelease(p_info_ch->sem_access);
            if (osOK != os_ercd)
            {
                ercd = EFAULT_RBSP;
            }
        }
    }

    if (ESUCCESS != ercd)
    {
        ret = EERROR;   /* EERROR(-1) */
    }
    else
    {
        p_channelHandle = (void*)p_info_ch;
        ret = (int_t)p_channelHandle;
    }
    SPDIF_SetErrCode(ercd, p_errno);

    return ret;
}

/******************************************************************************
* Function Name: R_SPDIF_Close
* @brief         Close an SPDIF channel.
*
*                Description:<br>
*
* @param[in,out] p_fd       :channel handle which was returned by R_SPDIF_Open
* @param[in,out] p_errno    :pointer of error code
* @retval        ESUCCESS   :Success.
* @retval        EERROR     :Failure.
******************************************************************************/
static int_t R_SPDIF_Close(void* const p_fd, int32_t* const p_errno)
{
    spdif_info_ch_t* const p_info_ch = p_fd;
    int_t ret = ESUCCESS;
    int_t ercd;
    osStatus os_ercd;
    int32_t os_ret;

    if (NULL == p_info_ch)
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
        /* ->MISRA 10.6 : This macro is defined by CMSIS-RTOS that can't be modified. */
        /* Get semaphore to access the channel data */
        os_ret = osSemaphoreAcquire(p_info_ch->sem_access, osWaitForever);
        /* <-MISRA 10.6 */

        if ((-1) == os_ret)
        {
            ercd = EFAULT_RBSP;
        }
        else
        {
            if (SPDIF_CHSTS_OPEN != p_info_ch->ch_stat)
            {
                ercd = EFAULT_RBSP;
            }
            else
            {
                SPDIF_PostAsyncCancel(p_info_ch, NULL);

                ercd = SPDIF_DisableChannel(p_info_ch);

                if (ESUCCESS == ercd)
                {
                    p_info_ch->ch_stat = SPDIF_CHSTS_INIT;
                }
            }

            /* Relese semaphore */
            os_ercd = osSemaphoreRelease(p_info_ch->sem_access);

            if (osOK != os_ercd)
            {
                ercd = EFAULT_RBSP;
            }
        }
    }

    if (ESUCCESS != ercd)
    {
        ret = EERROR;   /* EERROR(-1) */
    }
    SPDIF_SetErrCode(ercd, p_errno);

    return ret;
}

/******************************************************************************
* Function Name: R_SPDIF_Ioctl
* @brief         IOCTL function of the SPDIF deiver
*
*                Description:<br>
*
* @param[in,out] p_fd       :channel handle which was returned by R_SPDIF_Open
* @param[in]     request    :IOCTL request code
* @param[in,out] p_buf      :Meaning depends upon request.
* @param[in,out] p_errno    :pointer of error code
* @retval        ESUCCESS   :Success.
* @retval        EERROR     :Failure.
******************************************************************************/
static int_t R_SPDIF_Ioctl(void* const p_fd, const int_t request, void* const p_buf, int32_t* const p_errno)
{
    spdif_info_ch_t* const p_info_ch = p_fd;
    int_t   ret = ESUCCESS;
    int_t ercd = ESUCCESS;
    osStatus os_ercd;
    int32_t os_ret;

    if (NULL == p_info_ch)
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
        if (SPDIF_CHSTS_OPEN != p_info_ch->ch_stat)
        {
            ercd = EFAULT_RBSP;
        }
        else
        {
            /* ->MISRA 10.6 : This macro is defined by CMSIS-RTOS that can't be modified. */
            os_ret = osSemaphoreAcquire(p_info_ch->sem_access, osWaitForever);
            /* <-MISRA 10.6 */

            if ((-1) == os_ret)
            {
                ercd = EFAULT_RBSP;
            }

            if (ESUCCESS == ercd)
            {
                switch (request)
                {
                    case SPDIF_CONFIG_CHANNEL:
                    {
                        if (NULL == p_buf)
                        {
                            ercd = EFAULT_RBSP;
                        }
                        else
                        {
                            spdif_channel_cfg_t* const ch_info = (spdif_channel_cfg_t*)p_buf;
                            ercd = SPDIF_IOCTL_ConfigChannel(p_info_ch, ch_info);
                        }
                        break;
                    }

                    case SPDIF_SET_CHANNEL_STATUS:
                    {
                        if (NULL == p_buf)
                        {
                            ercd = EFAULT_RBSP;
                        }
                        else
                        {
                            ercd = SPDIF_IOCTL_SetChannelStatus(p_info_ch, (spdif_channel_status_t*)p_buf);
                        }
                        break;
                    }

                    case SPDIF_GET_CHANNEL_STATUS:
                    {
                        if (NULL == p_buf)
                        {
                            ercd = EFAULT_RBSP;
                        }
                        else
                        {
                            ercd = SPDIF_IOCTL_GetChannelStatus(p_info_ch, (spdif_channel_status_t*)p_buf);
                        }
                        break;
                    }

                    case SPDIF_SET_TRANS_AUDIO_BIT:
                    {
                        if (NULL == p_buf)
                        {
                            ercd = EFAULT_RBSP;
                        }
                        else
                        {
                            ercd = SPDIF_IOCTL_SetTransAudioBit(p_info_ch, *(spdif_chcfg_audio_bit_t*)p_buf);
                        }
                        break;
                    }

                    case SPDIF_SET_RECV_AUDIO_BIT:
                    {
                        if (NULL == p_buf)
                        {
                            ercd = EFAULT_RBSP;
                        }
                        else
                        {
                            ercd = SPDIF_IOCTL_SetRecvAudioBit(p_info_ch, *(spdif_chcfg_audio_bit_t*)p_buf);
                        }
                        break;
                    }

                    default:
                    {
                        ercd = EINVAL_RBSP;
                        break;
                    }
                } /* switch */
            }
        }

        os_ercd = osSemaphoreRelease(p_info_ch->sem_access);
        if (osOK != os_ercd)
        {
            ercd = EFAULT_RBSP;
        }
    }

    if (ESUCCESS != ercd)
    {
        ret = EERROR;   /* EERROR(-1) */
    }
    SPDIF_SetErrCode(ercd, p_errno);

    return ret;
}

/******************************************************************************
* Function Name: R_SPDIF_WriteAsync
* @brief         Enqueue asynchronous write request
*
*                Description:<br>
*
* @param[in,out] p_fd       :channel handle which was returned by R_SPDIF_Open
* @param[in]     p_aio      :aio control block of write request
* @param[in,out] p_errno    :pointer of error code
* @retval        ESUCCESS   :Success.
* @retval        EERROR     :Failure.
******************************************************************************/
static int_t R_SPDIF_WriteAsync(void* const p_fd, AIOCB* const p_aio, int32_t* const p_errno)
{
    spdif_info_ch_t* const p_info_ch = p_fd;
    int_t   ret = ESUCCESS;
    int_t ercd = ESUCCESS;

    if ((NULL == p_info_ch) || (NULL == p_aio))
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
        if (((uint32_t)O_RDONLY_RBSP) == ((uint32_t)p_info_ch->openflag & O_ACCMODE_RBSP))
        {
            ercd = EACCES_RBSP;
        }
        else if ((0u == p_aio->aio_nbytes) && (p_aio->aio_buf == NULL))
        {
            ercd = EINVAL_RBSP;
        }
        else
        {
            if (0u == p_aio->aio_nbytes)
            {
                p_aio->aio_return = SPDIF_ASYNC_STRUCT_W;
            }
            else
            {
                p_aio->aio_return = SPDIF_ASYNC_W;
            }
            SPDIF_PostAsyncIo(p_info_ch, p_aio);
        }
    }

    if (ESUCCESS != ercd)
    {
        ret = EERROR;   /* EERROR(-1) */
    }
    SPDIF_SetErrCode(ercd, p_errno);

    return ret;
}

/******************************************************************************
* Function Name: R_SPDIF_ReadAsync
* @brief         Enqueue asynchronous read request
*
*                Description:<br>
*
* @param[in,out] p_fd       :channel handle which was returned by R_SPDIF_Open
* @param[in]     p_aio      :aio control block of read request
* @param[in,out] p_errno    :pointer of error code
* @retval        ESUCCESS   :Success.
* @retval        EERROR     :Failure.
******************************************************************************/
static int_t R_SPDIF_ReadAsync(void* const p_fd, AIOCB* const p_aio, int32_t* const p_errno)
{
    spdif_info_ch_t* const p_info_ch = p_fd;
    int_t   ret = ESUCCESS;
    int_t ercd = ESUCCESS;

    if ((NULL == p_info_ch) || (NULL == p_aio))
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
        if ((O_WRONLY_RBSP == ((uint32_t)p_info_ch->openflag & O_ACCMODE_RBSP)))
        {
            ercd = EACCES_RBSP;
        }
        else if ((0u == p_aio->aio_nbytes) && (p_aio->aio_buf == NULL))
        {
            ercd = EINVAL_RBSP;
        }
        else
        {
            if (0u == p_aio->aio_nbytes)
            {
                p_aio->aio_return = SPDIF_ASYNC_STRUCT_R;
            }
            else
            {
                p_aio->aio_return = SPDIF_ASYNC_R;
            }
            SPDIF_PostAsyncIo(p_info_ch, p_aio);
        }
    }

    if (ESUCCESS != ercd)
    {
        ret = EERROR;   /* EERROR(-1) */
    }
    SPDIF_SetErrCode(ercd, p_errno);

    return ret;
}

/******************************************************************************
* Function Name: R_SPDIF_Cancel
* @brief         Cancel read or write request(s)
*
*                Description:<br>
*
* @param[in,out] p_fd       :channel handle which was returned by R_SPDIF_Open
* @param[in]     p_aio      :aio control block to cancel or NULL to cancel all.
* @param[in,out] p_errno    :pointer of error code
* @retval        ESUCCESS   :Success.
* @retval        EERROR     :Failure.
******************************************************************************/
static int_t R_SPDIF_Cancel(void* const p_fd, AIOCB* const p_aio, int32_t* const p_errno)
{
    spdif_info_ch_t* const p_info_ch = p_fd;
    int_t ret = ESUCCESS;
    int_t ercd = ESUCCESS;
    osStatus os_ercd;
    int32_t os_ret;

    if (NULL == p_info_ch)
    {
        ercd = EFAULT_RBSP;
    }
    else
    {
        /* ->MISRA 10.6 : This macro is defined by CMSIS-RTOS that can't be modified. */
        /* Get semaphore to access the channel data */
        os_ret = osSemaphoreAcquire(p_info_ch->sem_access, osWaitForever);
        /* <-MISRA 10.6 */

        if ((-1) == os_ret)
        {
            ercd = EFAULT_RBSP;
        }
        else
        {
            if (SPDIF_CHSTS_OPEN != p_info_ch->ch_stat)
            {
                ercd = EFAULT_RBSP;
            }
            else
            {
                SPDIF_PostAsyncCancel(p_info_ch, p_aio);
            }

            os_ercd = osSemaphoreRelease(p_info_ch->sem_access);

            if (osOK != os_ercd)
            {
                ercd = EFAULT_RBSP;
            }
        }
    }

    if (ESUCCESS != ercd)
    {
        ret = EERROR;   /* EERROR(-1) */
    }
    SPDIF_SetErrCode(ercd, p_errno);

    return ret;
}

/******************************************************************************
* Function Name: SPDIF_StrnLen
* @brief         computes the length of the string
*
*                Description:<br>
*
* @param[in]     p_str      :pointer of string.
* @param[in]     maxlen     :maximum length of inspection
* @retval        < maxlen   :number of characters in the string
* @retval        maxlen     :string is longer than maxlen
******************************************************************************/
static size_t SPDIF_StrnLen(const char_t p_str[], const size_t maxlen)
{
    size_t len;

    if (NULL == p_str)
    {
        len = 0;
    }
    else
    {
        for (len = 0; len < maxlen; len++)
        {
            if ((int_t)p_str[len] == '\0')
            {
                break;
            }
        }
    }

    return len;
}

/******************************************************************************
* Function Name: SPDIF_Strncmp
* @brief         Compare two strings
*
*                Description:<br>
*
* @param[in]     p_str1     :pointer of string1
* @param[in]     p_str2     :pointer of string2
* @param[in]     maxlen     :maximum length of comparison
* @retval        zero       :strings are same.
* @retval        non zero   :strings are different.
******************************************************************************/
static int32_t SPDIF_Strncmp(const char_t p_str1[], const char_t p_str2[], const uint32_t maxlen)
{
    int32_t result = 0;
    uint32_t index;

    if ((NULL == p_str1) || (NULL == p_str2))
    {
        result = -1;
    }
    else
    {
        for (index = 0; index < maxlen; index++)
        {
            /* compare charctor */
            result = ((int_t)p_str1[index]) - ((int_t)p_str2[index]);
            if ((result != 0)
                || ((int_t)p_str1[index] == '\0')
                || ((int_t)p_str2[index] == '\0'))
            {
                /* "charactor mismatch" or "end of string" */
                break;
            }
        }
    }

    return result;
}

/******************************************************************************
* Function Name: SPDIF_SetErrCode
* @brief         Set error code to error code pointer.
*
*                Description:<br>
*                If error code pointer is NULL, do nothing.
* @param[in]     error_code :Error code.
* @param[in,out] p_errno    :Pointer of set error code.
* @retval        none
******************************************************************************/
static void SPDIF_SetErrCode(const int_t error_code, int32_t* const p_errno)
{
    if (NULL != p_errno)
    {
        *p_errno = error_code;
    }

    return;
}

