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
* http://www.renesas.com/disclaimer *
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.    
*******************************************************************************/
/*******************************************************************************
* System Name  : SDHI Driver
* File Name    : r_sdif.h
* Version      : 1.00
* Device(s)    : RZ/A2M
* Tool-Chain   : e2 studio (GCC ARM Embedded)
* OS           : None
* H/W Platform : RSK board
* Description  : SD Memory card driver I/F
* Operation    : 
* Limitations  : None
******************************************************************************/
/*****************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 16.03.2018 1.00     First Release
*         : 14.12.2018 1.01     Changed the DMAC soft reset procedure.
*         : 28.12.2018 1.02     Support for OS
******************************************************************************/
#ifndef _R_SDIF_H_
#define _R_SDIF_H_

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/
#ifdef    __cplusplus
extern "C" {
#endif    /* __cplusplus    */

/* ---- User Configuration ---- */
/* SD card detection option */
typedef enum
{
     SD_CD_ENABLED      /* SD card detection is enabled. */
    ,SD_CD_DISABLED     /* When SD card detection is disabled,
                           the status is always loading. */
} e_sd_cd_layout;

/* SD write protection signal detection option */
typedef enum
{
     SD_WP_ENABLED      /* Write protection signal detection is enabled. */
    ,SD_WP_DISABLED     /* When write protection signal detection is disabled,
                           the status is always write protection signal off. */
} e_sd_wp_layout;

/******************************************************************************
Macro definitions
******************************************************************************/
/* ==== define  ==== */
#define SD_SCC_IP0_BASE_ADDR      (0xE8227000uL)        /* Set the base address of SCC ch0. */
#define SD_SCC_IP1_BASE_ADDR      (0xE8229000uL)        /* Set the base address of SCC ch1. */

/* ---- SD Driver work buffer ---- */
#define SD_SIZE_OF_INIT           (240)

/* ---- error code ---- */
#define SD_OK_LOCKED_CARD         (1)                   /* OK but card is locked status */
#define SD_OK                     (0)                   /* OK */
#define SD_ERR                    (-1)                  /* general error */
#define SD_ERR_WP                 (-2)                  /* write protect error */
/* 3 */
#define SD_ERR_RES_TOE            (-4)                  /* response time out error */
#define SD_ERR_CARD_TOE           (-5)                  /* card time out error */
#define SD_ERR_END_BIT            (-6)                  /* end bit error */
#define SD_ERR_CRC                (-7)                  /* CRC error */
#define SD_ERR_ILL_ACCESS         (-8)                  /* illegal access error */
#define SD_ERR_HOST_TOE           (-9)                  /* host time out error */
#define SD_ERR_CARD_ERASE         (-10)                 /* card erase error */
#define SD_ERR_CARD_LOCK          (-11)                 /* card lock error */
#define SD_ERR_CARD_UNLOCK        (-12)                 /* card unlock error */
#define SD_ERR_HOST_CRC           (-13)                 /* host CRC error */
#define SD_ERR_CARD_ECC           (-14)                 /* card internal ECC error */
#define SD_ERR_CARD_CC            (-15)                 /* card internal error */
#define SD_ERR_CARD_ERROR         (-16)                 /* unknown card error */
#define SD_ERR_CARD_TYPE          (-17)                 /* non support card type */
#define SD_ERR_NO_CARD            (-18)                 /* no card */
#define SD_ERR_ILL_READ           (-19)                 /* illegal buffer read */
#define SD_ERR_ILL_WRITE          (-20)                 /* illegal buffer write */
#define SD_ERR_AKE_SEQ            (-21)                 /* the sequence of authentication process */
#define SD_ERR_OVERWRITE          (-22)                 /* CID/CSD overwrite error */
/* 23-29 */
#define SD_ERR_CPU_IF             (-30)                 /* target CPU interface function error  */
#define SD_ERR_STOP               (-31)                 /* user stop */
/* 32-49 */
#define SD_ERR_CSD_VER            (-50)                 /* CSD register version error */
/* 51 */
#define SD_ERR_FILE_FORMAT        (-52)                 /* CSD register file format error  */
#define SD_ERR_NOTSUP_CMD         (-53)                 /* not supported command  */
/* 54-69 */
#define SD_ERR_IFCOND_VER         (-70)                 /* Interface condition version error */
/* 71 */
#define SD_ERR_IFCOND_ECHO        (-72)                 /* Interface condition echo back pattern error */
/* 73-79 */
#define SD_ERR_OUT_OF_RANGE       (-80)                 /* the argument was out of range */
#define SD_ERR_ADDRESS_ERROR      (-81)                 /* misassigned address */
#define SD_ERR_BLOCK_LEN_ERROR    (-82)                 /* transfered block length is not allowed */
#define SD_ERR_ILLEGAL_COMMAND    (-83)                 /* Command not legal  */
#define SD_ERR_RESERVED_ERROR18   (-84)                 /* Reserved bit 18 Error */
#define SD_ERR_RESERVED_ERROR17   (-85)                 /* Reserved bit 17 Error */
#define SD_ERR_CMD_ERROR          (-86)                 /* SD_INFO2 bit  0 CMD error */
#define SD_ERR_CBSY_ERROR         (-87)                 /* SD_INFO2 bit 14 CMD Type Reg Busy error */
#define SD_ERR_NO_RESP_ERROR      (-88)                 /* SD_INFO1 bit  0 No Response error */
/* 89-98 */
#define SD_ERR_INTERNAL           (-99)                 /* driver software internal error */

/* ---- driver mode ---- */
#define SD_MODE_POLL              (0x0000ul)            /* status check mode is software polling */
#define SD_MODE_HWINT             (0x0001ul)            /* status check mode is hardware interrupt */
#define SD_MODE_SW                (0x0000ul)            /* data transfer mode is software */
#define SD_MODE_DMA               (0x0002ul)            /* data transfer mode is DMA */

/* ---- support mode ---- */
#define SD_MODE_MEM               (0x0000ul)            /* memory cards only are supported */
#define SD_MODE_DS                (0x0000ul)            /* only default speed mode is supported */
#define SD_MODE_VER1X             (0x0000ul)            /* ver1.1 host */
#define SD_MODE_VER2X             (0x0080ul)            /* ver2.x host (high capacity and dual voltage) */
#define SD_MODE_1BIT              (0x0100ul)            /* SD Mode 1bit only is supported */
#define SD_MODE_4BIT              (0x0000ul)            /* SD Mode 1bit and 4bit is supported */

/* ---- media voltage ---- */
#define SD_VOLT_1_7               (0x00000010ul)        /* low voltage card minimum */
#define SD_VOLT_1_8               (0x00000020ul)
#define SD_VOLT_1_9               (0x00000040ul)
#define SD_VOLT_2_0               (0x00000080ul)
#define SD_VOLT_2_1               (0x00000100ul)        /* basic communication minimum */
#define SD_VOLT_2_2               (0x00000200ul)
#define SD_VOLT_2_3               (0x00000400ul)
#define SD_VOLT_2_4               (0x00000800ul)
#define SD_VOLT_2_5               (0x00001000ul)
#define SD_VOLT_2_6               (0x00002000ul)
#define SD_VOLT_2_7               (0x00004000ul)
#define SD_VOLT_2_8               (0x00008000ul)        /* memory access minimum */
#define SD_VOLT_2_9               (0x00010000ul)
#define SD_VOLT_3_0               (0x00020000ul)
#define SD_VOLT_3_1               (0x00040000ul)
#define SD_VOLT_3_2               (0x00080000ul)
#define SD_VOLT_3_3               (0x00100000ul)
#define SD_VOLT_3_4               (0x00200000ul)
#define SD_VOLT_3_5               (0x00400000ul)
#define SD_VOLT_3_6               (0x00800000ul)

/* ---- memory card write mode ---- */
#define SD_WRITE_WITH_PREERASE    (0x0000u)             /* pre-erease write */
#define SD_WRITE_OVERWRITE        (0x0001u)             /* overwrite  */

 /* ---- media type ---- */
#define SD_MEDIA_UNKNOWN          (0x0000u)             /* unknown media */
#define SD_MEDIA_MMC              (0x0010u)             /* MMC card */
#define SD_MEDIA_SD               (0x0020u)             /* SD Memory card */
#define SD_MEDIA_MEM              (0x0030u)             /* Memory card */

/* ---- write protect info --- */
#define SD_WP_OFF                 (0x0000u)             /* card is not write protect */
#define SD_WP_HW                  (0x0001u)             /* card is H/W write protect */
#define SD_WP_TEMP                (0x0002u)             /* card is TEMP_WRITE_PROTECT */
#define SD_WP_PERM                (0x0004u)             /* card is PERM_WRITE_PROTECT */
#define SD_WP_ROM                 (0x0010u)             /* card is SD-ROM */

/* ---- SD clock div ---- */    /* IMCLK is host controller clock */
#define SD_DIV_512                (0x0080u)             /* SDCLOCK = IMCLK/512 */
#define SD_DIV_256                (0x0040u)             /* SDCLOCK = IMCLK/256 */
#define SD_DIV_128                (0x0020u)             /* SDCLOCK = IMCLK/128 */
#define SD_DIV_64                 (0x0010u)             /* SDCLOCK = IMCLK/64 */
#define SD_DIV_32                 (0x0008u)             /* SDCLOCK = IMCLK/32 */
#define SD_DIV_16                 (0x0004u)             /* SDCLOCK = IMCLK/16 */
#define SD_DIV_8                  (0x0002u)             /* SDCLOCK = IMCLK/8 */
#define SD_DIV_4                  (0x0001u)             /* SDCLOCK = IMCLK/4 */
#define SD_DIV_2                  (0x0000u)             /* SDCLOCK = IMCLK/2 */
#define SD_DIV_1                  (0x00FFu)             /* SDCLOCK = IMCLK (option) */

/* ---- SD clock define ---- */    /* max frequency */
#define SD_CLK_400kHz             (0x0000u)             /* 400kHz */
#define SD_CLK_1MHz               (0x0001u)             /* 1MHz */
#define SD_CLK_5MHz               (0x0002u)             /* 5MHz */
#define SD_CLK_10MHz              (0x0003u)             /* 10MHz */
#define SD_CLK_20MHz              (0x0004u)             /* 20MHz */
#define SD_CLK_25MHz              (0x0005u)             /* 25MHz */
#define SD_CLK_50MHz              (0x0006u)             /* 50MHz (phys spec ver1.10) */

/* ---- speed class ---- */
#define SD_SPEED_CLASS_0          (0x00u)               /* not defined, or less than ver2.0 */
#define SD_SPEED_CLASS_2          (0x01u)               /* 2MB/sec */
#define SD_SPEED_CLASS_4          (0x02u)               /* 4MB/sec */
#define SD_SPEED_CLASS_6          (0x03u)               /* 6MB/sec */

/* ---- SD port mode ---- */
#define SD_PORT_SERIAL            (0x0000u)             /* 1bit mode */
#define SD_PORT_PARALLEL          (0x0001u)             /* 4bits mode */

/* ---- SD Card detect port ---- */
#define SD_CD_SOCKET              (0x0000u)             /* CD pin */
#define SD_CD_DAT3                (0x0001u)             /* DAT3 pin */

/* ---- SD Card detect interrupt ---- */
#define SD_CD_INT_DISABLE         (0x0000u)             /* card detect interrupt disable */
#define SD_CD_INT_ENABLE          (0x0001u)             /* card detect interrupt enable */

/* ---- lock/unlock mode ---- */
#define SD_FORCE_ERASE            (0x08)
#define SD_LOCK_CARD              (0x04)
#define SD_UNLOCK_CARD            (0x00)
#define SD_CLR_PWD                (0x02)
#define SD_SET_PWD                (0x01)

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
/* ==== API prototype ===== */
/* ---- access library I/F ---- */
int32_t sd_init(int32_t sd_port, uint32_t base, void *workarea, int32_t cd_port);
int32_t sd_cd_int(int32_t sd_port, int32_t enable,int32_t (*callback)(int32_t, int32_t));
int32_t sd_check_media(int32_t sd_port);
int32_t sd_mount(int32_t sd_port, uint32_t mode,uint32_t voltage);
int32_t sd_read_sect(int32_t sd_port, uint8_t *buff,uint32_t psn,int32_t cnt);
int32_t sd_write_sect(int32_t sd_port, uint8_t *buff,uint32_t psn,int32_t cnt,int32_t writemode);
int32_t sd_get_type(int32_t sd_port, uint16_t *type,uint16_t *speed,uint8_t *capa);
int32_t sd_get_size(int32_t sd_port, uint32_t *user,uint32_t *protect);
int32_t sd_iswp(int32_t sd_port);
int32_t sd_unmount(int32_t sd_port);
void    sd_stop(int32_t sd_port);
int32_t sd_set_intcallback(int32_t sd_port, int32_t (*callback)(int32_t, int32_t));
int32_t sd_set_dma_intcallback(int32_t sd_port, int32_t (*callback)(int32_t, int32_t));
void    sd_int_handler(int32_t sd_port);
int32_t sd_get_error(int32_t sd_port);
int32_t sd_check_int(int32_t sd_port);
void    sd_int_dm_handler(int32_t sd_port);
int32_t sd_check_int_dm(int32_t sd_port);
int32_t sd_get_reg(int32_t sd_port, uint8_t *ocr,uint8_t *cid,uint8_t *csd, uint8_t *dsr,uint8_t *scr);
int32_t sd_get_rca(int32_t sd_port, uint8_t *rca);
int32_t sd_get_sdstatus(int32_t sd_port, uint8_t *sdstatus);
int32_t sd_get_speed(int32_t sd_port, uint8_t *clss,uint8_t *move);
int32_t sd_finalize(int32_t sd_port);
int32_t sd_set_seccnt(int32_t sd_port, int16_t sectors);
int32_t sd_get_seccnt(int32_t sd_port);
int32_t sd_get_ver(int32_t sd_port, uint16_t *sdhi_ver,char_t *sddrv_ver);
int32_t sd_set_cdtime(int32_t sd_port, uint16_t cdtime);
int32_t sd_set_responsetime(int32_t sd_port, uint16_t responsetime);
int32_t sd_set_buffer(int32_t sd_port, void *buff,uint32_t size);
int32_t sd_inactive(int32_t sd_port);
int32_t sd_set_tmpwp(int32_t sd_port, int32_t is_set);
int32_t sd_lock_unlock(int32_t sd_port, uint8_t code,uint8_t *pwd,uint8_t len);

/* ---- target CPU I/F ---- */
int32_t sddev_init(int32_t sd_port);
int32_t sddev_power_on(int32_t sd_port);
int32_t sddev_power_off(int32_t sd_port);
int32_t sddev_read_data(int32_t sd_port, uint8_t *buff,uint32_t reg_addr,int32_t num);
int32_t sddev_write_data(int32_t sd_port, uint8_t *buff,uint32_t reg_addr,int32_t num);
uint32_t sddev_get_clockdiv(int32_t sd_port, int32_t clock);
int32_t sddev_set_port(int32_t sd_port, int32_t mode);
int32_t sddev_int_wait(int32_t sd_port, int32_t time);
int32_t sddev_init_dma(int32_t sd_port, uint32_t buff, int32_t dir);
int32_t sddev_wait_dma_end(int32_t sd_port, int32_t cnt);
int32_t sddev_disable_dma(int32_t sd_port);
int32_t sddev_reset_dma(int32_t sd_port);
int32_t sddev_finalize_dma(int32_t sd_port);
int32_t sddev_finalize(int32_t sd_port);
int32_t sddev_loc_cpu(int32_t sd_port);
int32_t sddev_unl_cpu(int32_t sd_port);
int32_t sddev_cd_layout(int32_t sd_port);
int32_t sddev_wp_layout(int32_t sd_port);
int32_t SD_status_callback_function(int32_t sd_port, int32_t cd);
int32_t SD_dma_end_callback_function(int32_t sd_port, int32_t cd);
void    SD_confirm_semaphore(int32_t sd_port);

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

#ifdef    __cplusplus
}
#endif    /* __cplusplus    */

#endif    /* _R_SDIF_H_ */

/* End of File */
