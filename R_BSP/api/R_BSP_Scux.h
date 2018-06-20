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
* @file          R_BSP_Scux.h
* @brief         SCUX API
******************************************************************************/

#ifndef R_BSP_SCUX_H
#define R_BSP_SCUX_H

#if defined(TARGET_RZ_A1XX)
#define R_BSP_SCUX_ENABLE    1

#include "R_BSP_SerialFamily.h"
#include "R_BSP_ScuxDef.h"

#define SAMPLING_RATE_8000HZ  (8000U)  /* Selects a sampling rate of 8 kHz. */
#define SAMPLING_RATE_11025HZ (11025U) /* Selects a sampling rate of 11.025 kHz. */
#define SAMPLING_RATE_12000HZ (12000U) /* Selects a sampling rate of 12 kHz. */
#define SAMPLING_RATE_16000HZ (16000U) /* Selects a sampling rate of 16 kHz. */
#define SAMPLING_RATE_22050HZ (22050U) /* Selects a sampling rate of 22.05 kHz. */
#define SAMPLING_RATE_24000HZ (24000U) /* Selects a sampling rate of 24 kHz. */
#define SAMPLING_RATE_32000HZ (32000U) /* Selects a sampling rate of 32 kHz. */
#define SAMPLING_RATE_44100HZ (44100U) /* Selects a sampling rate of 44.1 kHz. */
#define SAMPLING_RATE_48000HZ (48000U) /* Selects a sampling rate of 48 kHz. */
#define SAMPLING_RATE_64000HZ (64000U) /* Selects a sampling rate of 64 kHz. */
#define SAMPLING_RATE_88200HZ (88200U) /* Selects a sampling rate of 88.2 kHz. */
#define SAMPLING_RATE_96000HZ (96000U) /* Selects a sampling rate of 96 kHz. */
#define SELECT_IN_DATA_CH_0   (0U)     /* Specifies audio channel 0ch. */
#define SELECT_IN_DATA_CH_1   (1U)     /* Specifies audio channel 1ch. */

/** SRC parameter information */
typedef struct
{
    bool                  src_enable;                       /**< SRC function enable setting */
    scux_data_word_len_t  word_len;                         /**< Word length of the audio data to be used
                                                                 by the SRC. */
    bool                  mode_sync;                        /**< Synchronization mode */
    uint32_t              input_rate;                       /**< Input sampling rate */
    uint32_t              output_rate;                      /**< Output sampling rate */
    uint32_t              select_in_data_ch[SCUX_USE_CH_2]; /**< For SRC's input data position swapping */
} scux_src_usr_cfg_t;

/** The SCUX module is made up of a sampling rate converter, a digital volume unit, and a mixer.
 *  The SCUX driver can perform asynchronous and synchronous sampling rate conversions using the sampling rate
 *  converter. 
 *  The SCUX driver uses the DMA transfer mode to input and output audio data.
 */
class R_BSP_Scux : public R_BSP_SerialFamily {

public:
    /** Constructor: Initializes and opens the channel designated by the SCUX driver.
     *
     * @param channel SCUX channel number
     * @param int_level Transfer interrupt priority level (0x00-0xF7; default = 0x80)
     * @param max_write_num Maximum number of writes (1 to 128; default = 16)
     * @param max_read_num Maximum number of reads (1 to 128; default = 16)
     */
    R_BSP_Scux(scux_ch_num_t channel, uint8_t int_level = 0x80, int32_t max_write_num = 16, int32_t max_read_num = 16);

    /** Destructor: Closes the channel designated by the SCUX driver and exits.
     *
     */
    virtual ~R_BSP_Scux(void);

    /** Sets up the SCUX HW and starts operation, then starts accepting write/read requests.
     * Allocates a DMA channel for read requests.
     * The function also initializes the SCUX HW error state.
     *
     * @return Returns true if the function is successful. Returns false if the function fails.
     */
    bool TransStart(void);

    /** Stops accepting write/read requests, flushes out all data in the SCUX that is requested for transfer,
     *  then stops the HW operation.
     *   If a transfer request is being processed, the function stops accepting write/read requests,
     *   then performs preprocessing to stop the HW operation after flushing out all data in the SCUX
     *   that is requested for transfer. 
     *   If no transfer request is being processed, the function stops accepting write/read requests 
     *   before stopping the HW operation.
     * The function releases the DMA channel for read requests when the HW operation is stopped.
     * It also calls the user-own callback function at the end of the transfer.
     *
     * @param callback Pointer to the callback function
     *                 The SCUX driver places an error code indicating the execution result of FlushStop 
     *                 in the argument of the callback function before calling the callback function. 
     *                 The error code is set to 0 if FlushStop is successful. A negative number is set 
     *                 if FlushStop fails.
     * @return Returns true if the function is successful. Returns false if the function fails.
     */
    bool FlushStop(void (* const callback)(int32_t));

    /** Discards all data in the SCUX that is requested for transfer before stopping the hardware operation
     *  and stops accepting write/read requests. 
     *  The function releases the DMA channel for read requests when the HW operation is stopped.
     *
     * @return Returns true if the function is successful. Returns false if the function fails.
     */
    bool ClearStop(void);

    /** Sets up SRC parameters.
     *
     * @param p_src_param SRC parameter information
     * @return Returns true if the function is successful. Returns false if the function fails.
     */
    bool SetSrcCfg(const scux_src_usr_cfg_t * const p_src_param);

    /** Obtains the state information of the write request.
     *
     * @param p_write_stat Status of the write request
     *        SCUX_STAT_STOP (0)  Acceptance of requests is stopped.
     *        SCUX_STAT_IDLE (1)  Processing of all requests is completed and waiting for a request.
     *        SCUX_STAT_TRANS(2)  Transfer in progress
     * @return Returns true if the function is successful. Returns false if the function fails.
     */
    bool GetWriteStat(uint32_t * const p_write_stat);

    /** Obtains the state information of the read request.
     *
     * @param p_read_stat ead request state
     *        SCUX_STAT_STOP (0)  Acceptance of requests is stopped
     *        SCUX_STAT_IDLE (1)  Processing of all requests is completed and waiting for a request.
     *        SCUX_STAT_TRANS(2)  Transfer in progress
     * @return Returns true if the function is successful. Returns false if the function fails.
     */
    bool GetReadStat(uint32_t * const p_read_stat);

private:
    int32_t scux_ch;
};
#endif /* TARGET_RZ_A1XX */
#endif /* R_BSP_SCUX_H */
