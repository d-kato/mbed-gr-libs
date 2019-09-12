/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/*******************************************************************************
* System Name  : SDHI Driver
* File Name    : sd.h
* Version      : 1.20
* Device(s)    : RZ/A2M
* Tool-Chain   : e2 studio (GCC ARM Embedded)
* OS           : None
* H/W Platform : RZ/A2M Evaluation Board
* Description  : SD Driver header file
* Operation    :
* Limitations  : None
******************************************************************************/
/*****************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 16.03.2018 1.00     First Release
*         : 14.12.2018 1.01     Changed the DMAC soft reset procedure.
*         : 28.12.2018 1.02     Support for OS
*         : 29.05.2019 1.20     Correspond to internal coding rules
******************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "sys_sel.h"

#ifndef SD_H
#define SD_H

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef union
{
    uint64_t        longlong;
    struct
    {
        uint32_t    l_;
        uint32_t    h_;
    } st_long_t;
    struct
    {
        uint16_t    ll_;
        uint16_t    lh_;
        uint16_t    hl_;
        uint16_t    hh_;
    } st_word_t;
} u_sd_reg_t;

/******************************************************************************
Macro definitions
******************************************************************************/
/* ==== option ==== */
#define SD_UNMOUNT_CARD             (0x00u)
#define SD_MOUNT_UNLOCKED_CARD      (0x01u)
#define SD_MOUNT_LOCKED_CARD        (0x02u)
#define SD_CARD_LOCKED              (0x04u)

/* ==== SDHI register address ==== */
#define SD_CMD          ((0x0000u<<SD_REG_SHIFT)+SD_BYTE_OFFSET) /* Command Type                                 */
#define SD_ARG          ((0x0010u<<SD_REG_SHIFT)+SD_BYTE_OFFSET) /* SD Command Argument CF39-CF8(32bit)          */
#define SD_ARG1         ((0x0018u<<SD_REG_SHIFT)+SD_BYTE_OFFSET) /* SD Command Argument CF39-CF24(high 16bits)   */
#define SD_STOP         ((0x0020u<<SD_REG_SHIFT)+SD_BYTE_OFFSET) /* Data Stop                                    */
#define SD_SECCNT       ((0x0028u<<SD_REG_SHIFT)+SD_BYTE_OFFSET) /* Block Count                                  */
#define SD_RSP10        ((0x0030u<<SD_REG_SHIFT)+SD_BYTE_OFFSET) /* SD Card Response R71-8(64bit)                */
#define SD_RSP1         ((0x0038u<<SD_REG_SHIFT)+SD_BYTE_OFFSET) /* SD Card Response R39-24                      */
#define SD_RSP32        ((0x0040u<<SD_REG_SHIFT)+SD_BYTE_OFFSET) /* SD Card Response R71-40(32bit)               */
#define SD_RSP3         ((0x0048u<<SD_REG_SHIFT)+SD_BYTE_OFFSET) /* SD Card Response R71-56                      */
#define SD_RSP54        ((0x0050u<<SD_REG_SHIFT)+SD_BYTE_OFFSET) /* SD Card Response R127-72(56bit)              */
#define SD_RSP5         ((0x0058u<<SD_REG_SHIFT)+SD_BYTE_OFFSET) /* SD Card Response R103-88                     */
#define SD_RSP76        ((0x0060u<<SD_REG_SHIFT)+SD_BYTE_OFFSET) /* SD Card Response R127-104(24bit)             */
#define SD_RSP7         ((0x0068u<<SD_REG_SHIFT)+SD_BYTE_OFFSET) /* SD Card Response R127-120                    */
#define SD_INFO1        ((0x0070u<<SD_REG_SHIFT)+SD_BYTE_OFFSET) /* SD Card Interrupt Flag(1)                    */
#define SD_INFO2        ((0x0078u<<SD_REG_SHIFT)+SD_BYTE_OFFSET) /* SD Card Interrupt Flag(2)                    */
#define SD_INFO1_MASK   ((0x0080u<<SD_REG_SHIFT)+SD_BYTE_OFFSET) /* SD_INFO1 Interrupt Mask                      */
#define SD_INFO2_MASK   ((0x0088u<<SD_REG_SHIFT)+SD_BYTE_OFFSET) /* SD_INFO2 Interrupt Mask                      */
#define SD_CLK_CTRL     ((0x0090u<<SD_REG_SHIFT)+SD_BYTE_OFFSET) /* SD Clock Control                             */
#define SD_SIZE         ((0x0098u<<SD_REG_SHIFT)+SD_BYTE_OFFSET) /* Transfer Data Length                         */
#define SD_OPTION       ((0x00A0u<<SD_REG_SHIFT)+SD_BYTE_OFFSET) /* SD Card Access Control Option                */
#define SD_ERR_STS1     ((0x00B0u<<SD_REG_SHIFT)+SD_BYTE_OFFSET) /* SD Error Status 1                            */
#define SD_ERR_STS2     ((0x00B8u<<SD_REG_SHIFT)+SD_BYTE_OFFSET) /* SD Error Status 2                            */
#define SD_BUF0         ((0x00C0u<<SD_REG_SHIFT)+SD_BYTE_OFFSET) /* SD Buffer Read/Write                         */
#define CC_EXT_MODE     ((0x0360u<<SD_REG_SHIFT)+SD_BYTE_OFFSET) /* DMA Mode Enable                              */
#define SOFT_RST        ((0x0380u<<SD_REG_SHIFT)+SD_BYTE_OFFSET) /* Soft Reset                                   */
#define VERSION         ((0x0388u<<SD_REG_SHIFT)+SD_BYTE_OFFSET) /* Version                                      */

#define HOST_MODE                   ((0x0390u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)
#define SDIF_MODE                   ((0x0398u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)
#define SD_STATUS                   ((0x03C8u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)
#define DM_CM_DTRAN_MODE            ((0x0820u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)
#define DM_CM_DTRAN_CTRL            ((0x0828u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)
#define DM_CM_RST                   ((0x0830u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)
#define DM_CM_INFO1                 ((0x0840u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)
#define DM_CM_INFO1_MASK            ((0x0848u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)
#define DM_CM_INFO2                 ((0x0850u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)
#define DM_CM_INFO2_MASK            ((0x0858u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)
#define DM_DTRAN_ADDR               ((0x0880u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)

/* ==== SCC register address ==== */
#define SCC_DTCNTL                  (0x0000u<<SD_REG_SHIFT)
#define SCC_TAPSET                  (0x0008u<<SD_REG_SHIFT)
#define SCC_DT2FF                   (0x0010u<<SD_REG_SHIFT)
#define SCC_CKSEL                   (0x0018u<<SD_REG_SHIFT)
#define SCC_RVSCNTL                 (0x0020u<<SD_REG_SHIFT)
#define SCC_RVSREQ                  (0x0028u<<SD_REG_SHIFT)
#define SCC_SMPCMP                  (0x0030u<<SD_REG_SHIFT)

/* ==== command type ==== */
/* ---- SD commands ---- */
#define CMD0                        (0u)                        /* GO_IDLE_STATE */
#define CMD1                        (1u)                        /* SD_SEND_OP_COND for MMC */
#define CMD2                        (2u)                        /* ALL_SEND_CID */
#define CMD3                        (3u)                        /* SEND_RELATIVE_ADDR */
#define CMD4                        (4u)                        /* SET_DSR */
#define CMD7                        (7u)                        /* SELECT/DESELECT_CARD */
#define CMD9                        (9u)                        /* SEND_CSD */
#define CMD10                       (10u)                       /* SEND_CID */
#define CMD12                       (12u)                       /* STOP_TRANSMISSION */
#define CMD13                       (13u)                       /* SEND_STATUS */
#define CMD15                       (15u)                       /* GO_INACTIVE_STATE */
#define CMD16                       (16u)                       /* SET_BLOCK_LEN */
#define CMD17                       (17u)                       /* READ_SINGLE_BLOCK */
#define CMD18                       (18u)                       /* READ_MULTIPLE_BLOCK */
#define CMD24                       (24u)                       /* WRITE_SINGLE_BLOCK */
#define CMD25                       (25u)                       /* WRITE_MULTIPLE_BLOCK */
#define CMD27                       (27u)                       /* PROGRAM_CSD */
#define CMD28                       (28u)                       /* SET_WRITE_PROT */
#define CMD29                       (29u)                       /* CLR_WRITE_PROT */
#define CMD30                       (30u)                       /* SEND_WRITE_PROT */
#define CMD32                       (32u)                       /* ERASE_WR_BLK_START */
#define CMD33                       (33u)                       /* ERASE_WR_BLK_END */
#define CMD35                       (35u)                       /* ERASE_GROUP_START */
#define CMD36                       (36u)                       /* ERASE_GROUP_END */
#define CMD38                       (38u)                       /* ERASE */
#define CMD42                       (42u)                       /* LOCK_UNLOCK */
#define CMD55                       (55u)                       /* APP_CMD */

/* ---- switch function command (phys spec ver1.10) ---- */
#define CMD6                        (0x1C06u)                   /* SWITCH_FUNC */

/* ---- dual voltage inquiry command (phys spec ver2.0) ---- */
#define CMD8                        (0x0408u)                   /* SEND_IF_COND */

/* ---- application specific commands ---- */
#define ACMD6                       (0x40u|6u)                  /* SET_BUS_WIDTH */
#define ACMD13                      (0x40u|13u)                 /* SD_STATUS */
#define ACMD22                      (0x40u|22u)                 /* SEND_NUM_WR_BLOCKS */
#define ACMD23                      (0x40u|23u)                 /* SET_WR_BLK_ERASE_COUNT */
#define ACMD41                      (0x40u|41u)                 /* SD_SEND_OP_COND */
#define ACMD42                      (0x40u|42u)                 /* SET_CLR_CARD_DETECT */
#define ACMD51                      (0x40u|51u)                 /* SEND_SCR */

/* ---- security commands (security spec ver1.01) ---- */
#define ACMD18                      (0x40u|18u)                 /* SECURE_READ_MULTIPLE_BLOCK */
#define ACMD25                      (0x40u|25u)                 /* SECURE_WRITE_MULTIPLE_BLOCK */
#define ACMD26                      (0x40u|26u)                 /* SECURE_WRITE_MKB */
#define ACMD38                      (0x40u|38u)                 /* SECURE_ERASE */
#define ACMD43                      (0x40u|43u)                 /* GET_MKB */
#define ACMD44                      (0x40u|44u)                 /* GET_MID */
#define ACMD45                      (0x40u|45u)                 /* SET_CER_RN1 */
#define ACMD46                      (0x40u|46u)                 /* GET_CER_RN2 */
#define ACMD47                      (0x40u|47u)                 /* SET_CER_RES2 */
#define ACMD48                      (0x40u|48u)                 /* GET_CER_RES1 */
#define ACMD49                      (0x40u|49u)                 /* CHANGE_SECURE_AREA */

/* ==== constants ==== */
/* --- command arg --- */
#define ARG_ACMD6_1BIT              (0)
#define ARG_ACMD6_4BIT              (2)

/* ---- response type  ---- */
#define SD_RSP_NON                  (0)                         /* no response */
#define SD_RSP_R1                   (1)                         /* nomal response */
#define SD_RSP_R1B                  (2)                         /* nomal response with an optional busy signal */
#define SD_RSP_R1_SCR               (3)                         /* nomal response with an optional busy signal */
#define SD_RSP_R2_CID               (4)                         /* CID register */
#define SD_RSP_R2_CSD               (5)                         /* CSD register */
#define SD_RSP_R3                   (6)                         /* OCR register */
#define SD_RSP_R6                   (7)                         /* Published RCA response */
#define SD_RSP_R7                   (10)                        /* Card Interface Condition response */

/* --- R1 response error bit ---- */
#define RES_SW_INTERNAL             (0xe8400000ul)              /* Driver illegal process */
                                                                /* OUT_OF_RANGE */
                                                                /* ADDRESS_ERROR */
                                                                /* BLOCK_LEN_ERROR */
                                                                /* ERASE_PARAM */
                                                                /* RES_ILLEGAL_COMMAND */
#define RES_ERASE_SEQ_ERROR         (0x10008000ul)              /* ERASE_SEQ_ERROR + WP_ERASE_SKIP */
#define RES_WP_VIOLATION            (0x04000000ul)
#define RES_CARD_IS_LOCKED          (0x02000000ul)
#define RES_CARD_UNLOCKED_FAILED    (0x01000000ul)
#define RES_COM_CRC_ERROR           (0x00800000ul)
#define RES_CARD_ECC_FAILED         (0x00200000ul)
#define RES_CC_ERROR                (0x00100000ul)
#define RES_ERROR                   (0x00080000ul)
#define RES_AKE_SEQ_ERROR           (0x00000008ul)
#define RES_STATE                   (0x00001e00ul)

/* --- current_state --- */
#define STATE_IDEL                  (0)
#define STATE_READY                 (1u<<9u)
#define STATE_IDENT                 (2u<<9u)
#define STATE_STBY                  (3u<<9u)
#define STATE_TRAN                  (4u<<9u)
#define STATE_DATA                  (5u<<9u)
#define STATE_RCV                   (6u<<9u)
#define STATE_PRG                   (7u<<9u)
#define STATE_DIS                   (8u<<9u)

/* ---- maximum block count per multiple command ---- */
#define TRANS_SECTORS               (p_hndl->trans_sectors)     /* max 65535 blocks */
#define TRANS_BLOCKS                (p_hndl->trans_blocks)      /* max 65535 blocks */

/* ---- set block address, if HC card ---- */
#define SET_ACC_ADDR                ((p_hndl->csd_structure == 0x01) ? (psn) : (psn*512))

/* ---- SD clock control ---- */
#define SD_CLOCK_ENABLE             (1)                         /* supply clock */
#define SD_CLOCK_DISABLE            (0)                         /* halt clock */

/* ---- SD_OPTION register ---- */
#define SD_OPTION_INIT              ((uint64_t)(0x00ee))        /* SD_OPTION initial value */
                                                                /* b7-4 : timeout counter = SD_CLK x 2^27           */
                                                                /* b3-0 : card detect timer counter = SD_CLK x 2^24 */
#define SD_OPTION_TOP_MAX           ((uint64_t)(0x02e0))        /* SD_OPTION TOP value is max.                      */
#define SD_OPTION_TOP_MASK          ((uint64_t)(0x02f0))        /* SD_OPTION TOP mask                               */
#define SD_OPTION_WIDTH_MASK        (SD_OPTION_WIDTH|SD_OPTION_WIDTH8)
                                                                /* SD_OPTION Bus width mask                         */
#define SD_OPTION_WIDTH             ((uint64_t)(0x8000))        /* b15 : Bus width(WIDTH)                           */
#define SD_OPTION_WIDTH8            ((uint64_t)(0x2000))        /* b13 : Bus width(WIDTH8)                          */
                                                                /* WIDTH,WIDTH8                                     */
                                                                /*     0,     1 = 8 bits bus width                  */
                                                                /*     0,     0 = 4 bits bus width                  */
                                                                /*     1,     0 = 1 bit  bus width                  */
                                                                /*     1,     1 = 1 bit  bus width                  */


/* ---- SD_INFO1 interrupt mask register ---- */
#define SD_INFO1_MASK_ALL           ((uint64_t)(0x1031d))       /* ALL SD_INFO1 Interrupt                       */
#define SD_INFO1_MASK_STATE_DAT3    ((uint64_t)(0x0400))        /* b10 : SD_D3 State                            */
#define SD_INFO1_MASK_DET_DAT3      ((uint64_t)(0x0300))        /* b8,9: SD_D3 Card Insertion and Removal       */
#define SD_INFO1_MASK_INS_DAT3      ((uint64_t)(0x0200))        /* b9  : SD_D3 Card Insertion                   */
#define SD_INFO1_MASK_REM_DAT3      ((uint64_t)(0x0100))        /* b8  : SD_D3 Card Removal                     */
#define SD_INFO1_MASK_WP            ((uint64_t)(0x0080))        /* b7  : Write Protect                          */
#define SD_INFO1_MASK_STATE_CD      ((uint64_t)(0x0020))        /* b5  : SD_CD State                            */
#define SD_INFO1_MASK_DET_CD        ((uint64_t)(0x0018))        /* b3,4: SD_CD Card Insertion and Removal       */
#define SD_INFO1_MASK_INS_CD        ((uint64_t)(0x0010))        /* b4  : SD_CD Card Insertion                   */
#define SD_INFO1_MASK_REM_CD        ((uint64_t)(0x0008))        /* b3  : SD_CD Card Removal                     */
#define SD_INFO1_MASK_TRNS_RESP     ((uint64_t)(0x0005))        /* b0,2: Access End and Response End            */
#define SD_INFO1_MASK_DATA_TRNS     ((uint64_t)(0x0004))        /* b2  : Access End                             */
#define SD_INFO1_MASK_RESP          ((uint64_t)(0x0001))        /* b0  : Response End                           */
#define SD_INFO1_MASK_DET_DAT3_CD   (SD_INFO1_MASK_DET_DAT3|SD_INFO1_MASK_DET_CD)

/* ---- SD_INFO2 interrupt mask register ---- */
#define SD_INFO2_MASK_ALLP          ((uint64_t)(0x8b7f))    /* All SD_INFO2 Interrupt for OUTPUT(b11 is always 1) */
#define SD_INFO2_MASK_ALL           ((uint64_t)(0x837f))        /* All SD_INFO2 Interrupt                       */
#define SD_INFO2_MASK_BWE           ((uint64_t)(0x827f))        /* Write enable and All errors                  */
#define SD_INFO2_MASK_BRE           ((uint64_t)(0x817f))        /* Read enable and All errors                   */
#define SD_INFO2_MASK_ERR           ((uint64_t)(0x807f))        /* All errors                                   */
#define SD_INFO2_MASK_ILA           ((uint64_t)(0x8000))        /* b15 : Illegal Access Error                   */
#define SD_INFO2_MASK_CBSY          ((uint64_t)(0x4000))        /* b14 : Command Type Register Busy             */
#define SD_INFO2_MASK_SCLKDIVEN     ((uint64_t)(0x2000))        /* b13 : SD Bus Busy                            */
#define SD_INFO2_MASK_WE            ((uint64_t)(0x0200))        /* b9  : SD_BUF Write Enable                    */
#define SD_INFO2_MASK_RE            ((uint64_t)(0x0100))        /* b8  : SD_BUF Read Enable                     */
#define SD_INFO2_MASK_ERR6          ((uint64_t)(0x0040))        /* b6  : Response Timeout                       */
#define SD_INFO2_MASK_ERR5          ((uint64_t)(0x0020))        /* b5  : SD_BUF Illegal Read Access             */
#define SD_INFO2_MASK_ERR4          ((uint64_t)(0x0010))        /* b4  : SD_BUF Illegal Write Access            */
#define SD_INFO2_MASK_ERR3          ((uint64_t)(0x0008))        /* b3  : Data Timeout(except response timeout)  */
#define SD_INFO2_MASK_ERR2          ((uint64_t)(0x0004))        /* b2  : END Error                              */
#define SD_INFO2_MASK_ERR1          ((uint64_t)(0x0002))        /* b1  : CRC Error                              */
#define SD_INFO2_MASK_ERR0          ((uint64_t)(0x0001))        /* b0  : CMD Error                              */

/* ---- DMA mode enable register ---- */
#define CC_EXT_MODE_DMASDRW         ((uint64_t)(0x0002))        /* SD_BUF Read/Write DMA Transfer           */

/* ---- Software reset register ---- */
#define SOFT_RST_SDRST_RESET        ((uint64_t)(0x0006))        /* Software Reset                           */
#define SOFT_RST_SDRST_RELEASED     ((uint64_t)(0x0007))        /* Software Reset Released                  */

/* ---- Host interface mode set register ---- */
#define HOST_MODE_32BIT_ACCESS      ((uint64_t)(0x0101))        /* SD_BUF access size (32bit)               */
#define HOST_MODE_64BIT_ACCESS      ((uint64_t)(0x0000))        /* SD_BUF access size (64bit)               */

/* ---- DMAC mode regiter ---- */
#define DM_CM_DTRAN_MODE_READ       ((uint64_t)(0x10030))       /* SD upstream                              */
#define DM_CM_DTRAN_MODE_WRITE      ((uint64_t)(0x00030))       /* SD downstream                            */
                                                                /* b17,b16 : CH_NUM[1:0]                    */
                                                                /*   0,  0 : SD downstream                  */
                                                                /*   0,  1 : SD upstream                    */
                                                                /*  b5, b4 : BUS_WIDTH[1:0]                 */
                                                                /*   1,  1 : 64bit(fix)                     */

/* ---- DMAC control register ---- */
#define DM_CM_DTRAN_CTRL_DM_START   ((uint64_t)(0x0001))        /* b0 : DM_START                            */

/* ---- DMAC soft reset register ---- */
#define DM_CM_RST_RESET             ((uint64_t)(0xFFFFFCFF))    /* b8,9 = 0,0 : ch0/ch1 soft reset          */
#define DM_CM_RST_RELEASED          ((uint64_t)(0xFFFFFFFF))    /* b8,9 = 1,1 : ch0/ch1 soft reset released */

/* ---- DM_CM_INFO1 interrupt mask register ---- */
#define DM_CM_INFO1_MASK_DTRANEND1  ((uint64_t)(0x00100000))    /* b20 : DMAC ch1 trans end mask            */
#define DM_CM_INFO1_MASK_DTRANEND0  ((uint64_t)(0x00010000))    /* b16 : DMAC ch0 trans end mask            */
#define DM_CM_INFO1_MASK_ALLP       ((uint64_t)(0xFFFFFFFF))    /* All : b1-15, b17-19, b21-31 are always 1 */

/* ---- DM_CM_INFO2 interrupt mask register ---- */
#define DM_CM_INFO2_MASK_DTRANERR1  ((uint64_t)(0x00020000))    /* b17 : DMAC ch1 error mask                */
#define DM_CM_INFO2_MASK_DTRANERR0  ((uint64_t)(0x00010000))    /* b16 : DMAC ch0 error mask                */
#define DM_CM_INFO2_MASK_ALLP       ((uint64_t)(0xFFFFFFFF))    /* All : b1-15, b18-31 are always 1         */

/* ---- time out count ---- */
#define SD_TIMEOUT_CMD              (100)                       /* commnad timeout */
#define SD_TIMEOUT_MULTIPLE         (1000)                      /* block transfer timeout */
#define SD_TIMEOUT_RESP             (1000)                      /* command sequence timeout */
#define SD_TIMEOUT_DMA_END          (1000)                      /* DMA transfer timeout */
#define SD_TIMEOUT_ERASE_CMD        (10000)                     /* erase timeout */
#define SD_TIMEOUT_PROG_CMD         (10000)                     /* programing timeout */

/* ---- data transafer direction ---- */
#define SD_TRANS_READ               (0)                         /* Host <- SD */
#define SD_TRANS_WRITE              (1)                         /* Host -> SD */

/* ---- card register size ---- */
#define STATUS_DATA_BYTE            (64)                        /* STATUS_DATA size */
#define SD_STATUS_BYTE              (64)                        /* SD STATUS size */
#define SD_SCR_REGISTER_BYTE        (8)                         /* SCR register size */

/* ---- area distinction ---- */
#define SD_USER_AREA                (1u)
#define SD_PROT_AREA                (2u)

/* --- SD specification version ---- */
#define SD_SPEC_10                  (0)                         /* SD physical spec 1.01 (phys spec ver1.01) */
#define SD_SPEC_11                  (1)                         /* SD physical spec 1.10 (phys spec ver1.10) */
#define SD_SPEC_20                  (2)                         /* SD physical spec 2.00 (phys spec ver2.00) */
#define SD_SPEC_30                  (3)                         /* SD physical spec 3.00 (phys spec ver3.00) */

/* --- SD specification version register mask --- */
#define SD_SPEC_REGISTER_MASK       (0x0F00u)                   /* SD_SPEC SCR[59:56] mask */
#define SD_SPEC_30_REGISTER         (0x8000u)                   /* SD_SPEC3 SCR[47] is 1   */
#define SD_SPEC_20_REGISTER         (0x0200u)                   /* SD_SPEC SCR[57] is 1    */
#define SD_SPEC_11_REGISTER         (0x0100u)                   /* SD_SPEC SCR[56] is 1    */
/* ==== format parameter ==== */
#define SIZE_CARD_256KB             (256*1024/512)              /*  256*1KB/(sector size) */
#define SIZE_CARD_1MB               (1024*1024/512)             /* 1024*1KB/(sector size) */
#define SIZE_CARD_2MB               (2*1024*1024/512)           /*    2*1MB/(sector size) */
#define SIZE_CARD_4MB               (4*1024*1024/512)           /*    4*1MB/(sector size) */
#define SIZE_CARD_8MB               (8*1024*1024/512)           /*    8*1MB/(sector size) */
#define SIZE_CARD_16MB              (16*1024*1024/512)          /*   16*1MB/(sector size) */
#define SIZE_CARD_32MB              (32*1024*1024/512)          /*   32*1MB/(sector size) */
#define SIZE_CARD_64MB              (64*1024*1024/512)          /*   64*1MB/(sector size) */
#define SIZE_CARD_128MB             (128*1024*1024/512)         /*  128*1MB/(sector size) */
#define SIZE_CARD_256MB             (256*1024*1024/512)         /*  256*1MB/(sector size) */
#define SIZE_CARD_504MB             (504*1024*1024/512)         /*  504*1MB/(sector size) */
#define SIZE_CARD_1008MB            (1008*1024*1024/512)        /* 1008*1MB/(sector size) */
#define SIZE_CARD_1024MB            (1024*1024*1024/512)        /* 1024*1MB/(sector size) */
#define SIZE_CARD_2016MB            (2016*1024*1024/512)        /* 2016*1MB/(sector size) */
#define SIZE_CARD_2048MB            (2048ul*1024ul*1024ul/512ul)/* 2048*1MB/(sector size) */
#define SIZE_CARD_4032MB            (4032ul*1024ul*2ul)         /* 4032*(1MB/sector size) */
#define SIZE_CARD_4096MB            (4096ul*1024ul*2ul)         /* 4096*(1MB/sector size) */
#define SIZE_CARD_8192MB            (8192ul*1024ul*2ul)         /* 2048*(1MB/sector size) */
#define SIZE_CARD_16384MB           (16384ul*1024ul*2ul)        /* 2048*(1MB/sector size) */
#define SIZE_CARD_32768MB           (32768ul*1024ul*2ul)        /* 2048*(1MB/sector size) */
#define SIZE_CARD_128GB             (128ul*1024ul*1024ul*2ul)   /*  128*(1GB/sector size) */
#define SIZE_CARD_512GB             (512ul*1024ul*1024ul*2ul)   /*  512*(1GB/sector size) */
#define SIZE_CARD_2TB               (0xFFFFFFFF)                /*    2*(1TB/sector size) over 32bit max value! */

#define NUM_HEAD_2                  (2)
#define NUM_HEAD_4                  (4)
#define NUM_HEAD_8                  (8)
#define NUM_HEAD_16                 (16)
#define NUM_HEAD_32                 (32)
#define NUM_HEAD_64                 (64)
#define NUM_HEAD_128                (128)
#define NUM_HEAD_255                (255)

#define SEC_PER_TRACK_16            (16)
#define SEC_PER_TRACK_32            (32)
#define SEC_PER_TRACK_63            (63)

#define SEC_PER_CLUSTER_1           (1)
#define SEC_PER_CLUSTER_2           (2)
#define SEC_PER_CLUSTER_8           (8)
#define SEC_PER_CLUSTER_16          (16)
#define SEC_PER_CLUSTER_32          (32)
#define SEC_PER_CLUSTER_64          (64)
#define SEC_PER_CLUSTER_256         (256)
#define SEC_PER_CLUSTER_512         (512)
#define SEC_PER_CLUSTER_1024        (1024)

/* Boundary Unit Size(sectors) */
#define SIZE_OF_BU_1                (1)
#define SIZE_OF_BU_2                (2)
#define SIZE_OF_BU_8                (8)
#define SIZE_OF_BU_16               (16)
#define SIZE_OF_BU_32               (32)
#define SIZE_OF_BU_64               (64)
#define SIZE_OF_BU_128              (128)
#define SIZE_OF_BU_8192             (8192)
#define SIZE_OF_BU_32768            (32768)
#define SIZE_OF_BU_65536            (65536)
#define SIZE_OF_BU_131072           (131072)

#define SD_SECTOR_SIZE              (512)

/* Maximum AU size */
#define SD_ERASE_SECTOR             ((4096*1024)/512)

#define SCLKDIVEN_LOOP_COUNT        (10000)                     /* check SCLKDIVEN bit loop count */

/* ==== macro functions ==== */
#define SD_GET_HNDLS(a)            (gp_sdhandle[(a)])

/* 64bit access */
#define SD_OUTP(h,offset,data)      (((volatile u_sd_reg_t *)((h)->reg_base+(offset)))->longlong = (data))
#define SD_INP(h,offset)            (((volatile u_sd_reg_t *)((h)->reg_base+(offset)))->longlong)

/* 32bit access */
#define SD_OUTPH(h,offset,data)     (((volatile u_sd_reg_t *)((h)->reg_base+(offset)))->st_long_t.h_ = (data))
#define SD_OUTPL(h,offset,data)     (((volatile u_sd_reg_t *)((h)->reg_base+(offset)))->st_long_t.l_ = (data))
#define SD_INPH(h,offset)           (((volatile u_sd_reg_t *)((h)->reg_base+(offset)))->st_long_t.h_)
#define SD_INPL(h,offset)           (((volatile u_sd_reg_t *)((h)->reg_base+(offset)))->st_long_t.l_)

/* 16bit access */
#define SD_OUTPHH(h,offset,data)    (((volatile u_sd_reg_t *)((h)->reg_base+(offset)))->st_word_t.hh_ = (data))
#define SD_OUTPHL(h,offset,data)    (((volatile u_sd_reg_t *)((h)->reg_base+(offset)))->st_word_t.hl_ = (data))
#define SD_OUTPLH(h,offset,data)    (((volatile u_sd_reg_t *)((h)->reg_base+(offset)))->st_word_t.lh_ = (data))
#define SD_OUTPLL(h,offset,data)    (((volatile u_sd_reg_t *)((h)->reg_base+(offset)))->st_word_t.ll_ = (data))
#define SD_INPHH(h,offset)          (((volatile u_sd_reg_t *)((h)->reg_base+(offset)))->st_word_t.hh_)
#define SD_INPHL(h,offset)          (((volatile u_sd_reg_t *)((h)->reg_base+(offset)))->st_word_t.hl_)
#define SD_INPLH(h,offset)          (((volatile u_sd_reg_t *)((h)->reg_base+(offset)))->st_word_t.lh_)
#define SD_INPLL(h,offset)          (((volatile u_sd_reg_t *)((h)->reg_base+(offset)))->st_word_t.ll_)

/* ==== command type ==== */
/* ---- eSD commands ---- */
#define CMD43                       (0x052B)                    /* SELECT_PARTITIONS */
#define CMD44                       (0x0C2C)                    /* MANAGE_PARTITIONS */
#define CMD45                       (0x1C2D)                    /* QUERY_PARTITIONS */

/* ==== SD Card detect ==== */
#define SD_CD_DETECT                (0x0001u)                   /* card detection */

/* === SDR104 READ/WRITE CMD === */
#define SDR104_READ_CMD             (0x7c00u)

/* === SD Set Clock === */
#define SD_CLK_CTRL_SCLKEN          (0x0100uL)

/* ==== SD Driver work buffer (allocated by File system) ==== */
typedef struct __sdhndl                 /* SD handle */
{
    uint32_t    reg_base;               /* SDHI base address */
    uint32_t    card_sector_size;       /* sector size (user area) */
    uint32_t    prot_sector_size;       /* sector size (protect area) */
    uint32_t    erase_sect;             /* erase block size */
    uint8_t     fat_type;               /* FAT type (FAT12:1 FAT16:2 FAT32:3 unknown:0)  */
    uint8_t     csd_structure;          /* CSD structure (Standard capacity:0 High capacity:1) */
    uint8_t     csd_tran_speed;         /* CSD transfer speed */
    uint16_t    csd_ccc;                /* CSD command class */
    uint8_t     csd_copy;               /* CSD copy flag (not used) */
    uint8_t     csd_file_format;        /* CSD file format group */
    uint8_t     sd_spec;                /* SCR spec version */
    uint8_t     if_mode;                /* bus width (1bit:0 4bits:1) */
    uint16_t    speed_mode;             /* card speed mode; */
    uint8_t     speed_class;            /* card speed class */
    uint8_t     perform_move;           /* card move performance */
    uint8_t     media_type;             /* card type */
    uint8_t     write_protect;          /* write protect:       OFF : 0     */

                                        /*                   H/W WP : 1     */
                                        /*   CSD  TMP_WRITE_PROTECT : 2     */
                                        /*   CSD PERM_WRITE_PROTECT : 4     */
                                        /*   SD ROM                 : 0x10  */
    uint64_t    int_info1;              /* SD_INFO1 status */
    uint64_t    int_info2;              /* SD_INFO2 status */
    uint64_t    int_info1_mask;         /* SD_INFO1_MASK status */
    uint64_t    int_info2_mask;         /* SD_INFO2_MASK status */
    uint64_t    int_dm_info1;           /* DM_CM_INFO1 status */
    uint64_t    int_dm_info2;           /* DM_CM_INFO2 status */
    uint64_t    int_dm_info1_mask;      /* DM_CM_INFO1_MASK status */
    uint64_t    int_dm_info2_mask;      /* DM_CM_INFO2_MASK status */
    uint32_t    voltage;                /* system supplied voltage */
    int32_t     error;                  /* error detail information */
    uint16_t    stop;                   /* compulsory stop flag */
    uint8_t     mount;                  /* mount flag (mount:0 unmount:1) */
    uint8_t     int_mode;               /* interrupt flag detect method (polling:0 H/W interrupt:1) */
    uint8_t     trans_mode;             /* data transfer method  PIO : 0    */

                                        /*                SD_BUF DMA : 2    */
    uint8_t     sup_card;               /* support card;                */

                                        /*  Memory (include MMC) : 0    */
                                        /*                    IO : 1    */
    uint16_t    sup_speed;              /* support speed */
    uint8_t     sup_ver;                /* support version (ver1.1:0 ver2.x:1) */
    uint8_t     cd_port;                /* card detect method (CD pin:0 DAT3:1) */
    uint8_t     sd_port;                /* card port number */
    int16_t     trans_sectors;          /* maximum block counts per multiple command */
    int16_t     trans_blocks;           /* maximum block counts per multiple command */
    p_intCallbackFunc int_cd_callback;   /* callback function for card detection */
    p_fmtCallbackFunc int_format_callback;  /* callback function for card format */
    p_intCallbackFunc int_callback;         /* callback function for interrupt flags */
    p_intCallbackFunc int_dma_callback;     /* callback function for interrupt flags */
    uint32_t    resp_status;                            /* R1/R1b response status */
    uint16_t    ocr[4 / sizeof(uint16_t)];              /* OCR value */
    uint16_t    if_cond[4 / sizeof(uint16_t)];          /* IF_COND value */
    uint16_t    cid[16 / sizeof(uint16_t)];             /* CID value */
    uint16_t    csd[16 / sizeof(uint16_t)];             /* CSD value */
    uint16_t    dsr[2 / sizeof(uint16_t)];              /* DSR value */
    uint16_t    rca[4 / sizeof(uint16_t)];              /* RCA value */
    uint16_t    scr[8 / sizeof(uint16_t)];              /* SCR value */
    uint16_t    sdstatus[16 / sizeof(uint16_t)];        /* SD STATUS value */
    uint16_t    status_data[18 / sizeof(uint16_t)];     /* STATUS DATA value (phys spec ver1.10) */
    uint8_t     *p_rw_buff;                             /* work buffer pointer */
    uint32_t    buff_size;                              /* work buffer size */
    int32_t     sup_if_mode;                            /* supported bus width (1bit:0 4bits:1) */
} st_sdhndl_t;

extern st_sdhndl_t *gp_sdhandle[NUM_PORT];

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
/* ==== proto type ==== */
/* ---- sd_init.c ---- */
/* Function Name: _sd_init_hndl */
/**************************************************************************//**
 * @fn            int32_t _sd_init_hndl(st_sdhndl_t *p_hndl, uint32_t mode, uint32_t voltage)
 * @brief         initialize SD handle. <br>
 *                initialize following SD handle members <br>
 *                media_type       : card type <br>
 *                write_protect    : write protect <br>
 *                resp_status      : R1/R1b response status <br>
 *                error            : error detail information <br>
 *                stop             : compulsory stop flag <br>
 *                prot_sector_size : sector size (protect area) <br>
 *                card registers   : ocr, cid, csd, dsr, rca, scr, sdstatus and
 *                status_data
 * @warning       .
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @param [in]    uint32_t mode       : driver mode
 * @param [in]    uint32_t voltage    : working voltage
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t _sd_init_hndl(st_sdhndl_t *p_hndl, uint32_t mode, uint32_t voltage);

/* ---- sd_mount.c ---- */
/* Function Name: _sd_card_init */
/**************************************************************************//**
 * @fn            int32_t _sd_card_init(st_sdhndl_t *p_hndl)
 * @brief         initialize card. <br>
 *                initialize card from idle state to stand-by <br>
 *                distinguish card type (SD, MMC, IO or COMBO) <br>
 *                get CID, RCA, CSD from the card
 * @warning       .
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t _sd_card_init(st_sdhndl_t *p_hndl);

/* Function Name: _sd_mem_mount */
/**************************************************************************//**
 * @fn            int32_t _sd_mem_mount(st_sdhndl_t *p_hndl)
 * @brief         mount memory card. <br>
 *                mount memory part from stand-by to transfer state
 * @warning       .
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t _sd_mem_mount(st_sdhndl_t *p_hndl);

/* Function Name: _sd_card_get_status */
/**************************************************************************//**
 * @fn            int32_t _sd_card_get_status(st_sdhndl_t *p_hndl)
 * @brief         get SD Status (issue ACMD13)
 * @warning       .
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t _sd_card_get_status(st_sdhndl_t *p_hndl);

/* Function Name: _sd_card_get_scr */
/**************************************************************************//**
 * @fn            int32_t _sd_card_get_scr(st_sdhndl_t *p_hndl)
 * @brief         get SCR register (issue ACMD51).
 * @warning       .
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @retval        SD_OK : end of succeed
 *****************************************************************************/
int32_t _sd_card_get_scr(st_sdhndl_t *p_hndl);

/* Function Name: _sd_read_byte */
/**************************************************************************//**
 * @fn            int32_t _sd_read_byte(st_sdhndl_t *p_hndl, uint16_t cmd, uint16_t h_arg, uint16_t l_arg,
 *                      uint8_t *readbuff, uint16_t byte)
 * @brief         read byte data from card <br>
 *                issue byte data read command and read data from SD_BUF
 *                using following commands <br>
 *                SD STATUS(ACMD13),SCR(ACMD51),NUM_WRITE_BLOCK(ACMD22),
 * @warning       transfer type is PIO
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @param [in]    uint16_t cmd        : command code
 * @param [in]    uint16_t h_arg      : command argument high [31 : 16]
 * @param [in]    uint16_t l_arg      : command argument low [15 : 0]
 * @param [out]   uint8_t *readbuff   : read data buffer
 * @param [in]    uint16_t byte       : the number of read bytes
 * @retval        SD_OK : end of succeed
 *****************************************************************************/
int32_t _sd_read_byte(st_sdhndl_t *p_hndl, uint16_t cmd, uint16_t h_arg, uint16_t l_arg,
                        uint8_t *readbuff, uint16_t byte);

/* Function Name: _sd_write_byte */
/**************************************************************************//**
 * @fn            int32_t _sd_write_byte(st_sdhndl_t *p_hndl, uint16_t cmd, uint16_t h_arg, uint16_t l_arg,
 *                      uint8_t *writebuff, uint16_t byte)
 * @brief         write byte data to card <br>
 *                issue byte data write command and write data to SD_BUF
 *                using following commands 
 *                (CMD27 and CMD42)
 * @warning       transfer type is PIO
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @param [in]    uint16_t cmd        : command code
 * @param [in]    uint16_t h_arg      : command argument high [31 : 16]
 * @param [in]    uint16_t l_arg      : command argument low [15 : 0]
 * @param [out]   uint8_t *writebuff  : write data buffer
 * @param [in]    uint16_t byte       : the number of write bytes
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t _sd_write_byte(st_sdhndl_t *p_hndl, uint16_t cmd, uint16_t h_arg, uint16_t l_arg,
                        uint8_t *writebuff, uint16_t byte);

/* Function Name: _sd_calc_erase_sector */
/**************************************************************************//**
 * @fn            int32_t _sd_calc_erase_sector(st_sdhndl_t *p_hndl)
 * @brief         calculate erase sector. <br>
 *                This function calculate erase sector for SD Phy Ver2.0.
 * @warning       transfer type is PIO
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t _sd_calc_erase_sector(st_sdhndl_t *p_hndl);

/* ---- sd_trns.c ---- */
/* Function Name: _sd_software_trans */
/**************************************************************************//**
 * @fn            int32_t _sd_software_trans(st_sdhndl_t *p_hndl, uint8_t *buff, int32_t cnt, int32_t dir)
 * @brief         transfer data by software. <br>
 *                transfer data to/from card by software <br>
 *                this operations are used multiple command data phase <br>
 *                if dir is SD_TRANS_READ, data is from card to host <br>
 *                if dir is SD_TRANS_WRITE, data is from host to card
 * @warning       transfer finished, check CMD12 sequence refer to All end
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @param [out]   uint8_t *buff       : destination/source data buffer
 * @param [in]    int32_t cnt         : number of transfer bytes
 * @param [in]    int32_t dir         : transfer direction
 * @retval        p_hndl->error  : SD handle error value
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t _sd_software_trans(st_sdhndl_t *p_hndl, uint8_t *buff, int32_t cnt, int32_t dir);

/* Function Name: _sd_dma_trans */
/**************************************************************************//**
 * @fn            int32_t _sd_dma_trans(st_sdhndl_t *p_hndl, int32_t cnt)
 * @brief         transfer data by DMA. <br>
 *                transfer data to/from card by DMA <br>
 *                this operations are multiple command data phase
 * @warning       transfer finished, check CMD12 sequence refer to All end
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @param [in]    int32_t cnt         : number of transfer bytes
 * @retval        p_hndl->error  : SD handle error value
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t _sd_dma_trans(st_sdhndl_t *p_hndl, int32_t cnt);

/* ---- sd_read.c ---- */
    /* no function */

/* ---- sd_write.c ---- */
/* Function Name: _sd_write_sect */
/**************************************************************************//**
 * @fn            int32_t _sd_write_sect(st_sdhndl_t *p_hndl, uint8_t *buff, uint32_t psn,
 *                      int32_t cnt, int32_t writemode);
 * @brief         write sector data to card. <br>
 *                write sector data from physical sector number (=psn) by the
 *                number of sectors (=cnt) <br>
 *                if SD Driver mode is SD_MODE_SW, data transfer by
 *                sddev_read_data function <br>
 *                if SD Driver mode is SD_MODE_DMA, data transfer by DMAC
 * @warning       .
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @param [out]   uint8_t *buff       : write data buffer
 * @param [in]    uint32_t psn        : write physical sector number
 * @param [in]    int32_t cnt         : number of write sectors
 * @param [in]    int32_t writemode   : memory card write mode, <br>
 *                  SD_WRITE_WITH_PREERASE : pre-erease write <br>
 *                  SD_WRITE_OVERWRITE     : overwrite
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t _sd_write_sect(st_sdhndl_t *p_hndl, uint8_t *buff, uint32_t psn,
                        int32_t cnt, int32_t writemode);

/* ---- sd_cd.c ---- */
/* Function Name: _sd_check_media */
/**************************************************************************//**
 * @fn            int32_t _sd_check_media(st_sdhndl_t *p_hndl)
 * @brief         check card insertion <br>
 *                if card is inserted, return SD_OK <br>
 *                if card is not inserted, return SD_ERR <br>
 * @warning       .
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @retval        SD_OK  : card is inserted
 * @retval        SD_ERR : card is not inserted
 *****************************************************************************/
int32_t _sd_check_media(st_sdhndl_t *p_hndl);

/* ---- sd_cmd.c ---- */
/* Function Name: _sd_send_cmd */
/**************************************************************************//**
 * @fn            int32_t _sd_send_cmd(st_sdhndl_t *p_hndl, uint16_t cmd)
 * @brief         issue SD command, hearafter wait recive response
 * @warning       not get response and check response errors
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @param [in]    uint16_t cmd        : command code
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t _sd_send_cmd(st_sdhndl_t *p_hndl, uint16_t cmd);

/* Function Name: _sd_send_acmd */
/**************************************************************************//**
 * @fn            int32_t _sd_send_acmd(st_sdhndl_t *p_hndl, uint16_t cmd, uint16_t h_arg, uint16_t l_arg)
 * @brief         issue application specific command, hearafter wait recive
 *                response <br>
 *                issue CMD55 preceide application specific command
 * @warning       .
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @param [in]    uint16_t cmd        : command code
 * @param [in]    uint16_t h_arg      : command argument high [31 : 16]
 * @param [in]    uint16_t l_arg      : command argument low [15 : 0]
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t _sd_send_acmd(st_sdhndl_t *p_hndl, uint16_t cmd, uint16_t h_arg, uint16_t l_arg);

/* Function Name: _sd_send_mcmd */
/**************************************************************************//**
 * @fn            int32_t _sd_send_mcmd(st_sdhndl_t *p_hndl, uint16_t cmd, uint32_t startaddr)
 * @brief         issue multiple command (CMD18 or CMD25) <br>
 *                wait response <br>
 *                set read start address to startaddr <br>
 *                after this function finished, start data transfer
 * @warning       .
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @param [in]    uint16_t cmd        : command code (CMD18 or CMD25)
 * @param [in]    uint32_t startaddr  : data address (command argument)
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t _sd_send_mcmd(st_sdhndl_t *p_hndl, uint16_t cmd, uint32_t startaddr);

/* Function Name: _sd_card_send_cmd_arg */
/**************************************************************************//**
 * @fn            int32_t _sd_card_send_cmd_arg(st_sdhndl_t *p_hndl, uint16_t cmd,
 *                      int32_t resp, uint16_t h_arg, uint16_t l_arg)
 * @brief         issue general SD command. <br>
 *                issue command specified cmd code <br>
 *                get and check response
 * @warning       .
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @param [in]    uint16_t cmd        : command code (CMD18 or CMD25)
 * @param [in]    int32_t  resp       : command response
 * @param [in]    uint16_t h_arg      : command argument high [31 : 16]
 * @param [in]    uint16_t l_arg      : command argument low [15 : 0]
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t _sd_card_send_cmd_arg(st_sdhndl_t *p_hndl, uint16_t cmd, int32_t resp, uint16_t h_arg, uint16_t l_arg);

/* Function Name: _sd_set_arg */
/**************************************************************************//**
 * @fn            void _sd_set_arg(st_sdhndl_t *p_hndl, uint16_t h_arg, uint16_t l_arg)
 * @brief         set command argument to SDHI <br>
 *                h_arg means higher 16bits [31 : 16] and  set SD_ARG0 <br>
 *                l_arg means lower 16bits [15 : 0] and set SD_ARG1
 * @warning       SD_ARG0 and SD_ARG1 are like little endian order
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @param [in]    uint16_t h_arg      : command argument high [31 : 16]
 * @param [in]    uint16_t l_arg      : command argument low [15 : 0]
 * @retval        none
 *****************************************************************************/
void _sd_set_arg(st_sdhndl_t *p_hndl, uint16_t h_arg, uint16_t l_arg);

/* Function Name: _sd_card_send_ocr */
/**************************************************************************//**
 * @fn            int32_t _sd_card_send_ocr(st_sdhndl_t *p_hndl, int32_t type)
 * @brief         get OCR register and check card operation voltage <br>
 *                if type is SD_MEDIA_SD, issue ACMD41 <br>
 *                if type is SD_MEDIA_MMC, issue CMD1
 * @warning       .
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @param [in]    int32_t type        : card type
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t _sd_card_send_ocr(st_sdhndl_t *p_hndl, int32_t type);

/* Function Name: _sd_check_resp_error */
/**************************************************************************//**
 * @fn            int32_t _sd_check_resp_error(st_sdhndl_t *p_hndl)
 * @brief         distinguish error bit from R1 response <br>
 *                set the error bit to p_hndl->error
 * @warning       .
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @retval        SD_OK  : no error detected
 * @retval        SD_ERR : any errors detected
 *****************************************************************************/
int32_t _sd_check_resp_error(st_sdhndl_t *p_hndl);

/* Function Name: _sd_get_resp */
/**************************************************************************//**
 * @fn            int32_t _sd_get_resp(st_sdhndl_t *p_hndl, int32_t resp)
 * @brief         get response and check response errors. <br>
 *                get response value from RESP register <br>
 *                R1, R2, R3,(R4, R5) and R6 types are available
 *                specify response type by the argument resp <br>
 *                set response value to SD handle member
 * @warning       .
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @param [in]    int32_t resp        : response type
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t _sd_get_resp(st_sdhndl_t *p_hndl, int32_t resp);

/* Function Name: _sd_check_csd */
/**************************************************************************//**
 * @fn            int32_t _sd_check_csd(st_sdhndl_t *p_hndl)
 * @brief         check CSD register and get following information <br>
 *                Transfer Speed <br>
 *                Command Class <br>
 *                Read Block Length <br>
 *                Copy Bit <br>
 *                Write Protect Bit <br>
 *                File Format Group <br>
 *                Number of Erase Sector
 * @warning       .
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t _sd_check_csd(st_sdhndl_t *p_hndl);

/* Function Name: _sd_check_info2_err */
/**************************************************************************//**
 * @fn            int32_t _sd_check_info2_err(st_sdhndl_t *p_hndl)
 * @brief         check SD_INFO2 register errors. <br>
 *                check error bit of SD_INFO2 register <br>
 *                set the error bit to p_hndl->error
 * @warning       .
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @retval        SD_OK : end of succeed
 *****************************************************************************/
int32_t _sd_check_info2_err(st_sdhndl_t *p_hndl);

/* ---- sd_int.c ---- */
/* Function Name: _sd_set_int_mask */
/**************************************************************************//**
 * @fn            int32_t _sd_set_int_mask(st_sdhndl_t *p_hndl, uint64_t mask1, uint64_t mask2)
 * @brief         set SD_INFO1 and SD_INFO2 interrupt mask. <br>
 *                set int_info1_mask and int_info2_mask depend on the mask bits
 *                value <br>
 *                if mask bit is one, it is enabled <br>
 *                if mask bit is zero, it is disabled
 * @warning       .
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @param [in]    uint64_t mask1      : SD_INFO1_MASK1 bits value
 * @param [in]    uint64_t mask2      : SD_INFO1_MASK2 bits value
 * @retval        SD_OK : end of succeed
 *****************************************************************************/
int32_t _sd_set_int_mask(st_sdhndl_t *p_hndl, uint64_t mask1, uint64_t mask2);

/* Function Name: _sd_clear_int_mask */
/**************************************************************************//**
 * @fn            int32_t _sd_clear_int_mask(st_sdhndl_t *p_hndl, uint64_t mask1, uint64_t mask2)
 * @brief         clear SD_INFO1 and SD_INFO2 interrupt mask. <br>
 *                clear int_cc_status_mask depend on the mask bits value <br>
 *                if mask bit is one, it is disabled <br>
 *                if mask bit is zero, it is enabled
 * @warning       .
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @param [in]    uint64_t mask1      : SD_INFO1_MASK1 bits value
 * @param [in]    uint64_t mask2      : SD_INFO1_MASK2 bits value
 * @retval        SD_OK : end of succeed
 *****************************************************************************/
int32_t _sd_clear_int_mask(st_sdhndl_t *p_hndl, uint64_t mask1, uint64_t mask2);

/* Function Name: _sd_get_int */
/**************************************************************************//**
 * @fn            int32_t _sd_get_int(st_sdhndl_t *p_hndl)
 * @brief         get SD_INFO1 and SD_INFO2 interrupt elements. <br>
 *                get SD_INFO1 and SD_INFO2 bits <br>
 *                examine enabled elements <br>
 *                hearafter, clear SD_INFO1 and SD_INFO2 bits <br>
 *                save those bits to int_info1 or int_info2
 * @warning       .
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t _sd_get_int(st_sdhndl_t *p_hndl);

/* Function Name: _sd_clear_info */
/**************************************************************************//**
 * @fn            int32_t _sd_clear_info(st_sdhndl_t *p_hndl, uint64_t clear_info1, uint64_t clear_info2)
 * @brief         clear int_info bits. <br>
 *                clear int_info1 and int_info2 depend on the clear value
 * @warning       SD_INFO1 and SD_INFO2 bits are not cleared
 * @param [out]   st_sdhndl_t *p_hndl  : SD handle
 * @param [in]    uint64_t clear_info1 : int_info1 clear bits value
 * @param [in]    uint64_t clear_info2 : int_info2 clear bits value
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t _sd_clear_info(st_sdhndl_t *p_hndl, uint64_t clear_info1, uint64_t clear_info2);

/* Function Name: _sd_set_int_dm_mask */
/**************************************************************************//**
 * @fn            int32_t _sd_set_int_dm_mask(st_sdhndl_t *p_hndl, uint64_t mask1, uint64_t mask2)
 * @brief         set DM_CM_INFO1_MASK and DM_CM_INFO2_MASK interrupt mask. <br>
 *                set int_dm_info1_mask and int_dm_info2_mask depend on the mask bits
 *                value <br>
 *                if mask bit is one, it is enabled <br>
 *                if mask bit is zero, it is disabled <br>
 * @warning       .
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @param [in]    uint64_t mask1      : DM_CM_INFO1_MASK bits value
 * @param [in]    uint64_t mask2      : DM_CM_INFO2_MASK bits value
 * @retval        SD_OK : end of succeed
 *****************************************************************************/
int32_t _sd_set_int_dm_mask(st_sdhndl_t *p_hndl, uint64_t mask1, uint64_t mask2);

/* Function Name: _sd_clear_int_dm_mask */
/**************************************************************************//**
 * @fn            int32_t _sd_clear_int_dm_mask(st_sdhndl_t *p_hndl, uint64_t mask1, uint64_t mask2)
 * @brief         clear DM_CM_INFO1_MASK and DM_CM_INFO2_MASK interrupt mask. <br>
 *                clear int_dm_info1_mask and int_dm_info2_mask depend on the mask bits value <br>
 *                if mask bit is one, it is disabled <br>
 *                if mask bit is zero, it is enabled <br>
 * @warning       .
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @param [in]    uint64_t mask1      : DM_CM_INFO1_MASK bits value
 * @param [in]    uint64_t mask2      : DM_CM_INFO2_MASK bits value
 * @retval        SD_OK : end of succeed
 *****************************************************************************/
int32_t _sd_clear_int_dm_mask(st_sdhndl_t *p_hndl, uint64_t mask1, uint64_t mask2);

/* Function Name: _sd_get_int_dm */
/**************************************************************************//**
 * @fn            int32_t _sd_get_int_dm(st_sdhndl_t *p_hndl);
 * @brief         get DM_CM_INFO1 and DM_CM_INFO2 interrupt elements. <br>
 *                get DM_CM_INFO1 and DM_CM_INFO2 bits <br>
 *                examine enabled elements <br>
 *                hearafter, clear DM_CM_INFO1 and DM_CM_INFO2 bits <br>
 *                save those bits to int_dm_info1 or int_dm_info2
 * @warning       .
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @retval        SD_OK  : any interrupt occured
 * @retval        SD_ERR : no interrupt occured
 *****************************************************************************/
int32_t _sd_get_int_dm(st_sdhndl_t *p_hndl);

/* Function Name: _sd_clear_dm_info */
/**************************************************************************//**
 * @fn            int32_t _sd_clear_dm_info(st_sdhndl_t *p_hndl, uint64_t clear_info1, uint64_t clear_info2)
 * @brief         clear int_dm_info bits. <br>
 *                clear int_dm_info1 and int_dm_info2 depend on the clear value
 * @warning       DM_CM_INFO1 and DM_CM_INFO2 bits are not cleared
 * @param [out]   st_sdhndl_t *p_hndl  : SD handle
 * @param [in]    uint64_t clear_info1 : int_dm_info1 clear bits value
 * @param [in]    uint64_t clear_info2 : int_dm_info2 clear bits value
 * @retval        SD_OK : end of succeed
 *****************************************************************************/
int32_t _sd_clear_dm_info(st_sdhndl_t *p_hndl, uint64_t clear_info1, uint64_t clear_info2);

/* ---- sd_util.c ---- */
/* Function Name: _sd_set_clock */
/**************************************************************************//**
 * @fn            int32_t _sd_set_clock(st_sdhndl_t *p_hndl, int32_t clock, int32_t enable)
 * @brief         control SD clock. <br>
 *                supply or halt SD clock <br>
 *                if enable is SD_CLOCK_ENABLE, supply SD clock <br>
 *                if enable is SD_CLOCK_DISKABLE, halt SD clock <br>
 * @warning       .
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @param [in]    int32_t clock       : SD clock frequency
 * @param [in]    int32_t enable      : supply or halt SD clock
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t _sd_set_clock(st_sdhndl_t *p_hndl, int32_t clock, int32_t enable);

/* Function Name: _sd_set_port */
/**************************************************************************//**
 * @fn            int32_t _sd_set_port(st_sdhndl_t *p_hndl, int32_t port)
 * @brief         control data bus width. <br>
 *                change data bus width <br>
 *                if port is SD_PORT_SERIAL, set data bus width 1bit <br>
 *                if port is SD_PORT_PARALEL, set data bus width 4bits <br>
 *                change between 1bit and 4bits by ACMD6
 * @warning       before execute this function, check card supporting bus
 *                width <br>
 *                SD memory card is 4bits support mandatory
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @param [in]    int32_t port        : setting bus with
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t _sd_set_port(st_sdhndl_t *p_hndl, int32_t port);

/* Function Name: _sd_iswp */
/**************************************************************************//**
 * @fn            int32_t _sd_iswp(st_sdhndl_t *p_hndl)
 * @brief         check hardware write protect refer to SDHI register <br>
 *                if WP pin is disconnected to SDHI, return value has no
 *                meaning
 * @warning       don't check CSD write protect bits and ROM card
 * @param [in]    st_sdhndl_t *p_hndl : SD handle
 * @retval        SD_WP_OFF (0) : not write protected
 * @retval        SD_WP_HW  (1) : write protected
 *****************************************************************************/
int32_t _sd_iswp(st_sdhndl_t *p_hndl);

/* Function Name: _sd_set_err */
/**************************************************************************//**
 * @fn            int32_t _sd_set_err(st_sdhndl_t *p_hndl, int32_t error)
 * @brief         set errors information. <br>
 *                set error information (=error) to SD Handle member
 *                (=p_hndl->error)
 * @warning       if p_hndl->error was already set, no overwrite it
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @param [in]    int32_t error       : setting error information
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t _sd_set_err(st_sdhndl_t *p_hndl, int32_t error);

/* Function Name: _sd_bit_search */
/**************************************************************************//**
 * @fn            int32_t _sd_bit_search(uint16_t data)
 * @brief         get bit information. <br>
 *                check every bits of argument (data) from LSB <br>
 *                return first bit whose value is 1'b <br>
 *                bit number is big endian (MSB is 0)
 * @warning       just 16bits value can be applied
 * @param [in]    uint16_t data : checked data
 * @retval        not less than 0 : bit number has 1'b
 * @retval        -1 : no bit has 1'b
 *****************************************************************************/
int32_t _sd_bit_search(uint16_t data);

/* Function Name: _sd_get_size */
/**************************************************************************//**
 * @fn            int32_t _sd_get_size(st_sdhndl_t *p_hndl, uint32_t area)
 * @brief         get card size. <br>
 *                get memory card size
 * @warning       protect area is just the number of all sectors
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @param [in]    int32_t area        : memory area (bit0 : user area, bit1 : protect area)
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t _sd_get_size(st_sdhndl_t *p_hndl, uint32_t area);

/* Function Name: _sd_standby */
/**************************************************************************//**
 * @fn            int32_t _sd_standby(st_sdhndl_t *p_hndl)
 * @brief         transfer card to stand-by state. <br>
 *                transfer card from transfer state to stand-by state
 * @warning       .
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t _sd_standby(st_sdhndl_t *p_hndl);

/* Function Name: _sd_active */
/**************************************************************************//**
 * @fn            int32_t _sd_active(st_sdhndl_t *p_hndl)
 * @brief         transfer card to transfer state. <br>
 *                transfer card from stand-by state to transfer state
 * @warning       .
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t _sd_active(st_sdhndl_t *p_hndl);

/* Function Name: _sd_inactive */
/**************************************************************************//**
 * @fn            int32_t _sd_inactive(st_sdhndl_t *p_hndl)
 * @brief         transfer card to inactive state. <br>
 *                transfer card from any state to inactive state
 * @warning       .
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t _sd_inactive(st_sdhndl_t *p_hndl);

/* Function Name: _sd_memset */
/**************************************************************************//**
 * @fn            int32_t _sd_memset(uint8_t *p, uint8_t data, uint32_t cnt)
 * @brief         set memory. <br>
 *                fill memory filling data(=data) from start address(=p)
 *                by filling size(=cnt)
 * @warning       .
 * @param [out]   uint8_t *p   : start address of memory
 * @param [in]    uint8_t data : filling data
 * @param [in]    uint32_t cnt : filling size
 * @retval        0 : end of succeed
 *****************************************************************************/
int32_t _sd_memset(uint8_t *p, uint8_t data, uint32_t cnt);

/* Function Name: _sd_memcpy */
/**************************************************************************//**
 * @fn            int32_t _sd_memcpy(uint8_t *dst, uint8_t *src, uint32_t cnt)
 * @brief         copy memory. <br>
 *                copy data from source address(=src) to destination address
 *                (=dst) by copy size(=cnt)
 * @warning       .
 * @param [out]   uint8_t *dst : destination address
 * @param [in]    uint8_t *src : source address
 * @param [in]    uint32_t cnt : copy size
 * @retval        0 : end of succeed
 *****************************************************************************/
int32_t _sd_memcpy(uint8_t *dst, uint8_t *src, uint32_t cnt);

/* Function Name: _sd_rand */
/**************************************************************************//**
 * @fn            uint16_t _sd_rand(void)
 * @brief         create Volume ID Number. <br>
 *                get Volume ID Number <br>
 *                Volume ID Number is created by pseudo random number
 * @warning       .
 * @param [in]    none
 * @retval        created Volume ID Number
 *****************************************************************************/
uint16_t _sd_rand(void);

/* Function Name: _sd_srand */
/**************************************************************************//**
 * @fn            void    _sd_srand(uint32_t seed)
 * @brief         set initial value of Volume ID Number.
 * @warning       .
 * @param [in]    uint32_t seed : initial seting value
 * @retval        none
 *****************************************************************************/
void    _sd_srand(uint32_t seed);

/* Function Name: _sd_wait_rbusy */
/**************************************************************************//**
 * @fn            int32_t _sd_wait_rbusy(st_sdhndl_t *p_hndl, int32_t time)
 * @brief         wait response busy. <br>
 *                wait response busy finished
 * @warning       .
 * @param [out]   st_sdhndl_t *p_hndl : SD handle
 * @param [in]    int32_t time        : response busy wait interval
 * @retval        SD_OK  : response busy finished
 * @retval        SD_ERR : response busy not finished
 *****************************************************************************/
int32_t _sd_wait_rbusy(st_sdhndl_t *p_hndl, int32_t time);

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

#endif  /* SD_H   */

/* End of File */
