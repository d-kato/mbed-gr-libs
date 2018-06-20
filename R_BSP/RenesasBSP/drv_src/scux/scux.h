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
* Copyright (C) 2013-2016 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**************************************************************************//**
* @file          scux.h
* $Rev: $
* $Date::                           $
* @brief         Sample driver internal header
******************************************************************************/

#ifndef SCUX_H
#define SCUX_H

/******************************************************************************
Includes <System Includes>, "Project Includes"
******************************************************************************/

#include "scux_if.h"
#include "scux_int.h"
#include "aioif.h"
#include "iodefine.h"
#include "cmsis.h"
#include "dma_if.h"
#include "bsp_drv_cmn.h"

/******************************************************************************
Macro definitions
******************************************************************************/

/* SCUX Register Set Value */
/* Init Value */
/* IPC */
#define IPCIR_IPC0_INIT_VALUE      (0x00000001U)
#define IPSLR_IPC0_INIT_VALUE      (0x00000000U)
/* OPC */
#define OPCIR_OPC0_INIT_VALUE      (0x00000001U)
#define OPSLR_OPC0_INIT_VALUE      (0x00000000U)
/* FFD */
#define FFDIR_FFD0_INIT_VALUE      (0x00000001U)
#define FDAIR_FFD0_INIT_VALUE      (0x00000000U)
#define DRQSR_FFD0_INIT_VALUE      (0x00000000U)
#define FFDPR_FFD0_INIT_VALUE      (0x00000000U)
#define FFDBR_FFD0_INIT_VALUE      (0x00000000U)
#define DEVMR_FFD0_INIT_VALUE      (0x00000000U)
#define DEVCR_FFD0_INIT_VALUE      (0x00000000U)
/* FFU */
#define FFUIR_FFU0_INIT_VALUE      (0x00000001U)
#define FUAIR_FFU0_INIT_VALUE      (0x00000000U)
#define URQSR_FFU0_INIT_VALUE      (0x00000000U)
#define FFUPR_FFU0_INIT_VALUE      (0x00000000U)
#define UEVMR_FFU0_INIT_VALUE      (0x00000000U)
#define UEVCR_FFU0_INIT_VALUE      (0x00000000U)
/* SRC */
#define SRCIR_2SRC0_INIT_VALUE     (0x00000001U)
#define SADIR_2SRC0_INIT_VALUE     (0x00000000U)
#define SRCBR_2SRC0_INIT_VALUE     (0x00000000U)
#define IFSCR_2SRC0_INIT_VALUE     (0x00000000U)
#define IFSVR_2SRC0_INIT_VALUE     (0x00000000U)
#define SRCCR_2SRC0_INIT_VALUE     (0x00000000U)
#define MNFSR_2SRC0_INIT_VALUE     (0x00000000U)
#define BFSSR_2SRC0_INIT_VALUE     (0x00000000U)
#define WATSR_2SRC0_INIT_VALUE     (0x00000000U)
#define SEVMR_2SRC0_INIT_VALUE     (0x00000000U)
#define SEVCR_2SRC0_INIT_VALUE     (0x00000000U)
#define SRCIRR_2SRC0_INIT_VALUE    (0x00000001U)
/* DVU */
#define DVUIR_DVU0_INIT_VALUE      (0x00000001U)
#define VADIR_DVU0_INIT_VALUE      (0x00000000U)
#define DVUBR_DVU0_INIT_VALUE      (0x00000000U)
#define DVUCR_DVU0_INIT_VALUE      (0x00000000U)
#define ZCMCR_DVU0_INIT_VALUE      (0x00000000U)
#define VRCTR_DVU0_INIT_VALUE      (0x00000000U)
#define VRPDR_DVU0_INIT_VALUE      (0x00000000U)
#define VRDBR_DVU0_INIT_VALUE      (0x00000000U)
#define VRWTR_DVU0_INIT_VALUE      (0x00000000U)
#define VOL_N_R_DVU0_INIT_VALUE    (0x00000000U)
#define DVUER_DVU0_INIT_VALUE      (0x00000000U)
#define VEVMR_DVU0_INIT_VALUE      (0x00000000U)
#define VEVCR_DVU0_INIT_VALUE      (0x00000000U)
/* MIX */
#define MIXIR_MIX0_0_INIT_VALUE    (0x00000001U)
#define MADIR_MIX0_0_INIT_VALUE    (0x00000000U)
#define MIXBR_MIX0_0_INIT_VALUE    (0x00000000U)
#define MIXMR_MIX0_0_INIT_VALUE    (0x00000000U)
#define MVPDR_MIX0_0_INIT_VALUE    (0x00000000U)
#define MDB_N_R_MIX0_0_INIT_VALUE  (0x00000000U)
#define MDBER_MIX0_0_INIT_VALUE    (0x00000000U)
/* CIM */
#define SWRSR_CIM_INIT_VALUE       (0x00000001U)
#define DMACR_CIM_INIT_VALUE       (0x00000000U)
#define SSIRSEL_CIM_INIT_VALUE     (0x00000000U)
#define FDTSEL_CIM_INIT_VALUE      (0x00000000U)
#define FUTSEL_CIM_INIT_VALUE      (0x00000000U)
#define SSIPMD_CIM_INIT_VALUE      (0x00000000U)
#define SSICTRL_CIM_INIT_VALUE     (0x00000000U)
#define SRCRSEL_CIM_INIT_VALUE     (0x76543210U)
#define MIXRSEL_CIM_INIT_VALUE     (0x76543210U)

/* SCUX bit value & mask & fixed setting */

/* IPC */
#define IPCIR_IPC0_INIT_SET           (1U << 0)

#define IPSLR_IPC_PASS_SEL_MASK       (7U << 0)
#define IPSLR_IPC_PASS_SEL_SET_ASYNC  (3U << 0)
#define IPSLR_IPC_PASS_SEL_SET_SYNC   (4U << 0)

/* OPC */
#define OPCIR_OPC0_INIT_SET           (1U << 0)

#define OPSLR_OPC_PASS_SEL_MASK       (7U << 0)
#define OPSLR_OPC_PASS_SEL_SET_DIRECT (1U << 0)
#define OPSLR_OPC_PASS_SEL_SET_ASYNC  (3U << 0)
#define OPSLR_OPC_PASS_SEL_SET_SYNC   (4U << 0)

/* FFD */
#define FFDIR_FFD0_INIT_SET           (1U << 0)

#define FDAIR_FFD0_CHNUM_MASK         (0x0FU << 0)

#define DRQSR_FFD0_SIZE_MASK          (0x0FU << 0)

#define FFDPR_FFD0_PASS_SET_ASYNC     (1U << 0)
#define FFDPR_FFD0_PASS_SET_SYNC      (2U << 0)

#define FFDBR_FFD0_BOOT_SET           (1U << 0)

#define DEVMR_FFD0_DEVMUF_SET         (1U << 31)
#define DEVMR_FFD0_DEVMOF_SET         (1U << 30)
#define DEVMR_FFD0_DEVMOL_SET         (1U << 29)
#define DEVMR_FFD0_DEVMIUF_SET        (1U << 28)
#define DEVMR_FFD0_DEVMRQ_SET         (1U << 15)

#define DEVCR_FFD0_DEVCUF_MASK        (1U << 31)
#define DEVCR_FFD0_DEVCUF_SET         (1U << 31)
#define DEVCR_FFD0_DEVCOF_MASK        (1U << 30)
#define DEVCR_FFD0_DEVCOF_SET         (1U << 30)
#define DEVCR_FFD0_DEVCOL_MASK        (1U << 29)
#define DEVCR_FFD0_DEVCOL_SET         (1U << 29)
#define DEVCR_FFD0_DEVCIUF_MASK       (1U << 28)
#define DEVCR_FFD0_DEVCIUF_SET        (1U << 28)
#define DEVCR_FFD0_DEVCRQ_MASK        (1U << 15)
#define DEVCR_FFD0_DEVCRQ_SET         (1U << 15)

/* FFU */
#define FFUIR_FFU0_INIT_SET           (1U << 0)

#define FUAIR_FFU0_CHNUM_MASK         (0x0FU << 0)

#define URQSR_FFU0_SIZE_MASK          (0x0FU << 0)

#define FFUPR_FFU0_PASS_SET_ASYNC     (1U << 0)
#define FFUPR_FFU0_PASS_SET_SYNC      (2U << 0)

#define UEVMR_FFU0_UEVMUF_SET         (1U << 31)
#define UEVMR_FFU0_UEVMOF_SET         (1U << 30)
#define UEVMR_FFU0_UEVMOL_SET         (1U << 29)
#define UEVMR_FFU0_UEVMRQ_SET         (1U << 15)

#define UEVCR_FFU0_UEVCUF_MASK        (1U << 31)
#define UEVCR_FFU0_UEVCUF_SET         (1U << 31)
#define UEVCR_FFU0_UEVCOF_MASK        (1U << 30)
#define UEVCR_FFU0_UEVCOF_SET         (1U << 30)
#define UEVCR_FFU0_UEVCOL_MASK        (1U << 29)
#define UEVCR_FFU0_UEVCOL_SET         (1U << 29)
#define UEVCR_FFU0_UEVCRQ_MASK        (1U << 15)
#define UEVCR_FFU0_UEVCRQ_SET         (1U << 15)

/* SRC */
#define SRCIR_2SRC0_INIT_SET          (1U << 0)

#define SADIR_2SRC0_CHNUM_MASK        (0x0FU << 0)
#define SADIR_2SRC0_OTBL_MASK         (0x1FU << 16)
#define SADIR_2SRC0_OTBL_SET_24BIT    (0x0U << 16)
#define SADIR_2SRC0_OTBL_SET_16BIT    (0x8U << 16)

#define SRCBR_2SRC0_BYPASS_SET        (0x1U << 0)

#define IFSCR_2SRC0_INTIFSEN_SET      (0x1U << 0)

#define IFSVR_2SRC0_INTIFS_MASK       (0x0FFFFFFFU << 0)

#define SRCCR_2SRC0_BASE_VALUE        (0x00010110U)
#define SRCCR_2SRC0_WATMD_SET         (1U << 20)
#define SRCCR_2SRC0_BUFMD_SET         (1U << 12)
#define SRCCR_2SRC0_SRCMD_SET         (1U << 0)

#define MNFSR_2SRC0_MINFS_MASK        (0x0FFFFFFFU << 0)

#define BFSSR_2SRC0_BUFDATA_MASK      (0x3FFU << 16)
#define BFSSR_2SRC0_BUFDATA_SET_DELAY_MODE1 (0x80U << 16)
#define BFSSR_2SRC0_BUFDATA_SET_DELAY_MODE2 (0x40U << 16)
#define BFSSR_2SRC0_BUFIN_MASK        (0x0FU << 0)
#define BFSSR_2SRC0_BUFIN_SET_DELAY_MODE (0x05U << 0)

#define WATSR_2SRC0_WTIME_MASK        (0xFFFFFFU << 0)

#define SEVMR_2SRC0_EVMUF_SET         (1U << 31)
#define SEVMR_2SRC0_EVMOF_SET         (1U << 30)

#define SEVCR_2SRC0_EVCUF_MASK        (1U << 31)
#define SEVCR_2SRC0_EVCUF_SET         (1U << 31)
#define SEVCR_2SRC0_EVCOF_MASK        (1U << 30)
#define SEVCR_2SRC0_EVCOF_SET         (1U << 30)

#define SRCIRR_2SRC0_INIT_SET         (1U << 0)

#define DVUIR_DVU0_INIT_SET           (1U << 0)

#define VADIR_DVU0_OTBL_MASK          (0x1FU << 16)
#define VADIR_DVU0_OTBL_SET_24BIT     (0x0U << 16)
#define VADIR_DVU0_OTBL_SET_16BIT     (0x8U << 16)
#define VADIR_DVU0_CHNUM_MASK         (0x0FU << 0)

#define DVUBR_DVU0_BYPASS_SET         (0x1U << 0)

#define DVUCR_DVU0_VVMD_SET           (1U << 8)
#define DVUCR_DVU0_VRMD_SET           (1U << 4)
#define DVUCR_DVU0_ZCMD_SET           (1U << 0)

#define ZCMCR_DVU0_ZCEN_SET           (1U << 0)

#define VRCTR_DVU0_VREN_SET           (1U << 0)

#define VRPDR_DVU0_VRPDUP_SHIFT       (8U)
#define VRPDR_DVU0_VRPDUP_MASK        (0x1F << VRPDR_DVU0_VRPDUP_SHIFT)
#define VRPDR_DVU0_VRPDDW_SHIFT       (0U)
#define VRPDR_DVU0_VRPDDW_MASK        (0x1F << VRPDR_DVU0_VRPDDW_SHIFT)

#define VRDBR_DVU0_VRDB_MASK          (0x3FFU << 0)

#define VRWTR_DVU0_VRWT_MASK          (0xFFFFFFU << 0)

#define VOL_N_R_DVU_VOLVAL_MASK       (0xFFFFFFU << 0)

#define DVUER_DVU0_DVUEN_SET          (1U << 0)

#define DVUSR_DVU0_ZSTS_N_SHIFT       (16U)
#define DVUSR_DVU0_ZSTS_N_MASK        (1U << DVUSR_DVU0_ZSTS_N_SHIFT)
#define DVUSR_DVU0_ZSTS_MASK          (0xFFU << DVUSR_DVU0_ZSTS_N_SHIFT)
#define DVUSR_DVU0_VRSTS_MASK         (7U << 0)
#define DVUSR_DVU0_VRSTS_VOL_FIX      (4U << 0)

#define VEVMR_DVU0_VEVMZCM_SET        (1U << 24)

#define VEVCR_DVU0_VEVCZCM_SET        (1U << 24)

/* MIX */
#define MIXIR_MIX0_INIT_SET           (1U << 0)

#define MADIR_MIX0_CHNUM_MASK         (0x0FU << 0)

#define MIXMR_MIX0_MIXMODE_SET        (1U << 0)

#define MVPDR_MIX0_MXPDUP_SHIFT       (8U)
#define MVPDR_MIX0_MXPDUP_MASK        (0x0FU << MVPDR_MIX0_MXPDUP_SHIFT)
#define MVPDR_MIX0_MXPDDW_SHIFT       (0U)
#define MVPDR_MIX0_MXPDDW_MASK        (0x0FU << MVPDR_MIX0_MXPDDW_SHIFT)

#define MDB_N_R_MIX0_MIXDB_N_MASK     (0x3FFU <<0)

#define MDBER_MIX0_MIXDBEN_SET        (1U << 0)

#define MIXSR_MIX0_MIXSTS_MASK        (3U << 0)

/* CIM */
#define SWRSR_CIM_SWRST_SET           (1U << 0)

#define DMACR_CIM_DMAMDFFU_N_SET      (1U << 4)
#define DMACR_CIM_DMAMDFFD_N_SET      (1U << 0)

#if defined(TARGET_RZA1H) /* mbed */
#define SSIRSEL_CIM_SOSEL5_MASK       (3U << 20)
#define SSIRSEL_CIM_SOSEL5_SRC3_SET   (0U << 20)
#define SSIRSEL_CIM_SOSEL5_SRC1_SET   (1U << 20)
#define SSIRSEL_CIM_SOSEL5_SRC0_SET   (2U << 20)
#define SSIRSEL_CIM_SOSEL5_MIX_SET    (3U << 20)
#define SSIRSEL_CIM_SOSEL4_MASK       (3U << 16)
#define SSIRSEL_CIM_SOSEL4_SRC2_SET   (0U << 16)
#define SSIRSEL_CIM_SOSEL4_SRC1_SET   (1U << 16)
#define SSIRSEL_CIM_SOSEL4_SRC0_SET   (2U << 16)
#define SSIRSEL_CIM_SOSEL4_MIX_SET    (3U << 16)
#endif /* mbed */
#define SSIRSEL_CIM_SOSEL3_MASK       (3U << 12)
#define SSIRSEL_CIM_SOSEL3_SRC1_SET   (0U << 12)
#define SSIRSEL_CIM_SOSEL3_SRC0_SET   (1U << 12)
#define SSIRSEL_CIM_SOSEL3_MIX_SET    (2U << 12)
#define SSIRSEL_CIM_SOSEL2_MASK       (3U << 8)
#define SSIRSEL_CIM_SOSEL2_SRC3_SET   (0U << 8)
#define SSIRSEL_CIM_SOSEL2_SRC0_SET   (1U << 8)
#define SSIRSEL_CIM_SOSEL2_SRC1_SET   (2U << 8)
#define SSIRSEL_CIM_SOSEL2_MIX_SET    (3U << 8)
#define SSIRSEL_CIM_SOSEL1_MASK       (3U << 4)
#define SSIRSEL_CIM_SOSEL1_SRC2_SET   (0U << 4)
#define SSIRSEL_CIM_SOSEL1_SRC0_SET   (1U << 4)
#define SSIRSEL_CIM_SOSEL1_SRC1_SET   (2U << 4)
#define SSIRSEL_CIM_SOSEL1_MIX_SET    (3U << 4)
#define SSIRSEL_CIM_SOSEL0_MASK       (3U << 0)
#define SSIRSEL_CIM_SOSEL0_SRC0_SET   (0U << 0)
#define SSIRSEL_CIM_SOSEL0_SRC1_SET   (1U << 0)
#define SSIRSEL_CIM_SOSEL0_MIX_SET    (2U << 0)

#define FDTSEL_CIM_SCKDIV_SHIFT       (16U)
#define FDTSEL_CIM_SCKDIV_MASK        (0x7FFU << FDTSEL_CIM_SCKDIV_SHIFT)
#define FDTSEL_CIM_DIVEN_MASK         (1U << 8)
#define FDTSEL_CIM_DIVEN_SET          (1U << 8)
#define FDTSEL_CIM_MTUSEL_MASK        (1U << 4)
#define FDTSEL_CIM_MTUSEL_SET_TIOC3A  (0U << 4)
#define FDTSEL_CIM_MTUSEL_SET_TIOC4A  (1U << 4)
#define FDTSEL_CIM_SCKSEL_MASK        (0x0FU << 0)
#define FDTSEL_CIM_SCKSEL_AUDIO_CLK_SET (0U)
#define FDTSEL_CIM_SCKSEL_AUDIO_X1_SET  (1U)
#define FDTSEL_CIM_SCKSEL_MLB_CLK_SET   (2U)
#define FDTSEL_CIM_SCKSEL_USB_X1_SET    (3U)
#define FDTSEL_CIM_SCKSEL_CLKP1_2_SET   (4U)
#define FDTSEL_CIM_SCKSEL_MTUSEL2_SET   (5U)
#define FDTSEL_CIM_SCKSEL_SSIF0_WS_SET  (8U)
#define FDTSEL_CIM_SCKSEL_SSIF1_WS_SET  (9U)
#define FDTSEL_CIM_SCKSEL_SSIF2_WS_SET  (10U)
#define FDTSEL_CIM_SCKSEL_SSIF3_WS_SET  (11U)
#if defined(TARGET_RZA1H) /* mbed */
#define FDTSEL_CIM_SCKSEL_SSIF4_WS_SET  (12U)
#define FDTSEL_CIM_SCKSEL_SSIF5_WS_SET  (13U)
#endif /* mbed */

#define FUTSEL_CIM_SCKDIV_SHIFT         (16U)
#define FUTSEL_CIM_SCKDIV_MASK          (0x7FFU << FDTSEL_CIM_SCKDIV_SHIFT)
#define FUTSEL_CIM_DIVEN_MASK           (1U << 8)
#define FUTSEL_CIM_DIVEN_SET            (1U << 8)
#define FUTSEL_CIM_MTUSEL_MASK          (1U << 4)
#define FUTSEL_CIM_MTUSEL_SET_TIOC3A    (0U << 4)
#define FUTSEL_CIM_MTUSEL_SET_TIOC4A    (1U << 4)
#define FUTSEL_CIM_SCKSEL_MASK          (0x0FU << 0)
#define FUTSEL_CIM_SCKSEL_AUDIO_CLK_SET (0U)
#define FUTSEL_CIM_SCKSEL_AUDIO_X1_SET  (1U)
#define FUTSEL_CIM_SCKSEL_MLB_CLK_SET   (2U)
#define FUTSEL_CIM_SCKSEL_USB_X1_SET    (3U)
#define FUTSEL_CIM_SCKSEL_CLKP1_2_SET   (4U)
#define FUTSEL_CIM_SCKSEL_MTUSEL2_SET   (5U)
#define FUTSEL_CIM_SCKSEL_SSIF0_WS_SET  (8U)
#define FUTSEL_CIM_SCKSEL_SSIF1_WS_SET  (9U)
#define FUTSEL_CIM_SCKSEL_SSIF2_WS_SET  (10U)
#define FUTSEL_CIM_SCKSEL_SSIF3_WS_SET  (11U)
#if defined(TARGET_RZA1H) /* mbed */
#define FUTSEL_CIM_SCKSEL_SSIF4_WS_SET  (12U)
#define FUTSEL_CIM_SCKSEL_SSIF5_WS_SET  (13U)
#endif /* mbed */

#if defined(TARGET_RZA1H) /* mbed */
#define SSIPMD_CIM_SSI5CKS_MASK     (1U << 21)
#define SSIPMD_CIM_SSI5CKS_SET      (1U << 21)
#define SSIPMD_CIM_SSI4CKS_MASK     (1U << 20)
#define SSIPMD_CIM_SSI4CKS_SET      (1U << 20)
#endif /* mbed */
#define SSIPMD_CIM_SSI3CKS_MASK     (1U << 19)
#define SSIPMD_CIM_SSI3CKS_SET      (1U << 19)
#define SSIPMD_CIM_SSI2CKS_MASK     (1U << 18)
#define SSIPMD_CIM_SSI2CKS_SET      (1U << 18)
#define SSIPMD_CIM_SSI1CKS_MASK     (1U << 17)
#define SSIPMD_CIM_SSI1CKS_SET      (1U << 17)
#define SSIPMD_CIM_SSI0CKS_MASK     (1U << 16)
#define SSIPMD_CIM_SSI0CKS_SET      (1U << 16)
#define SSIPMD_CIM_SSI3PMD_SHIFT    (14U)
#define SSIPMD_CIM_SSI3PMD_MASK     (3U << SSIPMD_CIM_SSI3PMD_SHIFT)
#if defined(TARGET_RZA1H) /* mbed */
#define SSIPMD_CIM_SSI345EN_MASK    (1U << 12)
#define SSIPMD_CIM_SSI345EN_SET     (1U << 12)
#define SSIPMD_CIM_SSI4PMD_SHIFT    (10U)
#define SSIPMD_CIM_SSI4PMD_MASK     (3U << SSIPMD_CIM_SSI4PMD_SHIFT)
#define SSIPMD_CIM_SSI5PMD_SHIFT    (8U)
#define SSIPMD_CIM_SSI5PMD_MASK     (3U << SSIPMD_CIM_SSI5PMD_SHIFT)
#endif /* mbed */
#define SSIPMD_CIM_SSI012EN_MASK    (1U << 4)
#define SSIPMD_CIM_SSI012EN_SET     (1U << 4)
#define SSIPMD_CIM_SSI2PMD_SHIFT    (2U)
#define SSIPMD_CIM_SSI2PMD_MASK     (3U << SSIPMD_CIM_SSI2PMD_SHIFT)
#define SSIPMD_CIM_SSI1PMD_SHIFT    (0U)
#define SSIPMD_CIM_SSI1PMD_MASK     (3U << SSIPMD_CIM_SSI1PMD_SHIFT)

#define SSICTRL_CIM_SSI3TX_SET      (1U << 30)
#if defined(TARGET_RZA1H) /* mbed */
#define SSICTRL_CIM_SSI4TX_SET      (1U << 29)
#define SSICTRL_CIM_SSI5TX_SET      (1U << 28)
#define SSICTRL_CIM_SSI345TEN_SET   (1U << 17)
#endif /* mbed */
#define SSICTRL_CIM_SSI0TX_SET      (1U << 14)
#define SSICTRL_CIM_SSI1TX_SET      (1U << 13)
#define SSICTRL_CIM_SSI2TX_SET      (1U << 12)
#define SSICTRL_CIM_SSI012TEN_SET   (1U << 1)

#define SRCRSEL_CIM_PLACE_N_MASK    (7U)
#define SRCRSEL_CIM_PLACE_N_SHIFT   (4U)

#define MIXRSEL_CIM_PLACE_N_MASK    (7U)
#define MIXRSEL_CIM_PLACE_N_SHIFT   (4U)


/* SSIF Register Set Value */
/* Init Value */
#define SCUX_SSICR_INIT_VALUE       (0x00000000U)
#define SCUX_SSIFCR_INIT_VALUE      (0x00000000U)
#define SCUX_SSIFTDR_INIT_VALUE     (0x00000000U)
#define SCUX_SSITDMR_INIT_VALUE     (0x00000000U)
#define SCUX_SSIFCCR_INIT_VALUE     (0x00000000U)
#define SCUX_SSIFCMR_INIT_VALUE     (0x00000000U)

/* SSIF bit value & mask & fixed setting */
#define SCUX_SSICR_CKS_SET              (1U << 30)
#define SCUX_SSICR_TUIEN_SET            (1U << 29)
#define SCUX_SSICR_TOIEN_SET            (1U << 28)
#define SCUX_SSICR_RUIEN_SET            (1U << 27)
#define SCUX_SSICR_ROIEN_SET            (1U << 26)
#define SCUX_SSICR_CHNL_SET_1CH         (0U << 22)
#define SCUX_SSICR_CHNL_SET_2CH         (1U << 22)
#define SCUX_SSICR_CHNL_SET_3CH         (2U << 22)
#define SCUX_SSICR_CHNL_SET_4CH         (3U << 22)
#define SCUX_SSICR_DWL_16BIT_SET        (1U << 19)
#define SCUX_SSICR_DWL_24BIT_SET        (5U << 19)
#define SCUX_SSICR_SWL_SHIFT            (16U)
#define SCUX_SSICR_SCKD_SET             (1U << 15)
#define SCUX_SSICR_SWSD_SET             (1U << 14)
#define SCUX_SSICR_SCKP_SET             (1U << 13)
#define SCUX_SSICR_SWSP_SET             (1U << 12)
#define SCUX_SSICR_SPDP_SET             (1U << 11)
#define SCUX_SSICR_SDTA_SET             (1U << 10)
#define SCUX_SSICR_DEL_SET              (1U << 8)
#define SCUX_SSICR_CKDV_SHIFT           (4U)
#define SCUX_SSICR_TEN_SET              (1U << 1)

#define SCUX_SSITDMR_CONT_SET           (1U << 8)
#define SCUX_SSITDMR_TDM_SET            (1U << 0)

/* SCUX fixed value */
#define SCUX_RATE_CONVERT_CALC_VALUE     (0x400000U)
#define SCUX_CALC_MINFS_VALUE            (100U)
#define SCUX_MAX_DIV_CLK                 (2046U)
#define SCUX_MIN_RATE_MIN_PAERCENTAGE    (90U)
#define SCUX_MIN_RATE_MAX_PAERCENTAGE    (98U)
#define SCUX_MIN_RATE_DENOMINATOR        (100U)
#define SCUX_MAX_WAIT_TIME               (0xFFFFFFU)
#define SCUX_MAX_DIGITAL_VOLUME          (0x7FFFFFU)
#define SCUX_MAX_RAMP_VOLUME             (0x3FFU)
#define SCUX_RATE_INT_CONV_VALUE         (1000U)
#define SCUX_ADJUST_REST_VALUE           (1U)
#define SCUX_MAX_CONV_RATE               (16000U)
#define SCUX_MIN_CONV_RATE_NORMAL_CH1_2  (125U)
#define SCUX_MIN_CONV_RATE_NORMAL_CH4    (250U)
#define SCUX_MIN_CONV_RATE_NORMAL_CH6    (375U)
#define SCUX_MIN_CONV_RATE_NORMAL_CH8    (500U)
#define SCUX_MIN_CONV_RATE_DELAY1        (500U)
#define SCUX_MIN_CONV_RATE_DELAY2        (1000U)
#define SCUX_MIN_FREQ                    (1000U)
#define SCUX_MAX_FREQ_CH1_4              (96000U)
#define SCUX_MAX_FREQ_CH6                (66000U)
#define SCUX_MAX_FREQ_CH8                (49000U)
#define SCUX_PROC_DELAY_NONE             (0U)
#define SCUX_PROCESS_DELAY_NORMAL_CH1    (641U)
#define SCUX_PROCESS_DELAY_NORMAL_CH2    (321U)
#define SCUX_PROCESS_DELAY_NORMAL_CH4    (161U)
#define SCUX_PROCESS_DELAY_NORMAL_CH6    (102U)
#define SCUX_PROCESS_DELAY_NORMAL_CH8    (81U)
#define SCUX_PROCESS_DELAY_1_CH1_2       (81U)
#define SCUX_PROCESS_DELAY_2_CH1_2       (49U)
#define SCUX_LOGIC_DELAY_BYPASS_ON       (1U)
#define SCUX_LOGIC_DELAY_BYPASS_OFF      (3U)
#if defined(TARGET_RZA1H) /* mbed */
#define SCUX_AUDIO_X1                    (22579200U)
#else
#define SCUX_AUDIO_X1                    (12000000U)
#endif
#define SCUX_AUDIO_CLK                   (0U)
#define SCUX_MLB_CLK                     (66670000U)
#define SCUX_USB_X1                      (48000000U)
#define SCUX_CLKLP1_DIV2                 (33335000U)
#define SCUX_SYSTEMWORD_16               (16U)
#define SCUX_SYSTEMWORD_24               (24U)
#define SCUX_SYSTEMWORD_32               (32U)
#define SCUX_SYSTEMWORD_48               (48U)
#define SCUX_SYSTEMWORD_64               (64U)
#define SCUX_SYSTEMWORD_128              (128U)
#define SCUX_SYSTEMWORD_256              (256U)
#define SCUX_SSIF_CLK_DIV1               (1U)
#define SCUX_SSIF_CLK_DIV2               (2U)
#define SCUX_SSIF_CLK_DIV4               (4U)
#define SCUX_SSIF_CLK_DIV8               (8U)
#define SCUX_SSIF_CLK_DIV16              (16U)
#define SCUX_SSIF_CLK_DIV32              (32U)
#define SCUX_SSIF_CLK_DIV64              (64U)
#define SCUX_SSIF_CLK_DIV128             (128U)
#define SCUX_SSIF_CLK_DIV6               (6U)
#define SCUX_SSIF_CLK_DIV12              (12U)
#define SCUX_SSIF_CLK_DIV24              (24U)
#define SCUX_SSIF_CLK_DIV48              (48U)
#define SCUX_SSIF_CLK_DIV96              (96U)
#define SCUX_SSIF_GPIO_SNCR_SHIFT_CH0    (0U)
#define SCUX_SSIF_GPIO_SNCR_SHIFT_CH1    (1U)
#define SCUX_SSIF_GPIO_SNCR_SHIFT_CH2    (2U)
#define SCUX_SSIF_GPIO_SNCR_SHIFT_CH3    (3U)
#if defined(TARGET_RZA1H) /* mbed */
#define SCUX_SSIF_GPIO_SNCR_SHIFT_CH4    (4U)
#define SCUX_SSIF_GPIO_SNCR_SHIFT_CH5    (5U)
#endif /* mbed */
#define SCUX_DUMMY_BUF_SIZE              (4096U)
#define SCUX_GET_ROUTE_MASK              (0xF000)
#define SCUX_ROUTE_MEM_TO_MEM            (0x1000)
#define SCUX_ROUTE_SSIF                  (0x2000)
#define SCUX_ROUTE_MIX                   (0x3000)
#define SCUX_FIFO_SIZE_CH0_1             (1024U)
#define SCUX_FIFO_SIZE_CH2_3             (256U)
#define SCUX_SSIF_NO_USE_CH              (0xFFFFU)
#define SCUX_SSIF_USE_MIX_BIT            (0x1000U)
#define SRC_MOD_SRC0                     (0U)
#define SRC_MOD_SRC1                     (1U)
#define SCUX_EVEN_VALUE_DIV              (2U)
#define SCUX_SSIF_NUM_CH_ARRANGEMENT     (3U)
#define SCUX_SSIF_CH_ARRANGEMENT1        (0U)
#define SCUX_SSIF_CH_ARRANGEMENT2        (1U)
#define SCUX_SSIF_CH_ARRANGEMENT3        (2U)
#define SCUX_DMA_UNIT_SIZE16             (2U)
#define SCUX_DMA_UNIT_SIZE24             (4U)
#define SCUX_HALF_SIZE_VALUE             (2U)
#define SCUX_RAMP_WAIT_MAX               (10U)

/*************************************************************************
 Enumerated Types
*************************************************************************/

/* DRV Status */
typedef enum
{
    SCUX_DRV_UNINIT = 0,   /* Uninit */
#if(1) /* mbed */
    SCUX_DRV_INIT_RUNNING = 1, /* Init running */
    SCUX_DRV_INIT         = 2  /* Init */
#else  /* not mbed */
    SCUX_DRV_INIT   = 1    /* Init */
#endif /* end mbed */
} scux_stat_drv_t;

/* Channel Status */
typedef enum
{
    SCUX_CH_UNINIT         = 0,   /* Uninit */
    SCUX_CH_INIT           = 1,   /* Init */
    SCUX_CH_STOP           = 2,   /* Open (request can't be received) */
    SCUX_CH_TRANS_IDLE     = 4,   /* Open (request is not received) */
    SCUX_CH_TRANS_RD       = 8,   /* Transfer (read only) */
    SCUX_CH_TRANS_WR       = 16,  /* Transfer (read only) */
    SCUX_CH_TRANS_RDWR     = 32,  /* Transfer (read & write) */
    SCUX_CH_STOP_WAIT      = 64,  /* Transfer (wait flush stop) */
    SCUX_CH_STOP_WAIT_IDLE = 128  /* Transfer (wait flush stop and request is not received */
} scux_stat_ch_t;

/* fifo size */
typedef enum
{
    SCUX_FIFO_REQ_SIZE_256_64 = 0, /* fifo size is 256 data (SRC0, 1) and 64 data (SRC2, 3) */
    SCUX_FIFO_REQ_SIZE_128_32 = 1, /* fifo size is 128 data (SRC0, 1) and 32 data (SRC2, 3) */
    SCUX_FIFO_REQ_SIZE_64_16  = 2, /* fifo size is 64 data (SRC0, 1) and 16 data (SRC2, 3) */
    SCUX_FIFO_REQ_SIZE_32_8   = 3, /* fifo size is 32 data (SRC0, 1) and 8 data (SRC2, 3) */
    SCUX_FIFO_REQ_SIZE_16_4   = 4, /* fifo size is 16 data (SRC0, 1) and 4 data (SRC2, 3) */
    SCUX_FIFO_REQ_SIZE_8_2    = 5, /* fifo size is 8 data (SRC0, 1) and 2 data (SRC2, 3) */
    SCUX_FIFO_REQ_SIZE_4_1    = 6, /* fifo size is 4 data (SRC0, 1) and 1 data (SRC2, 3) */
    SCUX_FIFO_REQ_SIZE_2      = 7, /* fifo size is 2 data (SRC0, 1) */
    SCUX_FIFO_REQ_SIZE_1      = 8  /* fifo size is 1 data (SRC0, 1) */
} scux_fifo_req_size_t;

/* SSIF over sampling clock divide rate */
typedef enum
{
    SCUX_SSIF_CKDIV_1      = 0,   /* divide rate 1/1 */
    SCUX_SSIF_CKDIV_2      = 1,   /* divide rate 1/2 */
    SCUX_SSIF_CKDIV_4      = 2,   /* divide rate 1/4 */
    SCUX_SSIF_CKDIV_8      = 3,   /* divide rate 1/8 */
    SCUX_SSIF_CKDIV_16     = 4,   /* divide rate 1/16 */
    SCUX_SSIF_CKDIV_32     = 5,   /* divide rate 1/32 */
    SCUX_SSIF_CKDIV_64     = 6,   /* divide rate 1/64 */
    SCUX_SSIF_CKDIV_128    = 7,   /* divide rate 1/128 */
    SCUX_SSIF_CKDIV_6      = 8,   /* divide rate 1/6 */
    SCUX_SSIF_CKDIV_12     = 9,   /* divide rate 1/12 */
    SCUX_SSIF_CKDIV_24     = 10,  /* divide rate 1/24 */
    SCUX_SSIF_CKDIV_48     = 11,  /* divide rate 1/48 */
    SCUX_SSIF_CKDIV_96     = 12   /* divide rate 1/96 */
} scux_ssif_ckdiv_t;

/*************************************************************************
 Structures
*************************************************************************/

/* Information of SCUX register */
typedef struct
{
    struct st_scux_from_dvuir_dvu0_n            *p_dvu_reg;      /* DVU register */
    struct st_scux_from_srcir0_2src0_n          *p_src_reg;      /* SRC register */
    struct st_scux_from_ffuir_ffu0_n            *p_ffu_reg;      /* FFU register */
    struct st_scux_from_ffdir_ffd0_n            *p_ffd_reg;      /* FFD register */
    struct st_scux_from_opcir_opc0_n            *p_opc_reg;      /* OPC register */
    struct st_scux_from_ipcir_ipc0_n            *p_ipc_reg;      /* IPC register */
    volatile uint32_t                           *mixir_mix0_0;   /* MIXIR_MIX0_0 register */
    volatile uint32_t                           *madir_mix0_0;   /* MADIR_MIX0_0 register */
    volatile uint32_t                           *mixbr_mix0_0;   /* MIXBR_MIX0_0 register */
    volatile uint32_t                           *mixmr_mix0_0;   /* MIXMR_MIX0_0 register */
    volatile uint32_t                           *mvpdr_mix0_0;   /* MVPDR_MIX0_0 register */
    volatile uint32_t                           *mdb_n_r_mix0_0; /* MDBAR_MIX0_0 - MDBDR_MIX0_0 register */
    volatile uint32_t                           *mdber_mix0_0;   /* MDBER_MIX0_0 register */
    volatile uint32_t                           *mixsr_mix0_0;   /* MIXSR_MIX0_0 register */
    volatile uint32_t                           *swrsr_cim;      /* SWRSR_CIM register */
    volatile uint32_t                           *dmacr_cim;      /* DMACR_CIM register */
    volatile uint32_t                           *dmatd_n_cim;    /* DMATD0_CIM - DMATD3_CIM register */
    volatile uint32_t                           *dmatu_n_cim;    /* DMATU0_CIM - DMATU3_CIM register */
    volatile uint32_t                           *ssirsel_cim;    /* SSIRSEL_CIM register */
    volatile uint32_t                           *fdtsel_n_cim;   /* FDTSEL0_CIM - FDTSEL3_CIM register */
    volatile uint32_t                           *futsel_n_cim;   /* FUTSEL0_CIM - FUTSEL3_CIM register */
    volatile uint32_t                           *ssipmd_cim;     /* SSIPMD_CIM register */
    volatile uint32_t                           *ssictrl_cim;    /* SSICTRL_CIM register */
    volatile uint32_t                           *srcrsel_n_cim;  /* SRCRSEL0_CIM - SRCRSEL3_CIM register */
    volatile uint32_t                           *mixrsel_cim;    /* MIXRSEL_CIM register */
} scux_reg_info_t;

/* Information of SSIF */
typedef struct
{
    bool_t                  ssif_enable;              /* USE SSIF flag */
    bool_t                  ssif_setup;               /* SSIF setup complete flag */
    osSemaphoreId_t         sem_ch_scux_ssif_access;  /* SSIF info semaphore */
    int_t                   scux_channel;             /* SCUX ch which is using SSIF */
    scux_src_clk_source_t   pin_clk;                  /* AUDIO_CLK source */
    scux_pin_mode_t         pin_mode;                 /* SSIF sync setting */
    scux_pin_mode_t         pin_mode_backup;          /* back up of SSIF sync setting */
    scux_ssif_cfg_t         ssif_cfg;                 /* SSIF parameters */
    scux_ssif_ckdiv_t       clk_div;                  /* Over sampling clock divide rate */
    volatile struct st_ssif *p_scux_ssif_reg;         /* SSIF register */
} scux_ssif_info_t;

/* Information of Driver */
typedef struct
{
    osSemaphoreId_t      sem_shared_access;        /* Shared info semaphore */
    bool_t               mix_setup;                /* MIX setup complete flag */
    uint32_t             mix_run_ch;               /* MIX used channel */
    uint32_t             mix_ssif_ch;              /* SSIF channel on used MIX */
    bool_t               mixmode_ramp;             /* Ramp mode / Step mode select */
    scux_mix_ramp_time_t up_period;                /* Ramp up period */
    scux_mix_ramp_time_t down_period;              /* Ramp down period */
    uint32_t             mix_vol[SCUX_CH_NUM];     /* MIX volume value */
    scux_audio_channel_t select_out_data_ch[SCUX_AUDIO_CH_MAX]; /* Audio data position setting */
    uint32_t             ssictrl_cim_value;        /* SSICTRL_CIM register (write only) value */
} scux_shared_info_t;

/* Information of Channel */
typedef struct
{
    int_t              channel;                    /* Channel Number */
    bool_t             enabled;                    /* Used channel flag */
    int_t              open_flags;                 /* Read/write flag */
    scux_stat_ch_t     ch_stat;                    /* Channel Status */
    osSemaphoreId_t    sem_ch_scux_access;         /* Channel Semaphore */
    AHF_S              tx_que;                     /* Write request queue */
    AHF_S              rx_que;                     /* Read request queue */
    AIOCB              *p_tx_aio;                  /* Write request information pointer */
    AIOCB              *p_tx_next_aio;             /* Next write request information pointer */
    AIOCB              *p_rx_aio;                  /* Read request information pointer */
    AIOCB              *p_rx_next_aio;             /* Next read request information pointer */
    uint8_t            int_level;                  /* Interrupt priority */
    IRQn_Type          int_num[SCUX_INT_MAX];      /* Each interrupt number */
    int_t              dma_tx_ch;                  /* Write DMA channel */
    int_t              dma_rx_ch;                  /* Read DMA channel */
    dma_res_select_t   dma_resource_tx;            /* Write DMA resource */
    dma_res_select_t   dma_resource_rx;            /* Read DMA resource */
    uint32_t           dma_tx_current_size;        /* Current write DMA size */
    uint32_t           dma_tx_next_size;           /* Next write DMA size */
    uint32_t           dma_rx_current_size;        /* Current read DMA size */
    uint32_t           dma_rx_next_size;           /* Next read DMA size */
    dma_ch_setup_t     dma_tx_setup;               /* Write DMA paramtter */
    dma_ch_setup_t     dma_rx_setup;               /* Read DMA paramtter */
    scux_reg_info_t    *p_scux_reg;                /* SCUX register information */
    scux_route_t       route_set;                  /* SCUX route paramter */
    scux_src_cfg_t     src_cfg;                    /* SRC parameter */
    scux_dvu_cfg_t     dvu_cfg;                    /* DVU parameter */
    bool_t             dvu_setup;                  /* DVU setup complete flag */
    uint32_t           dvu_mute_stat;              /* DVU mute status */
    scux_ssif_info_t   *p_ssif_info1;              /* SSIF channel information1  */
    scux_ssif_info_t   *p_ssif_info2;              /* SSIF channel information2  */
    scux_ssif_info_t   *p_ssif_info3;              /* SSIF channel information3  */
    void               (*p_flush_callback)(int_t ercd);   /* Call back pointer for flush stop */
    bool_t             first_tx_flag;              /* First write data transfer flag */
    bool_t             first_rx_flag;              /* First read data transfer flag */
    bool_t             first_ramp_flag;            /* First ramp setting flag */
    volatile uint8_t   *p_tx_dummy_data;           /* Write dummy data buffer pointer */
    volatile uint8_t   *p_rx_dummy_data;           /* Read dummy data buffer pointer */
    uint32_t           flush_stop_size;            /* Needed transfer size for flush stop */
    uint32_t           fifo_size;                  /* Fifo size */
    uint32_t           fifo_req_size;              /* Fifo request size */
    uint32_t           last_dummy_size;            /* Last dummy data size for flush stop */
    uint32_t           tx_fifo_total_size;         /* Total data size of write request size / FIFO size */
    uint32_t           rx_fifo_total_size;         /* Total data size of read request size / FIFO size */
    bool_t             tx_dummy_run_flag;          /* Dummy data transfer flag */
    bool_t             cancel_operate_flag;        /* indicate cancel operation */
    bool_t             restart_ramp_flag;          /* ramp restart flag after cancel operation */
    uint32_t           input_rate;                 /* Input rate */
    uint32_t           output_rate;                /* Output rate */
    uint32_t           futsel_cim_value;           /* FUTSEL_CIM register value (write only) */
    int_t              err_stat_backup;            /* SCUX HW error status backup */
} scux_info_ch_t;

/* Information of Driver */
typedef struct
{
    scux_stat_drv_t    drv_stat;              /* DRV status */
    scux_info_ch_t     info_ch[SCUX_CH_NUM];  /* CH status */
    scux_shared_info_t shared_info;           /* shared resource information */
} scux_info_drv_t;

/***********************************************************************************
 Function Prototypes
***********************************************************************************/

scux_info_drv_t *SCUX_GetDrvInstance(void);
scux_info_ch_t  *SCUX_GetDrvChInfo(const int_t channel);
scux_ssif_info_t *SCUX_GetSsifChInfo(const int_t channel);
#if(1) /* mbed */
int_t SCUX_InitializeOne(const int_t channel, const scux_channel_cfg_t * const p_scux_init_param);
void SCUX_UnInitializeOne(const int_t channel);
#endif /* end mbed */
int_t SCUX_Initialize(const scux_channel_cfg_t * const p_scux_init_param);
void SCUX_UnInitialize(void);
int_t SCUX_OpenChannel(const int_t channel, const int_t flags);
int_t SCUX_CloseChannel(const int_t channel);
int_t SCUX_IoctlTransStart(const int_t channel);
int_t SCUX_IoctlFlushStop(const int_t channel, void (* const callback)(int_t ercd));
int_t SCUX_IoctlClearStop(const int_t channel);
void SCUX_IoctlSetRoute(const int_t channel, const scux_route_t route);
int_t SCUX_IoctlSetPinClk(const int_t channel, const scux_ssif_pin_clk_t * const p_pin_clk_param);
int_t SCUX_IoctlSetPinMode(const int_t channel, const scux_ssif_pin_mode_t * const p_pin_mode_param);
void SCUX_IoctlSetSrcCfg(const int_t channel, const scux_src_cfg_t * const p_src_param);
void SCUX_IoctlSetDvuCfg(const int_t channel, const scux_dvu_cfg_t * const p_dvu_param);
int_t SCUX_IoctlSetDvuDigiVol(const int_t channel, const scux_dvu_digi_vol_t * const p_digi_vol_param);
int_t SCUX_IoctlSetDvuRampVol(const int_t channel, const scux_dvu_ramp_vol_t * const p_ramp_vol_param);
int_t SCUX_IoctlSetDvuZerocrossMute(const int_t channel, const scux_zc_mute_t * const p_zc_mute_param);
int_t SCUX_IoctlSetStopMute(const int_t channel, const uint32_t audio_channel);
int_t SCUX_IoctlSetMixCfg(const scux_mix_cfg_t * const p_mix_param);
int_t SCUX_IoctlSetMixVol(const int_t channel, const uint32_t mix_vol);
int_t SCUX_IoctlSetSsifCfg(const scux_ssif_cfg_t *const p_ssif_param);
void SCUX_IoctlGetWriteStat(const int_t channel, uint32_t * const p_write_stat);
void SCUX_IoctlGetReadStat(const int_t channel, uint32_t * const p_read_stat);
void SCUX_IoctlGetDvuStat(const int_t channel, uint32_t * const p_dvu_stat);
void SCUX_IoctlGetMuteStat(const int_t channel, uint32_t * const p_mute_stat);
void SCUX_IoctlGetMixStat(const int_t channel, uint32_t * const p_mix_stat);
int_t SCUX_CheckParam(scux_info_ch_t * const p_scux_info_ch);
int_t SCUX_FlushWriteStart(scux_info_ch_t * const p_scux_info_ch);
void SCUX_InitHw(scux_info_ch_t * const p_scux_info_ch);
void SCUX_SetupSsif(const scux_info_ch_t * const p_scux_info_ch);
void SCUX_SetupSrc(scux_info_ch_t * const p_scux_info_ch);
void SCUX_SetupDvu(scux_info_ch_t * const p_scux_info_ch);
int_t SCUX_SetupDma(scux_info_ch_t * const p_scux_info_ch);
void  SCUX_SyncStartHw(const scux_info_ch_t * const p_scux_info_ch);
void SCUX_AsyncStartHw(scux_info_ch_t * const p_scux_info_ch);
int_t SCUX_CopyWriteStart(scux_info_ch_t * const p_scux_info_ch, AIOCB * const p_write_aio);
int_t SCUX_DirectWriteStart(scux_info_ch_t * const p_scux_info_ch, AIOCB * const p_write_aio);
int_t SCUX_CopyReadStart(scux_info_ch_t * const p_scux_info_ch, AIOCB * const p_read_aio);
int_t SCUX_CopyCancelSpecific(scux_info_ch_t * const p_scux_info_ch, AIOCB * const p_cancel_aio);
int_t SCUX_DirectCancelSpecific(scux_info_ch_t * const p_scux_info_ch, AIOCB * const p_cancel_aio);
int_t SCUX_CopyCancelAll(scux_info_ch_t * const p_scux_info_ch);
int_t SCUX_DirectCancelAll(scux_info_ch_t * const p_scux_info_ch);
void SCUX_AdjustAccessFifo(scux_info_ch_t * const p_scux_info_ch, const uint32_t tx_remain_size, const uint32_t rx_remain_size);
void  SCUX_SyncStartHw(const scux_info_ch_t * const p_scux_info_ch);
void SCUX_AsyncStartHw(scux_info_ch_t * const p_scux_info_ch);
void  SCUX_SyncStopHw(const scux_info_ch_t * const p_scux_info_ch);
void  SCUX_AsyncStopHw(scux_info_ch_t * const p_scux_info_ch);
void SCUX_SetDigiVolRegister(const scux_info_ch_t * const p_scux_info_ch);
void SCUX_SetRampVolRegister(scux_info_ch_t * const p_scux_info_ch);
void SCUX_SetZerocrossMuteRegister(const scux_info_ch_t * const p_scux_info_ch);
void SCUX_SetMixVolRegister(const int_t channel);
int_t SCUX_CheckSsifClockDiv(const scux_info_ch_t * const p_scux_info_ch, const uint32_t ssif_ch_num);
void SCUX_SetupSsifGpio(const scux_ssif_ch_num_t ssif_ch);
size_t SCUX_StrNLen(const char_t* p_str, size_t maxlen);

#endif /* SCUX_H */
