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

#include "R_BSP_Scux.h"
#if (R_BSP_SCUX_ENABLE == 1)
#include "r_bsp_cmn.h"
#include "scux_if.h"

#define CH_ERR_NUM                  (-1)     /* Channel error number */
#define INT_LEVEL_MAX               (0xF7)   /* The maximum value of the interrupt level */
#define REQ_BUFF_NUM_MIN            (1)      /* The minimum value of the request buffer */
#define REQ_BUFF_NUM_MAX            (128)    /* The maximum value of the request buffer */
#define INPUT_DIV_INIT_VALUE        (1000U)  /* The initial value of input divide ratio */
#define OUTPUT_DIV_INIT_VALUE       (0U)     /* The initial value of output divide ratio  */
#define INPUT_WS_INIT_VALUE         (1U)     /* The initial value of input WS frequency */
#define OUTPUT_WS_INIT_VALUE        (96000U) /* The initial value of output WS frequency */
#define FREQ_TIOC3A_INIT_VALUE      (1U)     /* The initial value of frequency of TIOC3 */
#define FREQ_TIOC4A_INIT_VALUE      (1U)     /* The initial value of frequency of TIOC4 */
#define WAIT_SAMPLE_INIT_VALUE      (0U)     /* The initial value of wait time */
#define MIN_RATE_PER_INIT_VALUE     (98U)    /* The initial value of minimum rate */
#define DIV_RATIO_CLK_AUDIO_22050HZ (1024U)  /* Divide ratio when the frequency is 22050Hz */
#define DIV_RATIO_CLK_AUDIO_44100HZ (512U)   /* Divide ratio when the frequency is 44100Hz */
#define DIV_RATIO_CLK_AUDIO_88200HZ (256U)   /* Divide ratio when the frequency is 88200Hz */
#define DIV_RATIO_CLK_USB_24000HZ   (2000U)  /* Divide ratio when the frequency is 24000Hz */
#define DIV_RATIO_CLK_USB_32000HZ   (1500U)  /* Divide ratio when the frequency is 36000Hz */
#define DIV_RATIO_CLK_USB_48000HZ   (1000U)  /* Divide ratio when the frequency is 48000Hz */
#define DIV_RATIO_CLK_USB_64000HZ   (750U)   /* Divide ratio when the frequency is 64000Hz */
#define DIV_RATIO_CLK_USB_96000HZ   (500U)   /* Divide ratio when the frequency is 96000Hz */

static bool set_src_init_cfg(scux_src_cfg_t * const src_cfg);

R_BSP_Scux::R_BSP_Scux(scux_ch_num_t channel, uint8_t int_level, int32_t max_write_num, int32_t max_read_num) {
    scux_channel_cfg_t scux_cfg;
    int32_t result;
    bool    init_result;

    if (channel >= SCUX_CH_NUM) {
        result = EERROR;
    } else if (int_level > INT_LEVEL_MAX) {
        result = EERROR;
    } else if ((max_write_num < REQ_BUFF_NUM_MIN) || (max_write_num > REQ_BUFF_NUM_MAX)) {
        result = EERROR;
    } else if ((max_read_num < REQ_BUFF_NUM_MIN) || (max_read_num > REQ_BUFF_NUM_MAX)) {
        result = EERROR;
    } else {
        result = R_BSP_CMN_Init();
        if (result == ESUCCESS) {
            scux_ch = (int32_t)channel;

            scux_cfg.enabled       = true;
            scux_cfg.int_level     = int_level;

            switch (channel) {
                case SCUX_CH_0:
                    scux_cfg.route = SCUX_ROUTE_SRC0_MEM;
                    break;
                case SCUX_CH_1:
                    scux_cfg.route = SCUX_ROUTE_SRC1_MEM;
                    break;
                case SCUX_CH_2:
                    scux_cfg.route = SCUX_ROUTE_SRC2_MEM;
                    break;
                case SCUX_CH_3:
                    scux_cfg.route = SCUX_ROUTE_SRC3_MEM;
                    break;
                default:
                    /* NOTREACHED on At the time of a normal performance */
                    scux_cfg.route = SCUX_ROUTE_SRC0_MEM;
                    break;
            }

            init_result = set_src_init_cfg(&scux_cfg.src_cfg);
            if (init_result != false) {
                init_result = init_channel(R_SCUX_MakeCbTbl_mbed(), (int32_t)channel, &scux_cfg, max_write_num, max_read_num);
                if (init_result == false) {
                    result = EERROR;
                }
            } else {
                result = EERROR;
            }
        }
    }

    if (result != ESUCCESS) {
        scux_ch   = CH_ERR_NUM;
    }
}

R_BSP_Scux::~R_BSP_Scux(void) {
}

bool R_BSP_Scux::TransStart(void) {
    return ioctl(SCUX_IOCTL_SET_START, NULL);
}

bool R_BSP_Scux::FlushStop(void (* const callback)(int32_t)) {
    return ioctl(SCUX_IOCTL_SET_FLUSH_STOP, (void *)callback);
}

bool R_BSP_Scux::ClearStop(void) {
    return ioctl(SCUX_IOCTL_SET_CLEAR_STOP, NULL);
}

bool R_BSP_Scux::SetSrcCfg(const scux_src_usr_cfg_t * const p_src_param) {
    scux_src_cfg_t src_cfg;
    bool    init_result;
    bool    ret = true;
    int32_t i;

    if (scux_ch == CH_ERR_NUM) {
        ret = false;
    } else if (p_src_param == NULL) {
        ret = false;
    } else if ((p_src_param->mode_sync != false) && (p_src_param->src_enable == false)) {
        ret = false;
    } else {
        init_result = set_src_init_cfg(&src_cfg);
        if (init_result != true) {
            ret = false;
        } else {
            src_cfg.src_enable = p_src_param->src_enable;
            src_cfg.mode_sync  = p_src_param->mode_sync;

            switch (p_src_param->word_len) {
                case SCUX_DATA_LEN_24:
                    /* fall through */
                case SCUX_DATA_LEN_16:
                    /* fall through */
                case SCUX_DATA_LEN_16_TO_24:
                    src_cfg.word_len = p_src_param->word_len;
                    break;
                default:
                    ret = false;
                    break;
            }

            if (ret == true) {
                if (p_src_param->mode_sync != false) {
                    switch (p_src_param->input_rate) {
                        case SAMPLING_RATE_8000HZ:
                            src_cfg.input_rate_sync = SCUX_SYNC_RATE_8;
                            break;
                        case SAMPLING_RATE_11025HZ:
                            src_cfg.input_rate_sync = SCUX_SYNC_RATE_11_025;
                            break;
                        case SAMPLING_RATE_12000HZ:
                            src_cfg.input_rate_sync = SCUX_SYNC_RATE_12;
                            break;
                        case SAMPLING_RATE_16000HZ:
                            src_cfg.input_rate_sync = SCUX_SYNC_RATE_16;
                            break;
                        case SAMPLING_RATE_22050HZ:
                            src_cfg.input_rate_sync = SCUX_SYNC_RATE_22_05;
                            break;
                        case SAMPLING_RATE_24000HZ:
                            src_cfg.input_rate_sync = SCUX_SYNC_RATE_24;
                            break;
                        case SAMPLING_RATE_32000HZ:
                            src_cfg.input_rate_sync = SCUX_SYNC_RATE_32;
                            break;
                        case SAMPLING_RATE_44100HZ:
                            src_cfg.input_rate_sync = SCUX_SYNC_RATE_44_1;
                            break;
                        case SAMPLING_RATE_48000HZ:
                            src_cfg.input_rate_sync = SCUX_SYNC_RATE_48;
                            break;
                        case SAMPLING_RATE_64000HZ:
                            src_cfg.input_rate_sync = SCUX_SYNC_RATE_64;
                            break;
                        case SAMPLING_RATE_88200HZ:
                            src_cfg.input_rate_sync = SCUX_SYNC_RATE_88_2;
                            break;
                        case SAMPLING_RATE_96000HZ:
                            src_cfg.input_rate_sync = SCUX_SYNC_RATE_96;
                            break;
                        default:
                            ret = false;
                            break;
                    }
                } else {
                    switch (p_src_param->input_rate) {
                        case SAMPLING_RATE_22050HZ:
                            src_cfg.input_clk_async = SCUX_CLK_AUDIO_X1;
                            src_cfg.input_div_async = DIV_RATIO_CLK_AUDIO_22050HZ;
                            break;
                        case SAMPLING_RATE_24000HZ:
                            src_cfg.input_clk_async = SCUX_CLK_USB_X1;
                            src_cfg.input_div_async = DIV_RATIO_CLK_USB_24000HZ;
                            break;
                        case SAMPLING_RATE_32000HZ:
                            src_cfg.input_clk_async = SCUX_CLK_USB_X1;
                            src_cfg.input_div_async = DIV_RATIO_CLK_USB_32000HZ;
                            break;
                        case SAMPLING_RATE_44100HZ:
                            src_cfg.input_clk_async = SCUX_CLK_AUDIO_X1;
                            src_cfg.input_div_async = DIV_RATIO_CLK_AUDIO_44100HZ;
                            break;
                        case SAMPLING_RATE_48000HZ:
                            src_cfg.input_clk_async = SCUX_CLK_USB_X1;
                            src_cfg.input_div_async = DIV_RATIO_CLK_USB_48000HZ;
                            break;
                        case SAMPLING_RATE_64000HZ:
                            src_cfg.input_clk_async = SCUX_CLK_USB_X1;
                            src_cfg.input_div_async = DIV_RATIO_CLK_USB_64000HZ;
                            break;
                        case SAMPLING_RATE_88200HZ:
                            src_cfg.input_clk_async = SCUX_CLK_AUDIO_X1;
                            src_cfg.input_div_async = DIV_RATIO_CLK_AUDIO_88200HZ;
                            break;
                        case SAMPLING_RATE_96000HZ:
                            src_cfg.input_clk_async = SCUX_CLK_USB_X1;
                            src_cfg.input_div_async = DIV_RATIO_CLK_USB_96000HZ;
                            break;
                        default:
                            ret = false;
                            break;
                    }
                }
            }

            if (ret == true) {
                if (p_src_param->mode_sync != false) {
                    switch (p_src_param->output_rate) {
                        case SAMPLING_RATE_44100HZ:
                            src_cfg.output_rate_sync = SCUX_SYNC_RATE_44_1;
                            break;
                        case SAMPLING_RATE_48000HZ:
                            src_cfg.output_rate_sync = SCUX_SYNC_RATE_48;
                            break;
                        case SAMPLING_RATE_96000HZ:
                            src_cfg.output_rate_sync = SCUX_SYNC_RATE_96;
                            break;
                        default:
                            ret = false;
                            break;
                    }
                } else {
                    switch (p_src_param->output_rate) {
                        case SAMPLING_RATE_44100HZ:
                            src_cfg.output_ws = SAMPLING_RATE_44100HZ;
                            break;
                        case SAMPLING_RATE_48000HZ:
                            src_cfg.output_ws = SAMPLING_RATE_48000HZ;
                            break;
                        case SAMPLING_RATE_88200HZ:
                            src_cfg.output_ws = SAMPLING_RATE_88200HZ;
                            break;
                        case SAMPLING_RATE_96000HZ:
                            src_cfg.output_ws = SAMPLING_RATE_96000HZ;
                            break;
                        default:
                            ret = false;
                            break;
                    }
                }
            }

            if (ret == true) {
                for (i = 0; i < SCUX_USE_CH_2; i++) {
                    switch (p_src_param->select_in_data_ch[i]) {
                        case SELECT_IN_DATA_CH_0:
                            src_cfg.select_in_data_ch[i] = SCUX_AUDIO_CH_0;
                            break;
                        case SELECT_IN_DATA_CH_1:
                            src_cfg.select_in_data_ch[i] = SCUX_AUDIO_CH_1;
                            break;
                        default:
                            ret = false;
                            break;
                    }
                }
            }

            if (ret == true) {
                ret = ioctl(SCUX_IOCTL_SET_SRC_CFG, (void *)&src_cfg);
            }
        }
    }

    return ret;
}

bool R_BSP_Scux::GetWriteStat(uint32_t * const p_write_stat) {
    return ioctl(SCUX_IOCTL_GET_WRITE_STAT, (void *)p_write_stat);
}

bool R_BSP_Scux::GetReadStat(uint32_t * const p_read_stat) {
    return ioctl(SCUX_IOCTL_GET_READ_STAT, (void *)p_read_stat);
}

/**************************************************************************//**
* Function Name: set_src_init_cfg
* @brief         SRC configuration initialization.
*
*                Description:<br>
*                
* @param[in]     src_cfg SRC configuration. 
* @retval        true  Setting success.
*                false Setting fails.
******************************************************************************/
static bool set_src_init_cfg(scux_src_cfg_t * const src_cfg) {
    bool ret = true;

    if (src_cfg == NULL) {
        ret = false;
    } else {
        src_cfg->src_enable                         = true;
        src_cfg->use_ch                             = SCUX_USE_CH_2;
        src_cfg->word_len                           = SCUX_DATA_LEN_16;
        src_cfg->mode_sync                          = true;
        src_cfg->input_rate_sync                    = SCUX_SYNC_RATE_48;
        src_cfg->input_clk_async                    = SCUX_CLK_USB_X1;
        src_cfg->input_div_async                    = INPUT_DIV_INIT_VALUE;
        src_cfg->output_rate_sync                   = SCUX_SYNC_RATE_96;
        src_cfg->output_clk_async                   = SCUX_CLK_SSIF0_WS;
        src_cfg->output_div_async                   = OUTPUT_DIV_INIT_VALUE;
        src_cfg->input_ws                           = INPUT_WS_INIT_VALUE;
        src_cfg->output_ws                          = OUTPUT_WS_INIT_VALUE;
        src_cfg->freq_tioc3a                        = FREQ_TIOC3A_INIT_VALUE;
        src_cfg->freq_tioc4a                        = FREQ_TIOC4A_INIT_VALUE;
        src_cfg->delay_mode                         = SCUX_DELAY_NORMAL;
        src_cfg->wait_sample                        = WAIT_SAMPLE_INIT_VALUE;
        src_cfg->min_rate_percentage                = MIN_RATE_PER_INIT_VALUE;
        src_cfg->select_in_data_ch[SCUX_AUDIO_CH_0] = SCUX_AUDIO_CH_0;
        src_cfg->select_in_data_ch[SCUX_AUDIO_CH_1] = SCUX_AUDIO_CH_1;
        src_cfg->select_in_data_ch[SCUX_AUDIO_CH_2] = SCUX_AUDIO_CH_2;
        src_cfg->select_in_data_ch[SCUX_AUDIO_CH_3] = SCUX_AUDIO_CH_3;
        src_cfg->select_in_data_ch[SCUX_AUDIO_CH_4] = SCUX_AUDIO_CH_4;
        src_cfg->select_in_data_ch[SCUX_AUDIO_CH_5] = SCUX_AUDIO_CH_5;
        src_cfg->select_in_data_ch[SCUX_AUDIO_CH_6] = SCUX_AUDIO_CH_6;
        src_cfg->select_in_data_ch[SCUX_AUDIO_CH_7] = SCUX_AUDIO_CH_7;
    }

    return ret;
}

#endif /* R_BSP_SCUX_ENABLE */
