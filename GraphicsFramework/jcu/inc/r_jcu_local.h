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
* Copyright (C) 2012 - 2014 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/******************************************************************************
* $FileName: jcu_local.h $
* $Module: JCU $ $PublicVersion: 1.00 $ (=JCU_VERSION)
* $Rev: 35 $
* $Date:: 2014-02-26 13:18:53 +0900#$
* Description  : JCU driver local parameter
******************************************************************************/

#ifndef JCU_LOCAL_H
#define JCU_LOCAL_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_jcu_api.h"
#include    "r_jcu_user.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
/******************************************************************************
Typedef definitions
******************************************************************************/

typedef struct st_jcu_i_lock_t   jcu_i_lock_t;  /*[jcu_i_lock_t]*/

/*! [jcu_internal_information_t] Internal Information for this driver */
typedef struct {
    jcu_codec_status_t             Codec;
    bool_t                         IsCountMode;
    jcu_int_detail_errors_t        ErrorFilter;  /* 1=Enable */

    jcu_async_status_t             AsyncStatus;
    r_ospl_caller_t                InterruptCallbackCaller;
    jcu_i_lock_t                  *I_Lock;
    const r_ospl_i_lock_vtable_t  *I_LockVTable;
    bool_t                         Is_I_LockMaster;
    r_ospl_async_t                *AsyncForFinalize;
} jcu_internal_information_t;

/*[jcu_i_lock_t]*/
struct st_jcu_i_lock_t {
    bool_t        IsLock;
    bool_t        IsRequestedFinalize;
};


/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
jcu_internal_information_t  *R_JCU_GetInternalInformation(void);
jcu_i_lock_t    *R_JCU_GetILockObject(void);
void             R_JCU_I_LOCK_Finalize(void);
jcu_errorcode_t  R_JCU_TerminateStep2(void);

#ifdef      JCU_PARAMETER_CHECK
jcu_errorcode_t JCU_ParaCheckStart(void);
jcu_errorcode_t JCU_ParaCheckSetCountMode(const jcu_count_mode_param_t  *const buffer);
jcu_errorcode_t JCU_ParaCheckSelectCodec(const jcu_codec_t codec);
jcu_errorcode_t JCU_ParaCheckSetCallbackFunc(const uintptr_t type);
jcu_errorcode_t JCU_ParaCheckSetDecodeParam(const jcu_decode_param_t *const decode, const jcu_buffer_param_t *const buffer);
jcu_errorcode_t JCU_ParaCheckGetImageInfo(const jcu_image_info_t *const buffer);
jcu_errorcode_t JCU_ParaCheckGetErrorInfo(const jcu_detail_error_t *const errorCode);
jcu_errorcode_t JCU_ParaCheckSetQuantizationTbl(const jcu_table_no_t tableNo);
jcu_errorcode_t JCU_ParaCheckSetHuffmanTable(const jcu_table_no_t tableNo, const jcu_huff_t type);
jcu_errorcode_t JCU_ParaCheckEncodeParam(const jcu_encode_param_t *const encode, const jcu_buffer_param_t *const buffer);
jcu_errorcode_t JCU_CheckInterruptSource(const uint32_t interruptKind);
#endif /* JCU_PARAMETER_CHECK */

jcu_errorcode_t JCU_SetRegisterForInitialize(void);
void JCU_SetRegisterForCodec(const jcu_codec_t codec);
void JCU_SetRegisterForStart(const jcu_int_detail_error_t error_filter);
void JCU_SetRegisterForReset(void);
void JCU_SetRegisterForSetCountMode(const jcu_count_mode_param_t *const buffer);
void JCU_SetRegisterForContinue(const jcu_continue_type_t type);
void JCU_SetRegisterForSetDecodePrm(const jcu_decode_param_t *const decode, const jcu_buffer_param_t *const buffer);
void JCU_SetRegisterForSetQtTable(const jcu_table_no_t tableNo, const uint8_t *const  table);
void JCU_SetRegisterForSetHuffTbl(const jcu_table_no_t tableNo, const jcu_huff_t type, const uint8_t *const table);
void JCU_SetRegisterForSetEncodePrm(const jcu_encode_param_t *const encode, const jcu_buffer_param_t *const buffer);
void JCU_SetRegisterForSetPause(const bool_t is_pause, const jcu_int_detail_errors_t filter);
void JCU_GetRegisterForGetImageInfo(jcu_image_info_t *const buffer);
void JCU_GetEncodedSize(size_t *const out_Size);
void JCU_ClearInterruptFlag(void);


/* ->SEC M1.2.2(QAC-1259) */
/*    If "Value" is signed, this is depend on CPU bit width. */
/*    But driver code is no problem. */
/*    This expects 32bit CPU. */
/* ->SEC R3.6.2(QAC-3345) */
/*    Volatile access at left of "=" and right of "=". */
/*    But this is not depend on compiler spcifications. */
/* ->MISRA 19.12 ->MISRA 19.13 ->SEC M5.1.2 (1) */
#define R_SET_REG_BIT_FIELD( Register, BitName, Value ) \
        ( (Register) = (BitName##_TYPE)( ( (uint32_t)(Register) & (uint32_t)(~(BitName##_MASK) ) ) |\
        ( (uint32_t)( (uint32_t)(Value) << (BitName##_SHIFT) ) & (uint32_t)(BitName##_MASK) ) ) )
#define R_GET_REG_BIT_FIELD( Register, BitName ) \
        ( (BitName##_TYPE)( ( ((uint32_t)(Register)) & (uint32_t)(BitName##_MASK) ) >> (BitName##_SHIFT) ) )
/* <-SEC M1.2.3(QAC-1259) */
/* <-SEC R3.6.2(QAC-3345) */
/* <-MISRA 19.12 <-MISRA 19.13 <-SEC M5.1.2 (1) */

/* Value of register */
/* 1bit(Flag) */
#define REG_F_CLR (0u)
#define REG_F_SET (1u)

/* Bit field name (Register name) */

/* SRST(CPG.SWRSTCR2) */
#define SRST_MASK       (0x00000002u)
#define SRST_SHIFT      (1u)
#define SRST_TYPE       uint8_t


/*DSP(JCU.JCMOD) */
#define DSP_MASK        (0x00000008u)
#define DSP_SHIFT       (3u)
#define DSP_TYPE        uint8_t

/*REDU(JCU.JCMOD) */
#define REDU_MASK       (0x00000007u)
#define REDU_SHIFT      (0u)
#define REDU_TYPE       uint8_t


/*BRST(JCU.JCCMD) */
#define BRST_MASK       (0x00000080u)
#define BRST_SHIFT      (7u)
#define BRST_TYPE       uint8_t

/*JEND(JCU.JCCMD) */
#define JEND_MASK       (0x00000004u)
#define JEND_SHIFT      (2u)
#define JEND_TYPE       uint8_t

/*JRST(JCU.JCCMD) */
#define JRST_MASK       (0x00000002u)
#define JRST_SHIFT      (1u)
#define JRST_TYPE       uint8_t

/*JSRT(JCU.JCCMD) */
#define JSRT_MASK       (0x00000001u)
#define JSRT_SHIFT      (0u)
#define JSRT_TYPE       uint8_t


/*QT3(JCU.JCQTN) */
#define QT3_MASK        (0x00000030u)
#define QT3_SHIFT       (4u)
#define QT3_TYPE        uint8_t

/*QT2(JCU.JCQTN) */
#define QT2_MASK        (0x0000000Cu)
#define QT2_SHIFT       (2u)
#define QT2_TYPE        uint8_t

/*QT1(JCU.JCQTN) */
#define QT1_MASK        (0x00000003u)
#define QT1_SHIFT       (0u)
#define QT1_TYPE        uint8_t


/*HTA3(JCU.JCHTN) */
#define HTA3_MASK       (0x00000020u)
#define HTA3_SHIFT      (5u)
#define HTA3_TYPE       uint8_t

/*HTD3(JCU.JCHTN) */
#define HTD3_MASK       (0x00000010u)
#define HTD3_SHIFT      (4u)
#define HTD3_TYPE       uint8_t

/*HTA2(JCU.JCHTN) */
#define HTA2_MASK       (0x00000008u)
#define HTA2_SHIFT      (3u)
#define HTA2_TYPE       uint8_t

/*HTD2(JCU.JCHTN) */
#define HTD2_MASK       (0x00000004u)
#define HTD2_SHIFT      (2u)
#define HTD2_TYPE       uint8_t

/*HTA1(JCU.JCHTN) */
#define HTA1_MASK       (0x00000002u)
#define HTA1_SHIFT      (1u)
#define HTA1_TYPE       uint8_t

/*HTD1(JCU.JCHTN) */
#define HTD1_MASK       (0x00000001u)
#define HTD1_SHIFT      (0u)
#define HTD1_TYPE       uint8_t


/*INT7(JCU.JINTE0) Error of restart interval data. JINTE0.INT7 -> JINTS0.INS5 */
#define INT7_MASK       (0x00000080u)
#define INT7_SHIFT      (7u)
#define INT7_TYPE       uint8_t

/*INT6(JCU.JINTE0) Error of Segment total data. JINTE0.INT6 -> JINTS0.INS5 */
#define INT6_MASK       (0x00000040u)
#define INT6_SHIFT      (6u)
#define INT6_TYPE       uint8_t

/*INT5(JCU.JINTE0) Error of MCU block data. JINTE0.INT5 -> JINTS0.INS5 */
#define INT5_MASK       (0x00000020u)
#define INT5_SHIFT      (5u)
#define INT5_TYPE       uint8_t

/*INT3(JCU.JINTE0) */
#define INT3_MASK       (0x00000008u)
#define INT3_SHIFT      (3u)
#define INT3_TYPE       uint8_t


/*INS6(JCU.JINTS0) End Core Only */
#define INS6_MASK       (0x00000040u)
#define INS6_SHIFT      (6u)
#define INS6_TYPE       uint8_t

/*INS5(JCU.JINTS0) Decode Error */
#define INS5_MASK       (0x00000020u)
#define INS5_SHIFT      (5u)
#define INS5_TYPE       uint8_t

/*INS3(JCU.JINTS0) Get Image Information */
#define INS3_MASK       (0x00000008u)
#define INS3_SHIFT      (3u)
#define INS3_TYPE       uint8_t


/*ERR(JCU.JCDERR) */
#define ERR_MASK        (0x0000000Fu)
#define ERR_SHIFT       (0u)
#define ERR_TYPE        uint8_t


/*JOUTRINI(JCU.JIFECNT) */
#define JOUTRINI_MASK   (0x00004000u)
#define JOUTRINI_SHIFT  (14u)
#define JOUTRINI_TYPE   uint32_t

/*JOUTRCMD(JCU.JIFECNT) */
#define JOUTRCMD_MASK   (0x00002000u)
#define JOUTRCMD_SHIFT  (13u)
#define JOUTRCMD_TYPE   uint32_t

/*JOUTC(JCU.JIFECNT) */
#define JOUTC_MASK      (0x00001000u)
#define JOUTC_SHIFT     (12u)
#define JOUTC_TYPE      uint32_t

/*JOUTSWAP(JCU.JIFECNT) */
#define JOUTSWAP_MASK   (0x00000700u)
#define JOUTSWAP_SHIFT  (8u)
#define JOUTSWAP_TYPE   uint32_t

/*DINRINI(JCU.JIFECNT) */
#define DINRINI_MASK    (0x00000040u)
#define DINRINI_SHIFT   (6u)
#define DINRINI_TYPE    uint32_t

/*DINRCMD(JCU.JIFECNT) */
#define DINRCMD_MASK    (0x00000020u)
#define DINRCMD_SHIFT   (5u)
#define DINRCMD_TYPE    uint32_t

/*DINLC(JCU.JIFECNT) */
#define DINLC_MASK      (0x00000010u)
#define DINLC_SHIFT     (4u)
#define DINLC_TYPE      uint32_t

/*DINSWAP(JCU.JIFECNT) */
#define DINSWAP_MASK    (0x00000007u)
#define DINSWAP_SHIFT   (0u)
#define DINSWAP_TYPE    uint32_t


/*ESMW(JCU.JIFESOFST) */
#define ESMW_MASK       (0x00007FFFu)
#define ESMW_SHIFT      (0u)
#define ESMW_TYPE       uint32_t


/*LINES(JCU.JIFESLC, JCU.JIFDDLC) */
#define LINES_MASK      (0x0000FFFFu)
#define LINES_SHIFT     (0u)
#define LINES_TYPE      uint32_t
#define LINES_CLEAR     (0u)


/*JDATAS(JCU.JIFESLC, JCU.JIFDSDC, JCU.JIFEDDC) */
#define JDATAS_MASK     (0x0000FFFFu)
#define JDATAS_SHIFT    (0u)
#define JDATAS_TYPE     uint32_t
#define JDATAS_CLEAR    (0u)


/*VINTER(JCU.JIFDCNT) */
#define VINTER_MASK     (0x3C000000u)
#define VINTER_SHIFT    (28u)
#define VINTER_TYPE     uint32_t

/*HINTER(JCU.JIFDCNT) */
#define HINTER_MASK     (0x0C000000u)
#define HINTER_SHIFT    (26u)
#define HINTER_TYPE     uint32_t

/*OPF(JCU.JIFDCNT) */
#define OPF_MASK        (0x03000000u)
#define OPF_SHIFT       (24u)
#define OPF_TYPE        uint32_t

/*JINRINI(JCU.JIFDCNT) */
#define JINRINI_MASK    (0x00004000u)
#define JINRINI_SHIFT   (14u)
#define JINRINI_TYPE    uint32_t

/*JINRCMD(JCU.JIFDCNT) */
#define JINRCMD_MASK    (0x00002000u)
#define JINRCMD_SHIFT   (13u)
#define JINRCMD_TYPE    uint32_t

/*JINC(JCU.JIFDCNT) */
#define JINC_MASK       (0x00001000u)
#define JINC_SHIFT      (12u)
#define JINC_TYPE       uint32_t

/*JINSWAP(JCU.JIFDCNT) */
#define JINSWAP_MASK    (0x00000700u)
#define JINSWAP_SHIFT   (8u)
#define JINSWAP_TYPE    uint32_t

/*DOUTRINI(JCU.JIFDCNT) */
#define DOUTRINI_MASK   (0x00000040u)
#define DOUTRINI_SHIFT  (6u)
#define DOUTRINI_TYPE   uint32_t

/*DOUTRCMD(JCU.JIFDCNT) */
#define DOUTRCMD_MASK   (0x00000020u)
#define DOUTRCMD_SHIFT  (5u)
#define DOUTRCMD_TYPE   uint32_t

/*DOUTLC(JCU.JIFDCNT) */
#define DOUTLC_MASK     (0x00000010u)
#define DOUTLC_SHIFT    (4u)
#define DOUTLC_TYPE     uint32_t

/*DOUTSWAP(JCU.JIFDCNT) */
#define DOUTSWAP_MASK   (0x00000007u)
#define DOUTSWAP_SHIFT  (0u)
#define DOUTSWAP_TYPE   uint32_t


/*DDMW(JCU.JIFDDOFST) */
#define DDMW_MASK       (0x00007FFFu)
#define DDMW_SHIFT      (0u)
#define DDMW_TYPE       uint32_t


/*ALPHA(JCU.JIFDADT) */
#define ALPHA_MASK      (0x000000FFu)
#define ALPHA_SHIFT     (0u)
#define ALPHA_TYPE      uint32_t


/*CBTEN(JCU.JINTE1) */
#define CBTEN_MASK      (0x00000040u)
#define CBTEN_SHIFT     (6u)
#define CBTEN_TYPE      uint32_t

/*DINLEN(JCU.JINTE1) */
#define DINLEN_MASK     (0x00000020u)
#define DINLEN_SHIFT    (5u)
#define DINLEN_TYPE     uint32_t

/*JOUTEN(JCU.JINTE1) */
#define JOUTEN_MASK     (0x00000010u)
#define JOUTEN_SHIFT    (4u)
#define JOUTEN_TYPE     uint32_t

/*DBTEN(JCU.JINTE1) */
#define DBTEN_MASK      (0x00000004u)
#define DBTEN_SHIFT     (2u)
#define DBTEN_TYPE      uint32_t

/*JINEN(JCU.JINTE1) */
#define JINEN_MASK      (0x00000002u)
#define JINEN_SHIFT     (1u)
#define JINEN_TYPE      uint32_t

/*DOUTLEN(JCU.JINTE1) */
#define DOUTLEN_MASK    (0x00000001u)
#define DOUTLEN_SHIFT   (0u)
#define DOUTLEN_TYPE    uint32_t


/*CBTF(JCU.JINTS1) JINTE1.CBTEN -> JINTS1.CBTF */
#define CBTF_MASK       (0x00000040u)
#define CBTF_SHIFT      (6u)
#define CBTF_TYPE       uint32_t

/*DINLF(JCU.JINTS1) Input Data for Encoding */
#define DINLF_MASK      (0x00000020u)
#define DINLF_SHIFT     (5u)
#define DINLF_TYPE      uint32_t

/*JOUTF(JCU.JINTS1) Output Data for Encoding */
#define JOUTF_MASK      (0x00000010u)
#define JOUTF_SHIFT     (4u)
#define JOUTF_TYPE      uint32_t

/*DBTF(JCU.JINTS1) JINTE1.DBTEN -> JINTS1.DBTF */
#define DBTF_MASK       (0x00000004u)
#define DBTF_SHIFT      (2u)
#define DBTF_TYPE       uint32_t

/*JINF(JCU.JINTS1) Input Data for Decoding */
#define JINF_MASK       (0x00000002u)
#define JINF_SHIFT      (1u)
#define JINF_TYPE       uint32_t

/*DOUTLF(JCU.JINTS1) Output Data for Decoding */
#define DOUTLF_MASK     (0x00000001u)
#define DOUTLF_SHIFT    (0u)
#define DOUTLF_TYPE     uint32_t


/*DINYCHG(JCU.JIFESVSZ) */
#define DINYCHG_MASK     (0x00008000u)
#define DINYCHG_SHIFT    (15u)
#define DINYCHG_TYPE     uint32_t

/*DOUTYCHG(JCU.JIFESHSZ) */
#define DOUTYCHG_MASK     (0x00008000u)
#define DOUTYCHG_SHIFT    (15u)
#define DOUTYCHG_TYPE     uint32_t

/* Others */
#define JCU_JINTS1_ALL  ( CBTF_MASK | DINLF_MASK | JOUTF_MASK | DBTF_MASK | JINF_MASK | DOUTLF_MASK )
#define SHIFT_16BITS    (16u)                               /* 16bit-SHIFT */
#define SHIFT_8BITS     (8u)                                /* 8bit-SHIFT */
#define JCU_SHIFT_JINTS1 (8)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* JCU_LOCAL_H */
