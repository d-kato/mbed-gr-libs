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
* File Name    : sd.h
* Version      : 1.00
* Device(s)    : RZ/A2M
* Tool-Chain   : e2 studio (GCC ARM Embedded)
* OS           : None
* H/W Platform : RSK board
* Description  : SD Driver header file
* Operation    : 
* Limitations  : None
******************************************************************************/
/*****************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 16.03.2018 1.00     First Release
*         : 14.12.2018 1.01     Changed the DMAC soft reset procedure.
*         : 28.12.2018 1.02     Support for OS
******************************************************************************/
#ifndef _SD_H_
#define _SD_H_

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "sys_sel.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/
/* ==== option ==== */
#define SD_UNMOUNT_CARD             (0x00u)
#define SD_MOUNT_UNLOCKED_CARD      (0x01u)
#define SD_MOUNT_LOCKED_CARD        (0x02u)
#define SD_CARD_LOCKED              (0x04u)

/* ==== SDHI register address ==== */
#define SD_CMD                      ((0x0000u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)        /* Command Type                                 */
#define SD_ARG                      ((0x0010u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)        /* SD Command Argument CF39-CF8(32bit)          */
#define SD_ARG1                     ((0x0018u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)        /* SD Command Argument CF39-CF24(high 16bits)   */
#define SD_STOP                     ((0x0020u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)        /* Data Stop                                    */
#define SD_SECCNT                   ((0x0028u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)        /* Block Count                                  */
#define SD_RSP10                    ((0x0030u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)        /* SD Card Response R71-8(64bit)                */
#define SD_RSP1                     ((0x0038u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)        /* SD Card Response R39-24                      */
#define SD_RSP32                    ((0x0040u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)        /* SD Card Response R71-40(32bit)               */
#define SD_RSP3                     ((0x0048u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)        /* SD Card Response R71-56                      */
#define SD_RSP54                    ((0x0050u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)        /* SD Card Response R127-72(56bit)              */
#define SD_RSP5                     ((0x0058u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)        /* SD Card Response R103-88                     */
#define SD_RSP76                    ((0x0060u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)        /* SD Card Response R127-104(24bit)             */
#define SD_RSP7                     ((0x0068u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)        /* SD Card Response R127-120                    */
#define SD_INFO1                    ((0x0070u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)        /* SD Card Interrupt Flag(1)                    */
#define SD_INFO2                    ((0x0078u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)        /* SD Card Interrupt Flag(2)                    */
#define SD_INFO1_MASK               ((0x0080u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)        /* SD_INFO1 Interrupt Mask                      */
#define SD_INFO2_MASK               ((0x0088u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)        /* SD_INFO2 Interrupt Mask                      */
#define SD_CLK_CTRL                 ((0x0090u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)        /* SD Clock Control                             */
#define SD_SIZE                     ((0x0098u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)        /* Transfer Data Length                         */
#define SD_OPTION                   ((0x00A0u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)        /* SD Card Access Control Option                */
#define SD_ERR_STS1                 ((0x00B0u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)        /* SD Error Status 1                            */
#define SD_ERR_STS2                 ((0x00B8u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)        /* SD Error Status 2                            */
#define SD_BUF0                     ((0x00C0u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)        /* SD Buffer Read/Write                         */
#define CC_EXT_MODE                 ((0x0360u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)        /* DMA Mode Enable                              */
#define SOFT_RST                    ((0x0380u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)        /* Soft Reset                                   */
#define VERSION                     ((0x0388u<<SD_REG_SHIFT)+SD_BYTE_OFFSET)        /* Version                                      */

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
#define ARG_ACMD6_1bit              (0)
#define ARG_ACMD6_4bit              (2)

/* ---- response type  ---- */
#define SD_RSP_NON                  (0)                         /* no response */
#define SD_RSP_R1                   (1)                         /* nomal response */
#define SD_RSP_R1b                  (2)                         /* nomal response with an optional busy signal */
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
#define TRANS_SECTORS               (hndl->trans_sectors)       /* max 65535 blocks */
#define TRANS_BLOCKS                (hndl->trans_blocks)        /* max 65535 blocks */

/* ---- set block address, if HC card ---- */
#define SET_ACC_ADDR                ((hndl->csd_structure == 0x01) ? (psn) : (psn*512))

/* ---- SD clock control ---- */
#define SD_CLOCK_ENABLE             (1)                         /* supply clock */
#define SD_CLOCK_DISABLE            (0)                         /* halt clock */

/* ---- SD_OPTION register ---- */
#define SD_OPTION_INIT              (uint64_t)(0x00ee)          /* SD_OPTION initial value */
                                                                /* b7-4 : timeout counter = SD_CLK x 2^27           */
                                                                /* b3-0 : card detect timer counter = SD_CLK x 2^24 */
#define SD_OPTION_TOP_MAX           (uint64_t)(0x02e0)          /* SD_OPTION TOP value is max.                      */
#define SD_OPTION_TOP_MASK          (uint64_t)(0x02f0)          /* SD_OPTION TOP mask                               */
#define SD_OPTION_WIDTH_MASK        (SD_OPTION_WIDTH|SD_OPTION_WIDTH8)
                                                                /* SD_OPTION Bus width mask                         */
#define SD_OPTION_WIDTH             (uint64_t)(0x8000)          /* b15 : Bus width(WIDTH)                           */
#define SD_OPTION_WIDTH8            (uint64_t)(0x2000)          /* b13 : Bus width(WIDTH8)                          */
                                                                /* WIDTH,WIDTH8                                     */
                                                                /*     0,     1 = 8 bits bus width                  */
                                                                /*     0,     0 = 4 bits bus width                  */
                                                                /*     1,     0 = 1 bit  bus width                  */
                                                                /*     1,     1 = 1 bit  bus width                  */


/* ---- SD_INFO1 interrupt mask register ---- */
#define SD_INFO1_MASK_ALL           (uint64_t)(0x1031d)         /* ALL SD_INFO1 Interrupt                       */
#define SD_INFO1_MASK_STATE_DAT3    (uint64_t)(0x0400)          /* b10 : SD_D3 State                            */
#define SD_INFO1_MASK_DET_DAT3      (uint64_t)(0x0300)          /* b8,9: SD_D3 Card Insertion and Removal       */
#define SD_INFO1_MASK_INS_DAT3      (uint64_t)(0x0200)          /* b9  : SD_D3 Card Insertion                   */
#define SD_INFO1_MASK_REM_DAT3      (uint64_t)(0x0100)          /* b8  : SD_D3 Card Removal                     */
#define SD_INFO1_MASK_WP            (uint64_t)(0x0080)          /* b7  : Write Protect                          */
#define SD_INFO1_MASK_STATE_CD      (uint64_t)(0x0020)          /* b5  : SD_CD State                            */
#define SD_INFO1_MASK_DET_CD        (uint64_t)(0x0018)          /* b3,4: SD_CD Card Insertion and Removal       */
#define SD_INFO1_MASK_INS_CD        (uint64_t)(0x0010)          /* b4  : SD_CD Card Insertion                   */
#define SD_INFO1_MASK_REM_CD        (uint64_t)(0x0008)          /* b3  : SD_CD Card Removal                     */
#define SD_INFO1_MASK_TRNS_RESP     (uint64_t)(0x0005)          /* b0,2: Access End and Response End            */
#define SD_INFO1_MASK_DATA_TRNS     (uint64_t)(0x0004)          /* b2  : Access End                             */
#define SD_INFO1_MASK_RESP          (uint64_t)(0x0001)          /* b0  : Response End                           */
#define SD_INFO1_MASK_DET_DAT3_CD   (SD_INFO1_MASK_DET_DAT3|SD_INFO1_MASK_DET_CD)

/* ---- SD_INFO2 interrupt mask register ---- */
#define SD_INFO2_MASK_ALLP          (uint64_t)(0x8b7f)          /* All SD_INFO2 Interrupt for OUTPUT(b11 is always 1) */
#define SD_INFO2_MASK_ALL           (uint64_t)(0x837f)          /* All SD_INFO2 Interrupt                       */
#define SD_INFO2_MASK_BWE           (uint64_t)(0x827f)          /* Write enable and All errors                  */
#define SD_INFO2_MASK_BRE           (uint64_t)(0x817f)          /* Read enable and All errors                   */
#define SD_INFO2_MASK_ERR           (uint64_t)(0x807f)          /* All errors                                   */
#define SD_INFO2_MASK_ILA           (uint64_t)(0x8000)          /* b15 : Illegal Access Error                   */
#define SD_INFO2_MASK_CBSY          (uint64_t)(0x4000)          /* b14 : Command Type Register Busy             */
#define SD_INFO2_MASK_SCLKDIVEN     (uint64_t)(0x2000)          /* b13 : SD Bus Busy                            */
#define SD_INFO2_MASK_WE            (uint64_t)(0x0200)          /* b9  : SD_BUF Write Enable                    */
#define SD_INFO2_MASK_RE            (uint64_t)(0x0100)          /* b8  : SD_BUF Read Enable                     */
#define SD_INFO2_MASK_ERR6          (uint64_t)(0x0040)          /* b6  : Response Timeout                       */
#define SD_INFO2_MASK_ERR5          (uint64_t)(0x0020)          /* b5  : SD_BUF Illegal Read Access             */
#define SD_INFO2_MASK_ERR4          (uint64_t)(0x0010)          /* b4  : SD_BUF Illegal Write Access            */
#define SD_INFO2_MASK_ERR3          (uint64_t)(0x0008)          /* b3  : Data Timeout(except response timeout)  */
#define SD_INFO2_MASK_ERR2          (uint64_t)(0x0004)          /* b2  : END Error                              */
#define SD_INFO2_MASK_ERR1          (uint64_t)(0x0002)          /* b1  : CRC Error                              */
#define SD_INFO2_MASK_ERR0          (uint64_t)(0x0001)          /* b0  : CMD Error                              */

/* ---- DMA mode enable register ---- */
#define CC_EXT_MODE_DMASDRW         (uint64_t)(0x0002)          /* SD_BUF Read/Write DMA Transfer           */

/* ---- Software reset register ---- */
#define SOFT_RST_SDRST_RESET        (uint64_t)(0x0006)          /* Software Reset                           */
#define SOFT_RST_SDRST_RELEASED     (uint64_t)(0x0007)          /* Software Reset Released                  */

/* ---- Host interface mode set register ---- */
#define HOST_MODE_32BIT_ACCESS      (uint64_t)(0x0101)          /* SD_BUF access size (32bit)               */
#define HOST_MODE_64BIT_ACCESS      (uint64_t)(0x0000)          /* SD_BUF access size (64bit)               */

/* ---- DMAC mode regiter ---- */
#define DM_CM_DTRAN_MODE_READ       (uint64_t)(0x10030)         /* SD upstream                              */
#define DM_CM_DTRAN_MODE_WRITE      (uint64_t)(0x00030)         /* SD downstream                            */
                                                                /* b17,b16 : CH_NUM[1:0]                    */
                                                                /*   0,  0 : SD downstream                  */
                                                                /*   0,  1 : SD upstream                    */
                                                                /*  b5, b4 : BUS_WIDTH[1:0]                 */
                                                                /*   1,  1 : 64bit(fix)                     */

/* ---- DMAC control register ---- */
#define DM_CM_DTRAN_CTRL_DM_START   (uint64_t)(0x0001)          /* b0 : DM_START                            */

/* ---- DMAC soft reset register ---- */
#define DM_CM_RST_RESET             (uint64_t)(0xFFFFFCFF)      /* b8,9 = 0,0 : ch0/ch1 soft reset          */
#define DM_CM_RST_RELEASED          (uint64_t)(0xFFFFFFFF)      /* b8,9 = 1,1 : ch0/ch1 soft reset released */

/* ---- DM_CM_INFO1 interrupt mask register ---- */
#define DM_CM_INFO1_MASK_DTRANEND1  (uint64_t)(0x00100000)      /* b20 : DMAC ch1 trans end mask            */
#define DM_CM_INFO1_MASK_DTRANEND0  (uint64_t)(0x00010000)      /* b16 : DMAC ch0 trans end mask            */
#define DM_CM_INFO1_MASK_ALLP       (uint64_t)(0xFFFFFFFF)      /* All : b1-15, b17-19, b21-31 are always 1 */

/* ---- DM_CM_INFO2 interrupt mask register ---- */
#define DM_CM_INFO2_MASK_DTRANERR1  (uint64_t)(0x00020000)      /* b17 : DMAC ch1 error mask                */
#define DM_CM_INFO2_MASK_DTRANERR0  (uint64_t)(0x00010000)      /* b16 : DMAC ch0 error mask                */
#define DM_CM_INFO2_MASK_ALLP       (uint64_t)(0xFFFFFFFF)      /* All : b1-15, b18-31 are always 1         */

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
#define _sd_get_hndls(a)            SDHandle[a]

typedef union {
    uint64_t        LONGLONG;
    struct {
        uint32_t    L;
        uint32_t    H;
    } LONG;
    struct {
        uint16_t    LL;
        uint16_t    LH;
        uint16_t    HL;
        uint16_t    HH;
    } WORD;
} SD_REG;

/* 64bit access */
#define SD_OUTP(h,offset,data)      (((volatile SD_REG *)((h)->reg_base+(offset)))->LONGLONG = (data))
#define SD_INP(h,offset)            (((volatile SD_REG *)((h)->reg_base+(offset)))->LONGLONG)

/* 32bit access */
#define SD_OUTPH(h,offset,data)     (((volatile SD_REG *)((h)->reg_base+(offset)))->LONG.H = (data))
#define SD_OUTPL(h,offset,data)     (((volatile SD_REG *)((h)->reg_base+(offset)))->LONG.L = (data))
#define SD_INPH(h,offset)           (((volatile SD_REG *)((h)->reg_base+(offset)))->LONG.H)
#define SD_INPL(h,offset)           (((volatile SD_REG *)((h)->reg_base+(offset)))->LONG.L)

/* 16bit access */
#define SD_OUTPHH(h,offset,data)    (((volatile SD_REG *)((h)->reg_base+(offset)))->WORD.HH = (data))
#define SD_OUTPHL(h,offset,data)    (((volatile SD_REG *)((h)->reg_base+(offset)))->WORD.HL = (data))
#define SD_OUTPLH(h,offset,data)    (((volatile SD_REG *)((h)->reg_base+(offset)))->WORD.LH = (data))
#define SD_OUTPLL(h,offset,data)    (((volatile SD_REG *)((h)->reg_base+(offset)))->WORD.LL = (data))
#define SD_INPHH(h,offset)          (((volatile SD_REG *)((h)->reg_base+(offset)))->WORD.HH)
#define SD_INPHL(h,offset)          (((volatile SD_REG *)((h)->reg_base+(offset)))->WORD.HL)
#define SD_INPLH(h,offset)          (((volatile SD_REG *)((h)->reg_base+(offset)))->WORD.LH)
#define SD_INPLL(h,offset)          (((volatile SD_REG *)((h)->reg_base+(offset)))->WORD.LL)

/* ==== command type ==== */
/* ---- eSD commands ---- */
#define CMD43                       (0x052B)                    /* SELECT_PARTITIONS */
#define CMD44                       (0x0C2C)                    /* MANAGE_PARTITIONS */
#define CMD45                       (0x1C2D)                    /* QUERY_PARTITIONS */

/* ==== SD Card detect ==== */
#define SD_CD_DETECT                (0x0001u)                   /* card detection */

/* === SDR104 READ/WRITE CMD === */
#define SDR104_READ_CMD              (0x7c00u)

/* === SD Set Clock === */
#define SD_CLK_CTRL_SCLKEN           (0x0100uL)

/* ==== SD Driver work buffer (allocated by File system) ==== */
typedef struct __sdhndl{                /* SD handle */
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
    int32_t     (*int_cd_callback)(int32_t,int32_t);    /* callback function for card detection */
    int32_t     (*int_format_callback)(int32_t);        /* callback function for card format */
    int32_t     (*int_callback)(int32_t,int32_t);       /* callback function for interrupt flags */
    int32_t     (*int_dma_callback)(int32_t,int32_t);   /* callback function for interrupt flags */    
    uint32_t    resp_status;                            /* R1/R1b response status */
    uint16_t    ocr[4/sizeof(uint16_t)];                /* OCR value */
    uint16_t    if_cond[4/sizeof(uint16_t)];            /* IF_COND value */
    uint16_t    cid[16/sizeof(uint16_t)];               /* CID value */
    uint16_t    csd[16/sizeof(uint16_t)];               /* CSD value */
    uint16_t    dsr[2/sizeof(uint16_t)];                /* DSR value */
    uint16_t    rca[4/sizeof(uint16_t)];                /* RCA value */
    uint16_t    scr[8/sizeof(uint16_t)];                /* SCR value */
    uint16_t    sdstatus[16/sizeof(uint16_t)];          /* SD STATUS value */
    uint16_t    status_data[18/sizeof(uint16_t)];       /* STATUS DATA value (phys spec ver1.10) */
    uint8_t     *rw_buff;                               /* work buffer pointer */
    uint32_t    buff_size;                              /* work buffer size */
    int32_t     sup_if_mode;                            /* supported bus width (1bit:0 4bits:1) */
}SDHNDL;

extern SDHNDL *SDHandle[NUM_PORT];

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
/* ==== proto type ==== */
/* ---- sd_init.c ---- */
int32_t _sd_init_hndl(SDHNDL *hndl,uint32_t mode,uint32_t voltage);

/* ---- sd_mount.c ---- */
int32_t _sd_card_init(SDHNDL *hndl);
int32_t _sd_mem_mount(SDHNDL *hndl);
int32_t _sd_card_get_status(SDHNDL *hndl);
int32_t _sd_card_get_scr(SDHNDL *hndl);
int32_t _sd_read_byte(SDHNDL *hndl, uint16_t cmd, uint16_t h_arg, uint16_t l_arg, uint8_t *readbuff, uint16_t byte);
int32_t _sd_write_byte(SDHNDL *hndl, uint16_t cmd, uint16_t h_arg, uint16_t l_arg, uint8_t *writebuff, uint16_t byte);
int32_t _sd_calc_erase_sector(SDHNDL *hndl);

/* ---- sd_trns.c ---- */
int32_t _sd_software_trans(SDHNDL *hndl,uint8_t *buff,int32_t cnt,int32_t dir);
int32_t _sd_dma_trans(SDHNDL *hndl,int32_t cnt);

/* ---- sd_read.c ---- */
 /* no function */

/* ---- sd_write.c ---- */
int32_t _sd_write_sect(SDHNDL *hndl,uint8_t *buff,uint32_t psn,
    int32_t cnt,int32_t writemode);

/* ---- sd_cd.c ---- */
int32_t _sd_check_media(SDHNDL *hndl);
int32_t _sd_get_ext_cd_int(SDHNDL *hndl);

/* ---- sd_cmd.c ---- */
int32_t _sd_send_cmd(SDHNDL *hndl,uint16_t cmd);
int32_t _sd_send_acmd(SDHNDL *hndl,uint16_t cmd,uint16_t h_arg,uint16_t l_arg);
int32_t _sd_send_mcmd(SDHNDL *hndl,uint16_t cmd,uint32_t startaddr);
int32_t _sd_card_send_cmd_arg(SDHNDL *hndl,uint16_t cmd, int32_t resp,uint16_t h_arg,uint16_t l_arg);
void _sd_set_arg(SDHNDL *hndl,uint16_t h_arg,uint16_t l_arg);
int32_t _sd_card_send_ocr(SDHNDL *hndl,int32_t type);
int32_t _sd_check_resp_error(SDHNDL *hndl);
int32_t _sd_get_resp(SDHNDL *hndl,int32_t resp);
int32_t _sd_check_csd(SDHNDL *hndl);
int32_t _sd_check_info2_err(SDHNDL *hndl);

/* ---- sd_int.c ---- */
int32_t _sd_set_int_mask(SDHNDL *hndl,uint64_t mask1,uint64_t mask2);
int32_t _sd_clear_int_mask(SDHNDL *hndl, uint64_t mask1, uint64_t mask2);
int32_t _sd_get_int(SDHNDL *hndl);
int32_t _sd_clear_info(SDHNDL *hndl, uint64_t clear_info1, uint64_t clear_info2);
int32_t _sd_set_int_dm_mask(SDHNDL *hndl,uint64_t mask1,uint64_t mask2);
int32_t _sd_clear_int_dm_mask(SDHNDL *hndl, uint64_t mask1, uint64_t mask2);
int32_t _sd_get_int_dm(SDHNDL *hndl);
int32_t _sd_clear_dm_info(SDHNDL *hndl, uint64_t clear_info1, uint64_t clear_info2);

/* ---- sd_util.c ---- */
int32_t _sd_set_clock(SDHNDL *hndl,int32_t clock,int32_t enable);
int32_t _sd_set_port(SDHNDL *hndl,int32_t port);
int32_t _sd_iswp(SDHNDL *hndl);
int32_t _sd_set_err(SDHNDL *hndl,int32_t error);
int32_t _sd_bit_search(uint16_t data);
int32_t _sd_get_size(SDHNDL *hndl,uint32_t area);
int32_t _sd_standby(SDHNDL *hndl);
int32_t _sd_active(SDHNDL *hndl);
int32_t _sd_inactive(SDHNDL *hndl);
int32_t _sd_memset(uint8_t *p,uint8_t data, uint32_t cnt);
int32_t _sd_memcpy(uint8_t *dst,uint8_t *src,uint32_t cnt);
uint16_t _sd_rand(void);
void    _sd_srand(uint32_t seed);
int32_t _sd_wait_rbusy(SDHNDL *hndl,int32_t time);

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

#endif  /* _SD_H_   */

/* End of File */
