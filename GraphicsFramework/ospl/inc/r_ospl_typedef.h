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
* Copyright (C) 2012 - 2015 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**
* @file  r_ospl_typedef.h
* @brief   OS Porting Layer. Data types.
*
* $Module: OSPL $ $PublicVersion: 0.90 $ (=R_OSPL_VERSION)
* $Rev: 35 $
* $Date:: 2014-04-15 21:38:18 +0900#$
*/

#ifndef R_OSPL_TYPEDEF_H
#define R_OSPL_TYPEDEF_H


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#ifdef  __GNUC__
#include  <stdint.h>
#include  <stdbool.h>
#endif
#include  "Project_Config.h"
#include  "r_typedefs.h"
#include  "mcu_interrupts_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************
Typedef definitions
******************************************************************************/

typedef struct st_r_ospl_async_t          r_ospl_async_t;
typedef struct st_r_ospl_async_status_t   r_ospl_async_status_t;
typedef struct st_r_ospl_flag32_t         r_ospl_flag32_t;
typedef struct st_r_ospl_interrupt_t      r_ospl_interrupt_t;
typedef struct st_r_ospl_irq_mask_t       r_ospl_irq_mask_t;
typedef struct st_r_ospl_memory_spec_t    r_ospl_memory_spec_t;
typedef struct st_r_ospl_ftimer_spec_t    r_ospl_ftimer_spec_t;
typedef struct st_r_ospl_table_t          r_ospl_table_t;
typedef struct st_r_ospl_i_lock_vtable_t  r_ospl_i_lock_vtable_t;
typedef struct st_r_ospl_caller_t         r_ospl_caller_t;


/**
* @typedef  errnum_t
* @brief  Error number
*/
typedef int_fast32_t   errnum_t;


/**
* @typedef  bit_flags_fast32_t
* @brief  Bit flags as "uint_fast32_t"
*/
typedef uint_fast32_t  bit_flags_fast32_t;


/**
* @typedef  bit_flags32_t
* @brief  Bit flags as "uint32_t"
*/
typedef uint32_t       bit_flags32_t;


/**
* @typedef  bit_flags16_t
* @brief  Bit flags as "uint16_t"
*/
typedef uint16_t       bit_flags16_t;


/**
* @typedef  bit_flags8_t
* @brief  Bit flags as "uint8_t"
*/
typedef uint8_t       bit_flags8_t;


/**
* @typedef  byte_t
* @brief  Byte type
*/
typedef uint8_t        byte_t;


/**
* @typedef  ssize_t
* @brief  Signed size type. This is a POSIX specification.
*/
#if ! defined( __GNUC__ )  ||  defined( __CC_ARM )
typedef int  ssize_t;
#endif


/***********************************************************************
* Section: Global
************************************************************************/
/* Start of a part of CMSIS : Copy from cmsys_os.h */
/* This is for not calling CMSIS function */


/**
* @typedef  osMailQId
* @brief  osMailQId
*/
#ifdef IS_CMSIS_USED
typedef struct os_mailQ_cb *osMailQId;
#endif


/**
* @struct  r_ospl_rtx_osMailQDef_t
* @brief  RTX:osMailQDef_t
*/
#ifdef IS_CMSIS_USED
typedef struct r_ospl_rtx_os_mailQ_def  {
    uint32_t                queue_sz;    ///< number of elements in the queue
    uint32_t                 item_sz;    ///< size of an item
    void                       *pool;    ///< memory array for mail
} r_ospl_rtx_osMailQDef_t;
#endif


/* Section: Global */
/**
* @def  r_ospl_rtx_osMailQDef
* @brief  RTX:osMailQDef
* @par Parameters
*    None
* @return  None.
*/
#ifdef IS_CMSIS_USED
#if defined (osObjectsExternal)  // object is external
#define r_ospl_rtx_osMailQDef(name, queue_sz, type) \
extern const osMailQDef_t os_mailQ_def_##name
#else                            // define the object
#define r_ospl_rtx_osMailQDef(name, queue_sz, type) \
uint32_t os_mailQ_q_##name[4+(queue_sz)] = { 0 }; \
uint32_t os_mailQ_m_##name[3+((sizeof(type)+3)/4)*(queue_sz)]; \
void *   os_mailQ_p_##name[2] = { (os_mailQ_q_##name), os_mailQ_m_##name }; \
const r_ospl_rtx_osMailQDef_t os_mailQ_def_##name =  \
{ (queue_sz), sizeof(type), (os_mailQ_p_##name) }
#endif
#endif


/**
* @def  osMailQ
* @brief  osMailQ
* @par Parameters
*    None
* @return  None.
*/
#ifdef IS_CMSIS_USED
#define osMailQ(name)  \
&os_mailQ_def_##name
#endif


/***********************************************************************
*  Section: Global
************************************************************************/
/* End of a part of CMSIS */


/**
* @struct  r_ospl_thread_id_t
* @brief  Pointer to a thread
*/
#ifdef IS_CMSIS_USED
typedef void * /*<r_ospl_thread_def_t*>*/  r_ospl_thread_id_t;
#define  R_OSPL_THREAD_INTERRUPT  ((void*) 0 )
#endif


/* Section: Global */
/**
* @brief   The function type of interrupt callback
*
* @param   InterruptSource Source of interrupt
* @param   Caller Driver's internal parameters about interrupt operations
* @return  Error code.  If there is no error, the return value is 0. The value set to "r_ospl_async_t::ReturnValue"
*
* @par Description
*    This is type of the interrupt callback function running in the
*    interrupt context and called from the interrupt handler.
*
*    It is possible to replace to application defined interrupt callback function
*    by setting to "r_ospl_async_t::InterruptCallback". But it is usually not
*    necessary to replace.
*
*    As interrupt callback function, the default interrupt callback function
*    provided from the driver is used. It is unusual to use application defined
*    interrupt callback function.
*    Write the response code of the interrupt (event driven code) next to the
*    code calling @ref R_OSPL_EVENT_Wait function
*
*    Whether the asynchronous operation was ended is possible to know whether
*    the variable of "r_ospl_async_state_t" type referred from
*    @ref R_DRIVER_GetAsyncStatus function is set to @ref R_OSPL_RUNNABLE value.
*
*    It is not necessary to write the code of interrupt return (IRET) in
*    the interrupt callback function. The interrupt handlers calling interrupt
*    callback function calls IRET, if necessary.
*
*    It is not possible to divide interrupt callback functions by the kind of
*    interrupt. Alternatively, it is possible to write operations in interrupt
*    handlers calling interrupt callback function in porting layer under the driver.
*    There are interrupt handlers by each interrupt numbers.
*
*    It is possible to signal any event from application defined interrupt
*    @par callback function. But it is necessary to do following operations
*/
typedef errnum_t  (* r_ospl_callback_t )( const r_ospl_interrupt_t *InterruptSource, const r_ospl_caller_t *Caller );


/**
* @enum   r_ospl_async_state_t
* @brief  Asynchronous State
*
*    - R_OSPL_UNINITIALIZED - 0
*    - R_OSPL_RUNNABLE - 1, Runnable or Waitable
*    - R_OSPL_RUNNING - 2, Running  or Waiting
*    - R_OSPL_INTERRUPTING - 3
*    - R_OSPL_INTERRUPTED - 4
*/
typedef enum {
    R_OSPL_UNINITIALIZED = 0,
    R_OSPL_RUNNABLE,
    R_OSPL_RUNNING,
    R_OSPL_INTERRUPTING,
    R_OSPL_INTERRUPTED
} r_ospl_async_state_t;


/**
* @enum   r_ospl_async_type_t
* @brief  Asynchronous State
*
*    - R_OSPL_ASYNC_TYPE_NORMAL - 1
*    - R_OSPL_ASYNC_TYPE_FINALIZE - 2, Asynchronous finalizing
*/
typedef enum {
    R_OSPL_ASYNC_TYPE_NORMAL   = 1,
    R_OSPL_ASYNC_TYPE_FINALIZE = 2
} r_ospl_async_type_t;


/**
* @struct  r_ospl_block_t
* @brief  Block in queue
*
* @par Description
*    Member variables should not be accessed.
*/
typedef struct st_r_ospl_block_t  r_ospl_block_t;
struct st_r_ospl_block_t {
    r_ospl_block_t  *Next;
};


/**
* @struct  r_ospl_queue_status_t
* @brief  r_ospl_queue_status_t
*/
typedef struct st_r_ospl_queue_status_t {

    /** UsedCount */
    int_fast32_t  UsedCount;

    /** MaxCount */
    int_fast32_t  MaxCount;
} r_ospl_queue_status_t;


/**
* @struct  r_ospl_queue_def_t
* @brief  r_ospl_queue_def_t
* @par Description
*    Member variables should not be accessed.
*/
typedef struct st_r_ospl_queue_def_t {
    osMailQId                       MailQId;
    r_ospl_queue_status_t           PublicStatus;
    const r_ospl_rtx_osMailQDef_t  *MailQDef;
} r_ospl_queue_def_t;


/**
* @struct  r_ospl_queue_t
* @brief  r_ospl_queue_t
* @par Description
*    Member variables should not be accessed.
*/
typedef r_ospl_queue_def_t  r_ospl_queue_t;


/* Section: Global */
/**
* @def  R_OSPL_QUEUE_DEF
* @brief  Defines attributes of queue and work area.
* @param   Name Thread name. Do not bracket by ""
* @param   MaxCount Max count of elements in the queue
* @param   Type Output
* @return  None.
*
* @par Description
*    It is not possible to use this macro in the library.
*    If implement of OSPL was changed, the library must be recompiled.
*/
#define  R_OSPL_QUEUE_DEF( Name, MaxCount, Type ) \
	r_ospl_rtx_osMailQDef( Name, MaxCount, Type ); \
	r_ospl_queue_def_t  Name##_OSPL_DEF = { NULL, 0, (MaxCount), osMailQ( Name ) }


/**
* @def  R_OSPL_QUEUE
* @brief  Returns initial attributes of queue and work area.
* @param   Name Queue's name. Do not bracket by ""
* @return  None.
*
* @par Description
*    It is not possible to use this macro in the library.
*    If implement of OSPL was changed, the library must be recompiled.
*/
#define  R_OSPL_QUEUE( Name )  (&Name##_OSPL_DEF)


/**
* @typedef  r_ospl_flush_t
* @brief  r_ospl_flush_t
*/
typedef bit_flags_fast32_t  r_ospl_flush_t;
#define  /*<uint_fast32_t>*/  R_OSPL_FLUSH_INVALIDATE                0u
#define  /*<uint_fast32_t>*/  R_OSPL_FLUSH_WRITEBACK_INVALIDATE      2u
#define  /*<uint_fast32_t>*/  R_OSPL_FLUSH_WRITEBACK_INVALIDATE_2ND  8u


/**
* @struct  r_ospl_flag32_t
* @brief  This is the type of flags having 32bit
*
* @par Description
*    Member variables should not be accessed.
*/
struct st_r_ospl_flag32_t {
    volatile uint32_t  Flags;
};


/**
* @struct  r_ospl_interrupt_t
* @brief  Structure related to interrupt source. e.g. interrupt number
*/
struct st_r_ospl_interrupt_t {

    /** <bsp_int_src_t> */
    bsp_int_src_t  IRQ_Num;

    /** ChannelNum */
    int_fast32_t   ChannelNum;

    /** Type */
    int_fast32_t   Type;

    /** Delegate */
    void  *Delegate;
};


/**
* @struct  r_ospl_async_status_t
* @brief  Structure of driver's status and interrupt status defined by OSPL
*/
struct st_r_ospl_async_status_t {

    /** <r_ospl_async_state_t> */
    volatile r_ospl_async_state_t  State;

    /** IsEnabledInterrupt */
    volatile bool_t  IsEnabledInterrupt;

    /** InterruptEnables */
    volatile r_ospl_flag32_t  InterruptEnables;

    /** InterruptFlags */
    volatile r_ospl_flag32_t  InterruptFlags;

    /** r_ospl_flag32_t < <r_ospl_cancel_flag_t> > */
    volatile r_ospl_flag32_t  CancelFlags;  /*<r_ospl_cancel_flag_t>*/
#if  R_OSPL_IS_PREEMPTION
    union {

        /** LockOwner */
        volatile r_ospl_thread_id_t  Thread;

        /** LockOwner */
        volatile void  *Context;
    } LockOwner;
#endif
};


/**
* @struct  r_ospl_async_t
* @brief  Setting of notifications
*/
struct st_r_ospl_async_t {

    /** Flags */
    bit_flags_fast32_t  Flags;

    /** Delegate */
    void  *Delegate;

    /** A_Thread */
    r_ospl_thread_id_t  A_Thread;

    /** A_EventValue */
    uint32_t  A_EventValue;  /* QAC 4130 */

    /** I_Thread */
    r_ospl_thread_id_t  I_Thread;

    /** I_EventValue */
    uint32_t  I_EventValue;  /* QAC 4130 */

    /** InterruptCallback */
    r_ospl_callback_t  InterruptCallback;

    /** ReturnValue */
    errnum_t  ReturnValue;
};

enum {
    R_F_OSPL_A_Thread          = 0x0001,
    R_F_OSPL_A_EventValue      = 0x0002,
    R_F_OSPL_I_Thread          = 0x0004,
    R_F_OSPL_I_EventValue      = 0x0008,
    R_F_OSPL_InterruptCallback = 0x0010,
    R_F_OSPL_Delegate          = 0x0080
};


/* Section: Global */
/***********************************************************************
* Macros: r_ospl_internal_sentinel
*
*    : R_F_OSPL_ASYNC_FLAGS_SENTINEL_MASK  - 0x7FFFFF40
*    : R_F_OSPL_ASYNC_FLAGS_SENTINEL_VALUE - 0x4A5C0000
************************************************************************/
#ifndef R_OSPL_NDEBUG
enum {
    R_F_OSPL_ASYNC_FLAGS_SENTINEL_MASK  = 0x7FFFFF40,
    R_F_OSPL_ASYNC_FLAGS_SENTINEL_VALUE = 0x4A5C0000
};
#endif


/**
* @struct  r_ospl_caller_t
* @brief  Context of interrupt callback function caller
*
* @par Description
*    Member variables should not be accessed.
*/
struct st_r_ospl_caller_t {
    r_ospl_async_t                *Async;
    volatile int_fast32_t         *PointerToState;
    int_fast32_t                   StateValueOfOnInterrupting;
    void                          *I_Lock;
    const r_ospl_i_lock_vtable_t  *I_LockVTable;
};


/***********************************************************************
* Class: r_ospl_i_lock_vtable_t
*    I-Lock V-Table
************************************************************************/
typedef bool_t (* r_ospl_i_lock_lock_func_t )( void *const  self_ );
typedef void   (* r_ospl_i_lock_unlock_func_t )( void *const  self_ );
typedef void   (* r_ospl_i_lock_get_rf_func_t )( void *const  self_ );    /* rf = RequestFinalize */
typedef bool_t (* r_ospl_i_lock_get_rcn_func_t )( const void *const  self_ );   /* rcn = RootChannelNum */

struct st_r_ospl_i_lock_vtable_t {

    /**
    * @brief   Lock
    *
    * @par Parameters
    *    None
    * @return  None.
    */
    bool_t  (* Lock )( void *const  self_ );

    /**
    * @brief   Unlock
    *
    * @par Parameters
    *    None
    * @return  None.
    */
    void    (* Unlock )( void *const  self_ );

    /**
    * @brief   RequestFinalize
    *
    * @par Parameters
    *    None
    * @return  None.
    */
    void    (* RequestFinalize )( void *const  self_ );

    /**
    * @brief   GetRootChannelNum
    *
    * @par Parameters
    *    None
    * @return  None.
    */
    int_fast32_t  (* GetRootChannelNum )( const void *const  self_ );
};


/**
* @struct  r_ospl_memory_spec_t
* @brief  Memory specification
*/
struct st_r_ospl_memory_spec_t {

    /** (byte) */
    uint_fast32_t  CacheLineSize;
};


/**
* @struct  r_ospl_ftimer_spec_t
* @brief  Free run timer specification
*/
struct st_r_ospl_ftimer_spec_t {

    /** msec_Numerator */
    uint32_t  msec_Numerator;

    /** msec_Denominator */
    uint32_t  msec_Denominator;

    /** MaxCount */
    uint32_t  MaxCount;

    /** ExtensionOfCount */
    uint32_t  ExtensionOfCount;
};


/**
* @struct  r_ospl_table_t
* @brief  Index table
*
* @par Description
*    Member variables should not be accessed.
*/
struct st_r_ospl_table_t {
    void         *Area;
    int_fast32_t  Count;
    int_fast32_t  MaxCount;
    const void   *KeyCache;
    int_fast32_t  IndexCache;  /* Not SortedKeyIndex */
    int16_t       FirstFreeIndex;
#if R_OSPL_IS_PREEMPTION
    bool_t        Is_T_Lock;
#endif
};


/* Section: Global */
/**
* @enum   r_ospl_if_not_t
* @brief  Operation if not exists
*
*    - R_OSPL_ERROR_IF_NOT - 0
*    - R_OSPL_ALLOCATE_IF_NOT - 1
*    - R_OSPL_DO_NOTHING_IF_NOT - 2
*/
typedef enum {
    R_OSPL_ERROR_IF_NOT      = 0,
    R_OSPL_ALLOCATE_IF_NOT   = 1,
    R_OSPL_DO_NOTHING_IF_NOT = 2
} r_ospl_if_not_t;


/**
* @struct  r_ospl_error_t
* @brief  Error status of a thread
*
* @par Description
*    Member variables should not be accessed.
*/
#if R_OSPL_ERROR_BREAK  ||  R_OSPL_TLS_ERROR_CODE
typedef  struct st_r_ospl_error_t  r_ospl_error_t;
struct st_r_ospl_error_t {
#if R_OSPL_ERROR_BREAK
    bool_t         IsError;
    int_fast32_t   ErrorID;
    const char_t  *FilePath;
    int_fast32_t   LineNum;
#if R_OSPL_IS_PREEMPTION
    int_fast32_t   ThreadLockedCount;
#endif
#endif
#if R_OSPL_TLS_ERROR_CODE
    errnum_t       ErrNum;
#endif
};
#endif


/**
* @struct  r_ospl_global_error_t
* @brief  Error status of global
*
* @par Description
*    Member variables should not be accessed.
*/
#if R_OSPL_ERROR_BREAK  ||  R_OSPL_TLS_ERROR_CODE
typedef  struct st_r_ospl_global_error_t  r_ospl_global_error_t;
struct st_r_ospl_global_error_t {
    r_ospl_table_t   ThreadIndexTable;
    r_ospl_error_t  *ErrorArray;

#if R_OSPL_ERROR_BREAK
    int_fast32_t     RaisedGlobalErrorID;
    int_fast32_t     BreakGlobalErrorID;
#endif
};
#endif


/* Section: Global */
/**
* @enum   r_ospl_axi_cache_attribute_t
* @brief  Cache attribute on AXI bus.
*
*    - R_OSPL_AXI_CACHE_ZERO - Not AXI
*    - R_OSPL_AXI_STRONGLY - Strongly order access
*    - R_OSPL_AXI_DEVICE - DEVICE
*    - R_OSPL_AXI_UNCACHED - Normal access (Out of order) uncached
*    - R_OSPL_AXI_WRITE_BACK_W - Write back, allocate on write
*    - R_OSPL_AXI_WRITE_BACK - Write back, allocate on both read and write
*/
typedef enum {
    R_OSPL_AXI_CACHE_ZERO     =  0,
    R_OSPL_AXI_STRONGLY       =  0,
    R_OSPL_AXI_DEVICE         =  1,
    R_OSPL_AXI_UNCACHED       =  3,
    R_OSPL_AXI_WRITE_BACK_W   = 11,
    R_OSPL_AXI_WRITE_BACK     = 15
} r_ospl_axi_cache_attribute_t;


/**
* @enum   r_ospl_axi_protection_t
* @brief  Protection on AXI bus.
*
*    - R_OSPL_AXI_PROTECTION_ZERO - Not AXI
*    - R_OSPL_AXI_SECURE - TrustZone secure acccess
*    - R_OSPL_AXI_NON_SECURE - TrustZone non-secure acccess
*/
typedef enum {
    R_OSPL_AXI_PROTECTION_ZERO = 0,
    R_OSPL_AXI_SECURE          = 0,
    R_OSPL_AXI_NON_SECURE      = 2
} r_ospl_axi_protection_t;


/******************************************************************************
Macro definitions
******************************************************************************/

/**
* @enum   Event_Bit_Name
* @brief  Event_Bit_Name
*    - R_OSPL_ANY_FLAG - 0x0000
*    - R_OSPL_A_FLAG - 0x0001
*    - R_OSPL_I_FLAG - 0x0002
*    - R_OSPL_FINAL_A_FLAG - 0x0004
*/
enum {
    R_OSPL_ANY_FLAG         = 0x00000000,
    R_OSPL_A_FLAG           = 0x00000001,
    R_OSPL_I_FLAG           = 0x00000002,
    R_OSPL_FINAL_A_FLAG     = 0x00000004
};


/**
* @typedef  r_ospl_cancel_flag_t
* @brief  Bit flags of <r_ospl_cancel_bit_t>
*/
typedef int_t  r_ospl_cancel_flag_t;


/**
* @enum   r_ospl_cancel_bit_t
* @brief  r_ospl_cancel_bit_t
*    - R_OSPL_CANNEL_REQUEST - 0x0001
*    - R_OSPL_CANNELING - 0x0002
*    - R_OSPL_CANNELED - 0x0004
*    - R_OSPL_FINALIZE_REQUEST - 0x0010
*    - R_OSPL_FINALIZING - 0x0020
*    - R_OSPL_FINALIZED - 0x0040
*/
enum {
    R_OSPL_CANNEL_REQUEST   = 0x00000001,
    R_OSPL_CANNELING        = 0x00000002,
    R_OSPL_CANNELED         = 0x00000004,
    R_OSPL_FINALIZE_REQUEST = 0x00000010,
    R_OSPL_FINALIZING       = 0x00000020,
    R_OSPL_FINALIZED        = 0x00000040
};


/***********************************************************************
* Macros: OSPL_Others
*
*    : R_OSPL_INFINITE         - One of time out value
*    : R_OSPL_FLAG32_ALL_BITS  - R_OSPL_FLAG32_ALL_BITS
*    : R_OSPL_EVENT_ALL_BITS   - R_OSPL_EVENT_ALL_BITS
*    : R_OSPL_TIMEOUT          - Raised time out
*    : R_OSPL_MAX_TIME_OUT     - Max value of time out
*    : R_OSPL_NO_INDEX         - R_OSPL_NO_INDEX
*    : R_OSPL_UNLOCKED_CHANNEL - R_OSPL_UNLOCKED_CHANNEL
************************************************************************/
#define R_OSPL_INFINITE           0xFFFFFFFFu   /* uint32_t */
#define R_OSPL_FLAG32_ALL_BITS    0xFFFFFFFFu   /* uint32_t */
enum {  R_OSPL_EVENT_ALL_BITS   = 0x0000FFFF };
enum {  R_OSPL_TIMEOUT          = 0x40000000 };
enum {  R_OSPL_MAX_TIME_OUT     = 65533 };
enum {  R_OSPL_NO_INDEX         = -1         };
enum {  R_OSPL_UNLOCKED_CHANNEL = 0x00000FEE };


/**
* @def  R_OSPL_TABLE_SIZE
* @brief  Calculates the size of <r_ospl_table_t> type index table
* @param   MaxCount Max index count
* @return  Table size
*/
/* ->MISRA 19.7 : Array count must const */ /* ->SEC M5.1.3 */
#define  R_OSPL_TABLE_1_SIZE  8  /* sizeof(r_ospl_table_block_t) */
#define  R_OSPL_TABLE_SIZE( MaxCount ) \
	( (MaxCount) * R_OSPL_TABLE_1_SIZE )
/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */


/**
* @def  R_OSPL_DEBUG_WORK_SIZE
* @brief  Calculates the size of debug work area
* @param   ThreadMaxCount Max thread count using error breaking system of OSPL
* @return  Size of debug work area
*/
/* ->MISRA 19.7 : Array count must const */ /* ->SEC M5.1.3 */
#define  R_OSPL_DEBUG_WORK_1_SIZE  32  /* sizeof(r_ospl_table_block_t) + sizeof(r_ospl_error_t) */
#define  R_OSPL_DEBUG_WORK_SIZE( ThreadMaxCount ) \
	( (ThreadMaxCount) * R_OSPL_DEBUG_WORK_1_SIZE )
/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */


/**
* @enum   errnum_t
* @brief  Error code defined by OSPL
*
*    - E_OTHERS - 0x01
*    - E_FEW_ARRAY - 0x02
*    - E_FEW_MEMORY - 0x03
*    - E_FIFO_OVER - 0x04
*    - E_NOT_FOUND_SYMBOL - 0x05
*    - E_NO_NEXT - 0x06
*    - E_ACCESS_DENIED - 0x07
*    - E_NOT_IMPLEMENT_YET - 0x09
*    - E_ERRNO - 0x0E
*    - E_LIMITATION - 0x0F
*    - E_STATE - 0x10
*    - E_NOT_THREAD - 0x11
*    - E_PATH_NOT_FOUND - 0x12
*    - E_BAD_COMMAND_ID - 0x16
*    - E_TIME_OUT - 0x17
*    - E_NO_DEBUG_TLS - 0x1D
*    - E_EXIT_TEST - 0x1E
*/
#define  E_CATEGORY_MASK   0xFFFFFFE0u  /* E_CATEGORY_* */
enum {   E_OFFSET_MASK   = 0x0000001F };

#ifndef  E_CATEGORY_COMMON  /* Overwritable */
#define  E_CATEGORY_COMMON  E_CATEGORY_COMMON
enum { E_CATEGORY_COMMON = 0x00000000 };  /* 0x01, 0x02 .. 0x1F : Reseved */
#endif

enum { E_OTHERS             = E_CATEGORY_COMMON | 0x01 }; /*  1 */
enum { E_FEW_ARRAY          = E_CATEGORY_COMMON | 0x02 }; /*  2 */
enum { E_FEW_MEMORY         = E_CATEGORY_COMMON | 0x03 }; /*  3 */
enum { E_FIFO_OVER          = E_CATEGORY_COMMON | 0x04 }; /*  4 */
enum { E_NOT_FOUND_SYMBOL   = E_CATEGORY_COMMON | 0x05 }; /*  5 */
enum { E_NO_NEXT            = E_CATEGORY_COMMON | 0x06 }; /*  6 */
enum { E_ACCESS_DENIED      = E_CATEGORY_COMMON | 0x07 }; /*  7 */
enum { E_NOT_IMPLEMENT_YET  = E_CATEGORY_COMMON | 0x09 }; /*  9 */
enum { E_ERRNO              = E_CATEGORY_COMMON | 0x0E }; /* 14 */
enum { E_LIMITATION         = E_CATEGORY_COMMON | 0x0F }; /* 15 */
enum { E_STATE              = E_CATEGORY_COMMON | 0x10 }; /* 16 */
enum { E_NOT_THREAD         = E_CATEGORY_COMMON | 0x11 }; /* 17 */
enum { E_PATH_NOT_FOUND     = E_CATEGORY_COMMON | 0x12 }; /* 18 */
enum { E_BAD_COMMAND_ID     = E_CATEGORY_COMMON | 0x16 }; /* 22 */
enum { E_TIME_OUT           = E_CATEGORY_COMMON | 0x17 }; /* 23 */
enum { E_NO_DEBUG_TLS       = E_CATEGORY_COMMON | 0x1D }; /* 29 */
enum { E_EXIT_TEST          = E_CATEGORY_COMMON | 0x1E }; /* 30 */


/**
* @def  R_JOIN_SYMBOL_FOR_ASSERT
* @brief  Sub routine of <R_STATIC_ASSERT>
* @par Parameters
*    None
* @return  None.
*
* - This macro extends "x", "y"
* - This code is referenced by CERT secure coding standard PRE05-C
*/
/* ->MISRA 19.7 : Extend macro arguments */ /* ->SEC M5.1.3 */
#define R_JOIN_SYMBOL_FOR_ASSERT(x, y)  R_JOIN_SYMBOL_FOR_ASSERT_SUB(x, y)
/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */
/* ->MISRA 19.13 : This is used only assertion in compiling */ /* ->SEC M5.1.2 (1) */
#define R_JOIN_SYMBOL_FOR_ASSERT_SUB(x, y) x##y
/* <-MISRA 19.13 */ /* <-SEC M5.1.2 (1) */


/**
* @def  R_STATIC_ASSERT
* @brief  "static_assert" for in function
* @par Parameters
*    None
* @return  None.
*
* @par Description
*    Compatible with static_assert (C++0x). But our naming rule is not match.
*    - This code is referenced by CERT secure coding standard DCL03 - C.
*    This macro raised QAC warning 3205. This is no problem.
*/
/* ->SEC M1.1.1 */
#define  R_STATIC_ASSERT( ConstantExpression, StringLiteral ) \
	do { typedef char_t R_JOIN_SYMBOL_FOR_ASSERT( assertion_failed_t_, __LINE__ ) \
		[(ConstantExpression) ? 1 : -1]; } while(0)
/* If "ConstantExpression" is false, illegal array size error will be raised. */
/* <-SEC M1.1.1 */


/**
* @def  R_STATIC_ASSERT_GLOBAL
* @brief  "static_assert" for in global scope
* @par Parameters
*    None
* @return  None.
*/
/* ->SEC M1.1.1 */
/* ->MISRA 19.4 : There is not if statement in global */ /* ->SEC M1.8.2 */
/* ->MISRA 19.7 : Cannot function */ /* ->SEC M5.1.3 */
#define  R_STATIC_ASSERT_GLOBAL( ConstantExpression, StringLiteral ) \
	     typedef char_t R_JOIN_SYMBOL_FOR_ASSERT( assertion_failed_t_, __LINE__ ) \
		[(ConstantExpression) ? 1 : -1]
/* If "ConstantExpression" is false, illegal array size error will be raised. */
/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */
/* <-MISRA 19.4 */ /* <-SEC M1.8.2 */
/* <-SEC M1.1.1 */


/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
/* In "r_ospl.h" */


/***********************************************************************
* End of File:
************************************************************************/
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif /* R_OSPL_H */

