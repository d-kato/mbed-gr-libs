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
* @file  r_ospl.h
* @brief   OS Porting Layer. Main Header. Functions.
*
* $Module: OSPL $ $PublicVersion: 0.90 $ (=R_OSPL_VERSION)
* $Rev: 35 $
* $Date:: 2014-04-15 21:38:18 +0900#$
*/

#ifndef R_OSPL_H
#define R_OSPL_H


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "Project_Config.h"
#include "platform.h"
#include "r_ospl_typedef.h"
#include "r_multi_compiler_typedef.h"
#include "locking.h"
#include "r_static_an_tag.h"
#include "r_ospl_debug.h"
#if ! R_OSPL_IS_PREEMPTION
#include "r_ospl_os_less.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************
Typedef definitions
******************************************************************************/
/* In "r_ospl_typedef.h" */

/******************************************************************************
Macro definitions
******************************************************************************/
/* In "r_ospl_typedef.h" */

/******************************************************************************
Variable Externs
******************************************************************************/
/* In "r_ospl_typedef.h" */

/******************************************************************************
Functions Prototypes
******************************************************************************/


/* Section: Version and initialize */
/**
* @brief   Returns version number of OSPL
*
* @par Parameters
*    None
* @return  Version number of OSPL
*
* @par Description
*    Return value is same as "R_OSPL_VERSION" macro.
*/
int32_t   R_OSPL_GetVersion(void);


/**
* @brief   Returns whether the environment is supported preemption
*
* @par Parameters
*    None
* @return  Whether the environment is RTOS supported preemption
*
* @par Description
*    Return value is same as "R_OSPL_IS_PREEMPTION" macro.
*/
bool_t    R_OSPL_IsPreemption(void);


/**
* @brief   Initializes the internal of OSPL
*
* @param   NullConfig Specify NULL
* @return  None
*
* @par Description
*    Initializes internal mutual exclusion objects.
*    However, "R_OSPL_Initialize" function does not have to be called for
*    OSPL of "R_OSPL_IS_PREEMPTION = 0".
*    "E_ACCESS_DENIED" error is raised, when the OSPL API that it is
*    necessary to call "R_OSPL_Initialize" before calling the API was called.
*/
errnum_t  R_OSPL_Initialize( const void *const  NullConfig );


/* Section: Standard functions */
/**
* @brief   No operation from C++ specification
*
* @par Parameters
*    None
* @return  None
*
* @par Description
*    Compatible with __noop (MS C++). But our naming rule is not match.
*/
INLINE void  R_NOOP(void) {}


/**
* @brief   Returns element count of the array
*
* @param   Array An array
* @return  Count of specified array's element
*
* @par Description
*    Compatible with _countof (MS C++) and ARRAY_SIZE (Linux).
*    But our naming rule is not match.
*
* @par Example
*    @code
*    uint32_t  array[10];
*    R_COUNT_OF( array )  // = 10
*    @endcode
*
* @par Example
*    Array argument must not be specified the pointer using like array.
*    @code
*    uint32_t   array[10];
*    func( array );
*
*    void  func( uint32_t array[] )  // "array" is a pointer
*    {
*        R_COUNT_OF( array )  // NG
*    }
*    @endcode
*/
/* ->MISRA 19.7 : Cannot function */ /* ->SEC M5.1.3 */
#define  R_COUNT_OF( Array )  ( sizeof( Array ) / sizeof( *(Array) ) )
/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */


/* Section: Error handling and debugging (1) */


/**
* @def  IF
* @brief  Breaks and transits to error state, if condition expression is not 0
* @param   Condition Condition expression
* @return  None
*
* @par Example
*    @code
*    e= TestFunction(); IF(e){goto fin;}
*    @endcode
*
* @par Description
*    "IF" is as same as general "if", if "R_OSPL_ERROR_BREAK" macro was
*    defined to be 0. The following descriptions are available,
*    if "R_OSPL_ERROR_BREAK" macro was defined to be 1.
*
*    "IF" macro supports to find the code raising an error.
*
*    If the "if statement" that is frequently seen in guard condition and
*    after calling functions was changed to "IF" macro, the CPU breaks
*    at raising an error. Then the status (values of variables) can be
*    looked immediately and the code (call stack) can be looked. Thus,
*    debug work grows in efficiency.
*
*    "IF" macro promotes recognizing normal code and exceptional code.
*    Reading speed will grow up by skipping exceptional code.
*
*    Call "R_OSPL_SET_BREAK_ERROR_ID" function, if set to break at the code
*    raising an error.
*
*    Whether the state was error state or the error raised count is stored
*    in the thread local storage. In Release configuration, the variable
*    of error state and the error raised count is deleted. Manage the error
*    code using auto variable and so on at out of OSPL.
*
*    The error state is resolved by calling "R_OSPL_CLEAR_ERROR" function.
*    If "R_DEBUG_BREAK_IF_ERROR" macro was called with any error state,
*    the process breaks at the macro.
*/
#if R_OSPL_ERROR_BREAK

/* ->MISRA 19.4 : Abnormal termination. Compliant with C language syntax. */ /* ->SEC M1.8.2 */
#define  IF( Condition ) \
		if ( IS( R_OSPL_OnRaisingErrorForMISRA( \
			IS( (int_fast32_t)( Condition ) ), __FILE__, __LINE__ ) ) )
/* (int_fast32_t) cast is for QAC warning of implicit cast unsigned to signed */
/* != 0 is for QAC warning of MISRA 13.2 Advice */
/* <-MISRA 19.4 */ /* <-SEC M1.8.2 */

#else  /* ! R_OSPL_ERROR_BREAK */

/* ->MISRA 19.4 : Abnormal termination. Compliant with C language syntax. */ /* ->SEC M1.8.2 */
#define  IF  if
/* <-MISRA 19.4 */ /* <-SEC M1.8.2 */
#endif


/**
* @def  IF_D
* @brief  It is same as "IF" (for Debug configuration only)
* @param   Condition Condition expression
* @return  None
*
* @par Description
*    In Release configuration, the result of condition expression is always "false".
*    The release configuration is the configuration defined "R_OSPL_NDEBUG".
*/
/* ->MISRA 19.4 : Compliant with C language syntax. */ /* ->SEC M1.8.2 */
/* ->MISRA 19.7 : Cannot function */ /* ->SEC M5.1.3 */
#ifndef R_OSPL_NDEBUG
#define  IF_D( Condition )  IF ( Condition )
#else
#define  IF_D( Condition )  if ( false )
#endif
/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */
/* <-MISRA 19.4 */ /* <-SEC M1.8.2 */


/**
* @def  ASSERT_R
* @brief  Assertion (Programming By Contract)
* @param   Condition The condition expression expected true
* @param   goto_fin_Statement The operation doing at condition is false
* @return  None
*
* @par Description
*    It is possible to write complex sentence divided by ";" in
*    "goto_fin_Statement" argument.
*
*    @par - Case of defined "R_OSPL_ERROR_BREAK" to be 0
*    If the result of condition expression is 0(false), do "StatementsForError".
*    If operations did nothing, write "R_NOOP()" at "StatementsForError" argument.
*
*    @par - Case of defined "R_OSPL_ERROR_BREAK" to be 1
*    If the result of condition expression is 0(false), the error state
*    will become active and the operation of "StatementForError" argument
*    will be done.
*/
#ifndef  __cplusplus
#define  ASSERT_R( Condition, goto_fin_Statement ) \
		do{ IF(!(Condition)) { goto_fin_Statement; } } while(0)  /* do-while is CERT standard PRE10-C */
#else
#define  ASSERT_R( Condition, goto_fin_Statement ) \
		{ IF(!(Condition)) { goto_fin_Statement; } }  /* no C5236(I) */
#endif


/**
* @def  ASSERT_D
* @brief  Assertion (Programming By Contract) (for Debug configuration only)
* @param   Condition The condition expression expected true
* @param   goto_fin_Statement The operation doing at condition is false
* @return  None
*
* @par Description
*    This does nothing in Release configuration.
*    Release configuration is the configuration defined "R_OSPL_NDEBUG"
*    as same as standard library.
*/
#ifndef R_OSPL_NDEBUG
#define  ASSERT_D  ASSERT_R
#else
/* ->MISRA 19.7 : Function's argument can not get "goto_fin_Statement" */ /* ->SEC M5.1.3 */
#define  ASSERT_D( Condition, goto_fin_Statement )  R_NOOP()
/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */
#endif


/**
* @brief   Sub routine of IF macro
*
* @param   Condition Condition in IF macro
* @param   File File name
* @param   Line Line number
* @return  "Condition" argument
*
* @par Description
*    - This part is for compliant to MISRA 2004 - 19.7.
*/
bool_t  R_OSPL_OnRaisingErrorForMISRA( bool_t const  Condition,  const char_t *const  File,
                                       int_t const  Line );


/***********************************************************************
* Class: r_ospl_thread_id_t
************************************************************************/

/**
* @brief   Get running thread ID (for OS less and OS-using environment)
*
* @par Parameters
*    None
* @return  The current running thread ID
*
* @par Description
*    - It is possible to use this function for both OS less and OS - using environment.
*    For OS less, returns thread ID passed to "R_OSPL_THREAD_SetCurrentId" function.
*    "NULL" is returned, if in interrupt context.
*/
r_ospl_thread_id_t  R_OSPL_THREAD_GetCurrentId(void);


/**
* @brief   Set one or some bits to 1
*
* @param   ThreadId The thread ID attached the target event
* @param   SetFlags The value of bit flags that target bit is 1
* @return  None
*
* @par Description
*    For OS less, there is the area disabled all interrupts.
*
*    - For OS - using environment, the thread waiting in "R_OSPL_EVENT_Wait"
*    function might wake up soon.
*
*    Do nothing, when "ThreadId" = "NULL"
*/
void  R_OSPL_EVENT_Set( r_ospl_thread_id_t const  ThreadId, bit_flags32_t const  SetFlags );


/**
* @brief   Set one or some bits to 0
*
* @param   ThreadId The thread ID attached the target event
* @param   ClearFlags1 The value of bit flags that clearing bit is 1
* @return  None
*
* @par Description
*    It is not necessary to call this function after called "R_OSPL_EVENT_Wait"
*    function.
*
*    The way that all bit flags is cleared is setting "R_OSPL_EVENT_ALL_BITS"
*    (=0x0000FFFF) at "ClearFlags1" argument.
*
*    When other thread was nofied by calling "R_OSPL_EVENT_Set", "R_OSPL_EVENT_Clear"
*    must not be called from caller (notifier) thread.
*
*    For OS less, there is the area disabled all interrupts.
*
*    Do nothing, when "ThreadId" = "NULL"
*/
void  R_OSPL_EVENT_Clear( r_ospl_thread_id_t const  ThreadId, bit_flags32_t const  ClearFlags1 );


/**
* @brief   Get 16bit flags value
*
* @param   ThreadId The thread ID attached the target event
* @return  The value of 16bit flags
*
* @par Description
*    This API cannot be used in newest specification.
*
*    In receiving the event, call "R_OSPL_EVENT_Wait" function instead of
*    "R_OSPL_EVENT_Get" function or call "R_OSPL_EVENT_Clear" function
*    passed the NOT operated value of flags got by "R_OSPL_EVENT_Get" function.
*/
#if ( ! defined( osCMSIS )  ||  osCMSIS <= 0x10001 ) &&  R_OSPL_VERSION < 85
bit_flags32_t  R_OSPL_EVENT_Get( r_ospl_thread_id_t const  ThreadId );
#endif


/**
* @brief   Waits for setting the flags in 16bit and clear received flags
*
* @param   WaigingFlags The bit flags set to 1 waiting or "R_OSPL_ANY_FLAG"
* @param   out_GotFlags NULL is permitted. Output: 16 bit flags or "R_OSPL_TIMEOUT"
* @param   Timeout_msec Time out (millisecond) or "R_OSPL_INFINITE"
* @return  Error code.  If there is no error, the return value is 0.
*
* @par Description
*    Waits in this function until the flags become passed flags pattern
*    by "R_OSPL_EVENT_Set" function.
*
*    Check "r_ospl_async_t::ReturnValue", when the asynchronous operation
*    was ended.
*/
errnum_t  R_OSPL_EVENT_Wait( bit_flags32_t const  WaigingFlags,  bit_flags32_t *const  out_GotFlags,
                             uint32_t const  Timeout_msec );
/* Unsigned flag (bit_flags32_t) is for QAC 4130 */


/***********************************************************************
* Class: r_ospl_flag32_t
************************************************************************/

/**
* @brief   Clears all flags in 32bit to 0
*
* @param   self The value of 32bit flags
* @return  None
*
* @par Description
*    Operates following operation.
*    @code
*    volatile bit_flags32_t  self->flags;
*    self->flags = 0;
*    @endcode
*/
void  R_OSPL_FLAG32_InitConst( volatile r_ospl_flag32_t *const  self );


/**
* @brief   Set one or some bits to 1
*
* @param   self The value of 32bit flags
* @param   SetFlags The value of bit flags that target bit is 1
* @return  None
*
* @par Description
*    Operates following operation.
*    @code
*    volatile bit_flags32_t  self->Flags;
*    bit_flags32_t           SetFlags;
*    self->Flags |= SetFlags;
*    @endcode
*    This function is not atomic because "|=" operator is "Read Modify Write" operation.
*/
void  R_OSPL_FLAG32_Set( volatile r_ospl_flag32_t *const  self, bit_flags32_t const  SetFlags );


/**
* @brief   Set one or some bits to 0
*
* @param   self The value of 32bit flags
* @param   ClearFlags1 The value of bit flags that clearing bit is 1
* @return  None
*
* @par Description
*    Operates following operation.
*    @code
*    volatile bit_flags32_t  self->Flags;
*    bit_flags32_t           ClearFlags1;
*
*    self->Flags &= ~ClearFlags1;
*    @endcode
*
*    Set "R_OSPL_FLAG32_ALL_BITS", if you wanted to clear all bits.
*
*    This function is not atomic because "&=" operator is "Read Modify Write" operation.
*/
void  R_OSPL_FLAG32_Clear( volatile r_ospl_flag32_t *const  self, bit_flags32_t const  ClearFlags1 );


/**
* @brief   Get 32bit flags value
*
* @param   self The value of 32bit flags
* @return  The value of 32bit flags
*
* @par Description
*    In receiving the event, call "R_OSPL_FLAG32_GetAndClear" function
*    instead of "R_OSPL_FLAG32_Get" function or call "R_OSPL_FLAG32_Clear"
*    function passed the NOT operated value of flags got by "R_OSPL_FLAG32_Get"
*    function.
*
*    @code
*    Operates following operation.
*    volatile bit_flags32_t  self->Flags;
*    bit_flags32_t           return_flags;
*
*    return_flags = self->Flags;
*
*    return  return_flags;
*    @endcode
*/
bit_flags32_t  R_OSPL_FLAG32_Get( volatile const r_ospl_flag32_t *const  self );


/**
* @brief   Get 32bit flags value
*
* @param   self The value of 32bit flags
* @return  The value of 32bit flags
*
* @par Description
*    Operates following operation.
*    @code
*    volatile bit_flags32_t  self->Flags;
*    bit_flags32_t           return_flags;
*
*    return_flags = self->Flags;
*    self->Flags = 0;
*
*    return  return_flags;
*    @endcode
*
*    This function is not atomic because the value might be set before clearing to 0.
*/
bit_flags32_t  R_OSPL_FLAG32_GetAndClear( volatile r_ospl_flag32_t *const  self );


/***********************************************************************
* Class: r_ospl_queue_t
************************************************************************/

/**
* @brief   Initializes a queue
*
* @param   out_self Output: Address of initialized queue object
* @param   QueueDefine Initial attributes of queue and work area
* @return  Error code.  If there is no error, the return value is 0
*
* @par Description
*    It is not possible to call this function from the library.
*    This function is called from porting layer of the driver and send
*    created queue to the driver.
*
*    OSPL does not have finalizing function (portabled with CMSIS).
*    An object specified "QueueDefine" argument can be specified to the
*    create function 1 times only. Some OS does not have this limitation.
*
*    The address of a variable as "r_ospl_queue_t*" type is set at
*    "out_self" argument.
*    Internal variables of the queue are stored in the variable specified
*    with "QueueDefine" argument.
*/
errnum_t  R_OSPL_QUEUE_Create( r_ospl_queue_t **out_self, r_ospl_queue_def_t *QueueDefine );


/**
* @brief   Gets status of the queue
*
* @param   self A queue object
* @param   out_Status Output: Pointer to the status structure
* @return  Error code.  If there is no error, the return value is 0
*
* @par Description
*    Got status are the information at calling moment.
*    If ohter threads were run, the status will be changed.
*    See "R_DRIVER_GetAsyncStatus" function about pointer type of
*    "out_Status" argument
*/
errnum_t  R_OSPL_QUEUE_GetStatus( r_ospl_queue_t *self,  const r_ospl_queue_status_t **out_Status );


/**
* @brief   Allocates an element from the queue object
*
* @param   self A queue object
* @param   out_Address Output: Address of allocated element
* @param   Timeout_msec Timeout (msec) or R_OSPL_INFINITE
* @return  Error code.  If there is no error, the return value is 0
*
* @par Description
*    An error will be raised, if "Timeout_msec != 0" in interrupt context.
*    It becomes "*out_Address = NULL", when it was timeout and
*    "Timeout_msec = 0".
*    E_TIME_OUT error is raised, when it was timeout and "Timeout_msec != 0".
*/
errnum_t  R_OSPL_QUEUE_Allocate( r_ospl_queue_t *self,  void *out_Address,  uint32_t Timeout_msec );


/**
* @brief   Sends the element to the queue
*
* @param   self A queue object
* @param   Address Address of element to put
* @return  Error code.  If there is no error, the return value is 0
*
* @par Description
*    It is correct, even if other thread put to the queue or get from
*    the queue from calling "R_OSPL_QUEUE_Allocate" to calling
*    "R_OSPL_QUEUE_Put".
*
*    The message put to the queue by this function receives the thread
*    calling "R_OSPL_QUEUE_Get" function.
*/
errnum_t  R_OSPL_QUEUE_Put( r_ospl_queue_t *self,  void *Address );


/**
* @brief   Receives the element from the queue
*
* @param   self A queue object
* @param   out_Address Output: Address of received element
* @param   Timeout_msec Timeout (msec) or R_OSPL_INFINITE
* @return  Error code.  If there is no error, the return value is 0
*
* @par Description
*    Call "R_OSPL_QUEUE_Free" function after finishing to access to
*    the element. Don't access the memory area of the element after
*    calling "R_OSPL_QUEUE_Free".
*
*    "E_NOT_THREAD" error is raised, if "Timeout_msec = 0" was specified
*    from interrupt context. It is not possible to wait for put data to
*    the queue in interrupt context.
*
*    "*out_Address" is NULL and any errors are not raised, if it becomed
*    to timeout and "Timeout_msec = 0". "E_TIME_OUT" is raised,
*    if "Timeout_msec != 0".
*
*    Specify "Timeout_msec = 0", call the following functions by the following
*    order and use an event for preventing to block to receive other events
*    by the thread having waited for the queue.
*
*    Sending Side
*    - R_OSPL_QUEUE_Allocate
*    - R_OSPL_QUEUE_Put
*    - R_OSPL_EVENT_Set
*
*    Receiving Side
*    - R_OSPL_EVENT_Wait
*    - R_OSPL_QUEUE_Get
*    - R_OSPL_QUEUE_Free
*
*    In OS less environment, "R_OSPL_QUEUE_Get" supports pseudo multi
*    threading. See "R_OSPL_THREAD_GetIsWaiting" function.
*/
errnum_t  R_OSPL_QUEUE_Get( r_ospl_queue_t *self,  void *out_Address,  uint32_t Timeout_msec );


/**
* @brief   Releases the element to the queue object
*
* @param   self A queue object
* @param   Address Address of received element
* @return  Error code.  If there is no error, the return value is 0
*
* @par Description
*    It is correct, even if other thread put to the queue or get from
*    the queue from calling "R_OSPL_QUEUE_Get" to calling "R_OSPL_QUEUE_Free".
*/
errnum_t  R_OSPL_QUEUE_Free( r_ospl_queue_t *self,  void *Address );


/**
* @brief   Print status of the queue object
*
* @param   self A queue object
* @return  Error code.  If there is no error, the return value is 0
*/
#ifndef  R_OSPL_NDEBUG
errnum_t  R_OSPL_QUEUE_Print( r_ospl_queue_t *self );
#endif


/***********************************************************************
* Class: r_ospl_async_t
************************************************************************/

/**
* @brief   CopyExceptAThread
*
* @param   Source Source
* @param   Destination Destination
* @return  None
*/
void  R_OSPL_ASYNC_CopyExceptAThread( const r_ospl_async_t *const  Source,
                                      r_ospl_async_t *const  Destination );


/***********************************************************************
* Class: r_ospl_caller_t
************************************************************************/

/**
* @brief   Calls the interrupt callback function. It is called from OS porting layer in the driver
*
* @param   self The internal parameters about interrupt operations
* @param   InterruptSource The source of the interrupt
* @return  None
*/
void  R_OSPL_CallInterruptCallback( const r_ospl_caller_t *const  self,
                                    const r_ospl_interrupt_t *const  InterruptSource );


/**
* @brief   Initialize <r_ospl_caller_t>.
*
* @param   self The internal parameters about interrupt operations
* @param   Async <r_ospl_async_t>
* @return  None
*/
void  R_OSPL_CALLER_Initialize( r_ospl_caller_t *const  self,  r_ospl_async_t *const  Async,
                                volatile void *const  PointerToState,  int_t const  StateValueOfOnInterrupting,
                                void *const  I_Lock, const r_ospl_i_lock_vtable_t *const  I_LockVTable );


/**
* @brief   GetRootChannelNum.
*
* @param   self The internal parameters about interrupt operations
* @return  RootChannelNum
*/
INLINE int_fast32_t  R_OSPL_CALLER_GetRootChannelNum( const r_ospl_caller_t *const  self );


/* Section: Interrupt */
/**
* @brief   Interrupt callback function for unregisterd interrupt.
*
* @param   int_sense (See INTC driver)
* @return  None
*/
void  R_OSPL_OnInterruptForUnregistered( uint32_t const  int_sense );


/**
* @brief   Releases all disabled interrupts
*
* @par Parameters
*    None
* @return  None
*
* @par Description
*    Driver user should not call this function.
*    Call this function at the end of area of all interrupts disabled.
*    Do not release, if all interrupts was already disabled by caller function.
*    This function does not release disabled NMI.
*/
void  R_OSPL_EnableAllInterrupt(void);


/**
* @brief   Disables all interrupts
*
* @par Parameters
*    None
* @return  None
*
* @par Description
*    Driver user should not call this function.
*    Call this function at begin of area of all interrupts disabled.
*    This function does not disable NMI.
*
* @par Example
*    @code
*    void  Func()
*    {
*        bool_t  was_all_enabled = false;
*
*        was_all_enabled = R_OSPL_DisableAllInterrupt();
*
*        // All interrupt disabled
*
*        if ( was_all_enabled )
*            { R_OSPL_EnableAllInterrupt(); }
*    }
*    @endcode
*/
bool_t    R_OSPL_DisableAllInterrupt(void);


/**
* @brief   Sets the priority of the interrupt line.
*
* @param   IRQ_Num Interrupt request number
* @param   Priority Priority. The less the prior.
* @return  Error code.  If there is no error, the return value is 0
*/
errnum_t  R_OSPL_SetInterruptPriority( bsp_int_src_t const  IRQ_Num, int_fast32_t const  Priority );


/* Section: Locking channel */
/**
* @brief   Locks by channel number.
*
* @param   ChannelNum Locking channel number or "R_OSPL_UNLOCKED_CHANNEL"
* @param   out_ChannelNum Output: Locked channel number, (in) NULL is permitted
* @param   HardwareIndexMin Hardware index of channel number = 0
* @param   HardwareIndexMax Hardware index of max channel number
* @return  Error code.  If there is no error, the return value is 0
*
* @par Description
*    This function is called from the internal of "R_DRIVER_Initialize"
*    function or "R_DRIVER_LockChannel" function.
*    This function calls "R_BSP_HardwareLock".
*/
errnum_t  R_OSPL_LockChannel( int_fast32_t ChannelNum, int_fast32_t *out_ChannelNum,
                              mcu_lock_t  HardwareIndexMin,  mcu_lock_t  HardwareIndexMax );


/**
* @brief   Unlocks by channel number.
*
* @param   ChannelNum Channel number
* @param   e Raising error code, If there is no error, 0
* @param   HardwareIndexMin Hardware index of channel number = 0
* @param   HardwareIndexMax Hardware index of max channel number
* @return  Error code.  If there is no error, the return value is 0
*
* @par Description
*    This function is called from the internal of "R_DRIVER_Finalize"
*    function or "R_DRIVER_UnlockChannel" function.
*    This function calls "R_BSP_HardwareUnlock".
*/
errnum_t  R_OSPL_UnlockChannel( int_fast32_t ChannelNum,  errnum_t  e,
                                mcu_lock_t  HardwareIndexMin,  mcu_lock_t  HardwareIndexMax );


/***********************************************************************
* Class: r_ospl_c_lock_t
************************************************************************/

/**
* @brief   Initializes the C-lock object
*
* @param   self C-lock object
* @return  None
*
* @par Description
*    If *self is global variable or static variable initialized 0,
*    this function does not have to be called.
*/
void      R_OSPL_C_LOCK_InitConst( r_ospl_c_lock_t *const  self );


/**
* @brief   Locks the target, if lockable state.
*
* @param   self C-lock object
* @return  Error code.  If there is no error, the return value is 0.
*
* @par Description
*    Even if lock owner called this function, if lock object was already
*    locked, E_ACCESS_DENIED error is raised.
*
*    "R_OSPL_C_LOCK_Lock" does not do exclusive control.
*/
errnum_t  R_OSPL_C_LOCK_Lock( r_ospl_c_lock_t *const  self );


/**
* @brief   Unlocks the target.
*
* @param   self C-lock object
* @return  Error code.  If there is no error, the return value is 0.
*
* @par Description
*    If this function was called with unlocked object, this function
*    does nothing and raises "E_ACCESS_DENIED" error.
*
*    If self == NULL, this function does nothing and raises no error.
*    E_NOT_THREAD error is raised, if this function was called from the
*    interrupt context.
*
*    - I - lock does not do in this function.
*
*    "R_OSPL_C_LOCK_Unlock" does not do exclusive control.
*/
errnum_t  R_OSPL_C_LOCK_Unlock( r_ospl_c_lock_t *const  self );


/***********************************************************************
* Class: r_ospl_i_lock_vtable_t
************************************************************************/

/**
* @brief   Do nothing. This is registered to r_ospl_i_lock_vtable_t::Lock.
*
* @param   self_ I-lock object
* @return  false
*/
bool_t    R_OSPL_I_LOCK_LockStub( void *const  self_ );


/**
* @brief   Do nothing. This is registered to r_ospl_i_lock_vtable_t::Unlock.
*
* @param   self_ I-lock object
* @return  None
*/
void      R_OSPL_I_LOCK_UnlockStub( void *const  self_ );


/**
* @brief   Do nothing. This is registered to r_ospl_i_lock_vtable_t::RequestFinalize.
*
* @param   self_ I-lock object
* @return  None
*/
void      R_OSPL_I_LOCK_RequestFinalizeStub( void *const  self_ );


/**
* @brief   Get root channel number
*
* @param   self <r_ospl_caller_t> object
* @return  Root channel number
*/
INLINE int_fast32_t  R_OSPL_CALLER_GetRootChannelNum( const r_ospl_caller_t *const  self )
{
    int_fast32_t  root_channel_num;

    IF_DQ( self == NULL ) {
        root_channel_num = 0;
    }
    else {
        root_channel_num = self->I_LockVTable->GetRootChannelNum( self->I_Lock );
    }

    return  root_channel_num;
}


/* Section: Memory Operation */
/**
* @brief   Flushes cache memory
*
* @param   FlushType The operation of flush
* @return  None
*
* @par Description
*    Call the function of the driver after flushing input output buffer
*    in the cache memory, If the data area accessing by the hardware is
*    on cache and the driver did not manage the cache memory.
*    Whether the driver manages the cache memory is depend on the driver
*    specification.
*/
void  R_OSPL_MEMORY_Flush( r_ospl_flush_t const  FlushType );


/**
* @brief   Flushes cache memory with the range of virtual address.
*
* @param   FlushType The operation of flush
* @return  None
*
* @par Description
*    Align "StartAddress" argument and "Length" argument to cache line size.
*    If not aligned, E_OTHERS error is raised.
*    Refer to : R_OSPL_MEMORY_GetSpecification
*
*    If the data area written by the hardware and read from CPU was in cache
*    rea, when the hardware started without invalidate
*    ("R_OSPL_FLUSH_WRITEBACK_INVALIDATE" or "R_OSPL_FLUSH_INVALIDATE"),
*    invalidate the data area and read it after finished to write by hardware.
*    (If the driver does not manage the cache memory.)
*/
errnum_t  R_OSPL_MEMORY_RangeFlush( r_ospl_flush_t const  FlushType,
                                    const void *const  StartAddress,  size_t const  Length );


/**
* @brief   Gets the specification about memory and cache memory.
*
* @param   out_MemorySpec The specification about memory and cache memory
* @return  None
*/
void      R_OSPL_MEMORY_GetSpecification( r_ospl_memory_spec_t *const  out_MemorySpec );


/**
* @brief   Set a memory barrier.
*
* @par Parameters
*    None
* @return  None
*
* @par Description
*    In ARM, This function calls DSB assembler operation.
*    This effects to L1 cache only.
*/
void      R_OSPL_MEMORY_Barrier(void);


/**
* @brief   Set a instruction barrier.
*
* @par Parameters
*    None
* @return  None
*
* @par Description
*    In ARM, This function calls ISB assembler operation.
*/
void      R_OSPL_InstructionSyncBarrier(void);


/**
* @brief   Changes to physical address
*
* @param   Address Virtual address
* @param   out_PhysicalAddress Output: Physical address
* @return  Error code.  If there is no error, the return value is 0.
*
* @par Description
*    This function must be modified by MMU setting.
*/
errnum_t  R_OSPL_ToPhysicalAddress( const volatile void *const  Address, uintptr_t *const  out_PhysicalAddress );


/**
* @brief   Changes to the address in the L1 cache area
*
* @param   Address Virtual address
* @param   out_CachedAddress Output: Virtual address for cached area
* @return  Error code.  If there is no error, the return value is 0.
*
* @par Description
*    This function must be modified by MMU setting.
*    If "E_ACCESS_DENIED" error was raised, you may know the variable by
*    looking at value of "Address" argument and map file.
*/
errnum_t  R_OSPL_ToCachedAddress( const volatile void *const  Address, void *const  out_CachedAddress );


/**
* @brief   Changes to the address in the L1 uncached area
*
* @param   Address Virtual address
* @param   out_UncachedAddress Output: Virtual address for uncached area
* @return  Error code.  If there is no error, the return value is 0.
*
* @par Description
*    This function must be modified by MMU setting.
*    If "E_ACCESS_DENIED" error was raised, you may know the variable by
*    looking at value of "Address" argument and map file.
*/
errnum_t  R_OSPL_ToUncachedAddress( const volatile void *const  Address, void *const  out_UncachedAddress );


/**
* @brief   Gets the level of cache for flushing the memory indicated by the address.
*
* @param   Address The address in flushing memory
* @param   out_Level Output: 0=Not need to flush, 1=L1 cache only, 2=both of L1 and L2 cache
* @return  Error code.  If there is no error, the return value is 0.
*/
errnum_t  R_OSPL_MEMORY_GetLevelOfFlush( const void *Address, int_fast32_t *out_Level );


/**
* @brief   Get 2nd cache attribute of AXI bus for peripheral (not CPU) from physical address.
*
* @param   PhysicalAddress The physical address in the memory area
* @param   out_CacheAttribute Output: Cache_attribute, AWCACHE[3:0], ARCACHE[3:0]
* @return  Error code.  If there is no error, the return value is 0.
*/
errnum_t  R_OSPL_AXI_Get2ndCacheAttribute( uintptr_t const  PhysicalAddress,
        r_ospl_axi_cache_attribute_t *const  out_CacheAttribute );


/**
* @brief   Gets protection attribute of AXI bus from the address
*
* @param   PhysicalAddress The physical address in the memory area
* @param   out_CacheAttribute Output: The protection attribute of AXI bus AWPROT[2:0], ARPROT[2:0]
* @return  Error code.  If there is no error, the return value is 0.
*/
errnum_t  R_OSPL_AXI_GetProtection( uintptr_t const  physical_address,
                                    r_ospl_axi_protection_t *const  out_protection );


/* Section: Timer */
/**
* @brief   Waits for a while until passed time
*
* @param   DelayTime_msec Time of waiting (millisecond)
* @return  Error code.  If there is no error, the return value is 0.
*
* @par Description
*    Maximum value is "R_OSPL_MAX_TIME_OUT" (=65533).
*/
errnum_t  R_OSPL_Delay( uint32_t const  DelayTime_msec );


/**
* @brief   Set up the free running timer
*
* @param   out_Specification NULL is permitted. Output: The precision of the free run timer
* @return  Error code.  If there is no error, the return value is 0.
*
* @par Description
*    The free running timer does not stop.
*
*    If the counter of the free running timer was overflow, the counter returns to 0.
*    Even in interrupt handler, the counter does count up.
*    OSPL free running timer does not use any interrupt.
*
*    Using timer can be selected by "R_OSPL_FTIMER_IS" macro.
*
*    If the free running timer was already set up, this function does not set up it,
*    outputs to "out_Specification" argument and does not raise any error.
*
*    When OSPL API function with timeout or "R_OSPL_Delay" function was called,
*    "R_OSPL_FTIMER_InitializeIfNot" function is callbacked from these functions.
*
*    There is all interrupt disabled area inside.
*/
errnum_t  R_OSPL_FTIMER_InitializeIfNot(  r_ospl_ftimer_spec_t *const  out_Specification );


/**
* @brief   Gets the specification of free running timer.
*
* @param   out_Specification Output: The precision of the free run timer
* @return  None
*/
void      R_OSPL_FTIMER_GetSpecification( r_ospl_ftimer_spec_t *const  out_Specification );


/**
* @brief   Get current time of free running timer.
*
* @par Parameters
*    None
* @return  The current clock count of free run timer
*
* @par Description
*    Call "R_OSPL_FTIMER_InitializeIfNot" function before calling this function.
*    Call "R_OSPL_FTIMER_IsPast" function, when it is determined whether time passed.
*
* @par Example
*    @code
*    errnum_t              e;
*    r_ospl_ftimer_spec_t  ts;
*    uint32_t              start;
*    uint32_t              end;
*
*    e= R_OSPL_FTIMER_InitializeIfNot( &ts ); IF(e){goto fin;}
*    start = R_OSPL_FTIMER_Get();
*
*    // The section of measuring
*
*    end = R_OSPL_FTIMER_Get();
*    printf( "%d msec\n", R_OSPL_FTIMER_CountToTime(
*            &ts, end - start ) );
*    @endcode
*/
uint32_t  R_OSPL_FTIMER_Get(void);


/**
* @brief   Returns whether specified time was passed
*
* @param   ts Precision of the free running timer
* @param   Now Count of current time
* @param   TargetTime Count of target time
* @param   out_IsPast Output: Whether the target time was past or not
* @return  Error code.  If there is no error, the return value is 0.
*/
errnum_t  R_OSPL_FTIMER_IsPast( const r_ospl_ftimer_spec_t *const  ts,
                                uint32_t const  Now,  uint32_t const  TargetTime,  bool_t *const  out_IsPast );


/**
* @brief   Change from mili-second unit to free running timer unit
*
* @param   ts Precision of the free running timer
* @param   msec The value of mili-second unit
* @return  The value of free running timer unit
*
* @par Description
*    The fractional part is been round up. (For waiting time must be more
*    than specified time.)
*
*    This function calculates like the following formula.
*    @code
*    ( msec * ts->msec_Denominator + ts->msec_Numerator - 1 ) / ts->msec_Numerator
*    @endcode
*
*    - Attention: If "ts - >msec_Denominator" was more than "ts->msec_Numerator",
*    take care of overflow.
*/
INLINE uint32_t  R_OSPL_FTIMER_TimeToCount( const r_ospl_ftimer_spec_t *const  ts,
        uint32_t const  msec )
{
    uint32_t  count;

    IF_DQ( ts == NULL ) {
        count = 0;
    }
    else {
        count = ( ((msec * ts->msec_Denominator) + ts->msec_Numerator) - 1u ) / ts->msec_Numerator;
    }
    return  count;
}


/**
* @brief   Change from free running timer unit to mili-second unit
*
* @param   ts Precision of the free running timer
* @param   Count The value of free running timer unit
* @return  The value of mili-second unit
*
* @par Description
*    The fractional part is been round down. (Because overflow does not
*    occur, when "Count = r_ospl_ftimer_spec_t::MaxCount" )
*
*    This function calculates like the following formula.
*    @code
*    ( Count * ts->msec_Numerator ) / ts->msec_Denominator
*    @endcode
*/
INLINE uint32_t  R_OSPL_FTIMER_CountToTime( const r_ospl_ftimer_spec_t *const  ts,
        uint32_t const  Count )
{
    uint32_t  time;

    IF_DQ( ts == NULL ) {
        time = 0;
    }
    else {
        time = ( Count * ts->msec_Numerator ) / ts->msec_Denominator;
    }
    return  time;
}


/***********************************************************************
* Class: r_ospl_table_t
************************************************************************/

/**
* @brief   Initializes an index table
*
* @param   self Index table object
* @param   Area First address of the index table
* @param   AreaByteSize Size of the index table. See <R_OSPL_TABLE_SIZE>
* @param   Is_T_Lock Whether to call <R_OSPL_Start_T_Lock>
* @return  None
*/
void  R_OSPL_TABLE_InitConst( r_ospl_table_t *const  self,
                              void *const  Area, size_t const  AreaByteSize, bool_t const  Is_T_Lock );


/**
* @brief   Returns index from related key
*
* @param   self Index table object
* @param   Key Key number
* @param   out_Index Output: Related index
* @param   TypeOfIfNot Behavior when key was not registerd. See <r_ospl_if_not_t>
* @return  Error code.  If there is no error, the return value is 0.
*/
errnum_t  R_OSPL_TABLE_GetIndex( r_ospl_table_t *const  self, const void *const  Key,
                                 int_fast32_t *const  out_Index,  r_ospl_if_not_t const  TypeOfIfNot );


/**
* @brief   Separates relationship of specified key and related index
*
* @param   self Index table object
* @param   Key Key number
* @return  None
*
* @par Description
*    Error is not raised, even if specified key was already separated.
*/
void  R_OSPL_TABLE_Free( r_ospl_table_t *const  self, const void *const  Key );


/**
* @brief   Print status of specified index table object (for debug)
*
* @param   self Index table object
* @return  None
*/
#if R_OSPL_DEBUG_TOOL
void  R_OSPL_TABLE_Print( r_ospl_table_t *const  self );
#endif


/* Section: Bit flags */
/**
* @brief   Evaluate whether any passed bits are 1 or not
*
* @param   Variable The value of target bit flags
* @param   ConstValue The value that investigating bits are 1
* @return  Whether the any passed bit are 1
*/
/* ->MISRA 19.7 : For return _Bool type */ /* ->SEC M5.1.3 */
#define  IS_BIT_SET( Variable, ConstValue ) \
	( BIT_And_Sub( Variable, ConstValue ) != 0u )
/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */


/**
* @brief   Evaluate whether any passed bits are 1 or not
*
* @param   Variable The value of target bit flags
* @param   OrConstValue The value that investigating bits are 1
* @return  Whether the any passed bit are 1
*/
/* ->MISRA 19.7 : For return _Bool type */ /* ->SEC M5.1.3 */
#define  IS_ANY_BITS_SET( Variable, OrConstValue ) \
	( BIT_And_Sub( Variable, OrConstValue ) != 0u )
/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */


/**
* @brief   Evaluate whether all passed bits are 1 or not
*
* @param   Variable The value of target bit flags
* @param   OrConstValue The value that investigating bits are 1
* @return  Whether the all passed bit are 1
*/
/* ->MISRA 19.7 : For return _Bool type */ /* ->SEC M5.1.3 */
#define  IS_ALL_BITS_SET( Variable, OrConstValue ) \
	( BIT_And_Sub( Variable, OrConstValue ) == (OrConstValue) )
/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */


/**
* @brief   Evaluate whether the passed bit is 0 or not
*
* @param   Variable The value of target bit flags
* @param   ConstValue The value that investigating bit is 1
* @return  Whether the passed bit is 0
*/
/* ->MISRA 19.7 : For return _Bool type */ /* ->SEC M5.1.3 */
#define  IS_BIT_NOT_SET( Variable, ConstValue ) \
	( BIT_And_Sub( Variable, ConstValue ) == 0u )
/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */


/**
* @brief   Evaluate whether any passed bits are 0 or not
*
* @param   Variable The value of target bit flags
* @param   OrConstValue The value that investigating bits are 1
* @return  Whether the any passed bit are 0
*/
/* ->MISRA 19.7 : For return _Bool type */ /* ->SEC M5.1.3 */
#define  IS_ANY_BITS_NOT_SET( Variable, OrConstValue ) \
	( BIT_And_Sub( Variable, OrConstValue ) != (OrConstValue) )
/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */


/**
* @brief   Evaluate whether all passed bits are 0 or not
*
* @param   Variable The value of target bit flags
* @param   OrConstValue The value that investigating bits are 1
* @return  Whether the all passed bit are 0
*/
/* ->MISRA 19.7 : For return _Bool type */ /* ->SEC M5.1.3 */
#define  IS_ALL_BITS_NOT_SET( Variable, OrConstValue ) \
	( BIT_And_Sub( Variable, OrConstValue ) == 0u )
/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */


/**
* @brief   Sub routine of bitwise operation
*
* @param   Variable The value of target bit flags
* @param   ConstValue The value that investigating bits are 1
* @return  Whether the all passed bit are 0
*
* @par Description
*    - This part is for compliant to MISRA 2004 - 19.7.
*/
INLINE uint_fast32_t  BIT_And_Sub( bit_flags_fast32_t const  Variable,
                                   bit_flags_fast32_t const  ConstValue )
{
    return  ((Variable) & (ConstValue));
}


/***********************************************************************
* About: IS_BIT_SET__Warning
*
* - This is for QAC-3344 warning : MISRA 13.2 Advice : Tests of a value against
*   zero should be made explicit, unless the operand is effectively Boolean.
* - This is for QAC-1253 warning : SEC M1.2.2 : A "U" suffix shall be applied
*   to all constants of unsigned type.
************************************************************************/


/* Section: Error handling and debugging (2) */
/**
* @brief   Breaks here
*
* @par Parameters
*    None
* @return  None
*
* @par Description
*    Does break by calling "R_DebugBreak" function.
*    This macro is not influenced the setting of "R_OSPL_ERROR_BREAK" macro.
*/
#define  R_DEBUG_BREAK()  R_DebugBreak(__FILE__,__LINE__)


/**
* @brief   The function callbacked from OSPL for breaking
*
* @param   Variable The value of target bit flags
* @param   ConstValue The value that investigating bits are 1
* @return  Whether the all passed bit are 0
*
* @par Description
*    Set a break point at this function.
*    In Release configuration, "File = NULL, Line = 0".
*    If "File = NULL", "Line" argument is error code.
*    This function can be customized by application developer.
*/
void  R_DebugBreak( const char_t *const  File,  int_fast32_t const  Line );


/**
* @brief   Breaks here, if it is error state
*
* @par Parameters
*    None
* @return  None
*
* @par Description
*    This function does nothing, if "R_OSPL_ERROR_BREAK" macro was defined
*    to be 0. The following descriptions are available, if "R_OSPL_ERROR_BREAK"
*    macro was defined to be 1.
*
*    Checks the error state of the current thread.
*    Call this macro from the last of each thread.
*    Does break by calling "R_DebugBreak" function.
*
*    If an error was raised, this function calls "printf" with following message.
*    Set "error_ID" to "R_OSPL_SET_BREAK_ERROR_ID"
*    @code
*    <ERROR error_ID="0x1" file="../src/api.c(336)"/>
*    @endcode
*/
#if R_OSPL_ERROR_BREAK
#define  R_DEBUG_BREAK_IF_ERROR()  R_OSPL_DebugBreakIfError(__FILE__,__LINE__)
void  R_OSPL_DebugBreakIfError( const char_t *const  File,  int_fast32_t const  Line );
#else
INLINE void  R_DEBUG_BREAK_IF_ERROR(void) {}
#endif


/**
* @brief   Raises the error of system unrecoverable
*
* @param   e Error code
* @return  None
*
* @par Description
*    The error of system unrecoverable is the error of impossible to
*    - self - recover by process or main system. Example, the heap area was
*    broken or there are not any responses from hardware. This error can
*    be recoverable by OS or the system controller(e.g. Software reset)
*
*    Example, when an error of recovery process was raised,
*    "R_OSPL_RaiseUnrecoverable" function must be called.
*
*    "R_OSPL_RaiseUnrecoverable" function can be customized by the
*    application. By default, it calls "R_DebugBreak" function and falls
*    into the infinite loop.
*/
void  R_OSPL_RaiseUnrecoverable( errnum_t const  e );


/**
* @brief   Merge the error code raised in the finalizing operation
*
* @param   CurrentError Current error code
* @param   AppendError New append error code
* @return  Merged error code
*
* @par Description
*    When the state was error state, if other new error was raised,
*    new error code is ignored.
*    - If "CurrentError != 0", this function returns "CurrentError" argument.
*    - If "CurrentError == 0", this function returns "AppendError" argument.
*
*    This function can be modify by user.
*
* @par Example
*    @code
*    ee= Sample();
*    e= R_OSPL_MergeErrNum( e, ee );
*    return  e;
*    @endcode
*/
INLINE errnum_t  R_OSPL_MergeErrNum( errnum_t const  CurrentError,  errnum_t const  AppendError )
{
    errnum_t  e;

    if ( CurrentError != 0 ) {
        e = CurrentError;
    } else {
        e = AppendError;
    }
    return  e;
}


/**
* @brief   Sets an error code to TLS (Thread Local Storage).
*
* @param   e Raising error code
* @return  None
*
* @par Description
*    Usually error code is returned. If API function cannot return any
*    error code, API function can have the specification of setting error
*    code by "R_OSPL_SetErrNum".
*
*    There is this function, if "R_OSPL_TLS_ERROR_CODE" macro was defined
*    to be 1.
*    This function does nothing, if any error code was stored already in TLS.
*    The state does not change to error state, if "R_OSPL_SetErrNum" function
*    was called only. See "R_OSPL_GET_ERROR_ID".
*/
#if R_OSPL_TLS_ERROR_CODE
void  R_OSPL_SetErrNum( errnum_t const  e );
#endif


/**
* @brief   Returns the error code from TLS (Thread Local Storage).
*
* @par Parameters
*    None
* @return  Error code
*
* @par Description
*    Usually error code is returned. If API function cannot return any
*    error code, API function may have the specification of getting error
*    code by "R_OSPL_GetErrNum".
*
*    There is this function, if "R_OSPL_TLS_ERROR_CODE" macro was defined
*    to be 1. This function returns 0 after called "R_OSPL_CLEAR_ERROR"
*    function.
*/
#if R_OSPL_TLS_ERROR_CODE
errnum_t  R_OSPL_GetErrNum(void);
#endif


/**
* @brief   Clears the error state
*
* @par Parameters
*    None
* @return  None
*
* @par Description
*    This function does nothing, if "R_OSPL_ERROR_BREAK" macro and
*    "R_OSPL_TLS_ERROR_CODE" macro were defined to be 0. The following
*    descriptions are available, if "R_OSPL_ERROR_BREAK" macro was
*    defined to be 1.
*
*    Whether the state is the error state is stored in thread local
*    storage. "R_OSPL_GetErrNum" function returns 0 after called this
*    function.
*
*    If the error state was not cleared, the following descriptions were caused.
*    - Breaks at "R_DEBUG_BREAK_IF_ERROR" macro
*    - "R_OSPL_SET_BREAK_ERROR_ID" function behaves not expected behavior
*       because the count of error is not counted up.
*/
#if R_OSPL_ERROR_BREAK  ||  R_OSPL_TLS_ERROR_CODE
void  R_OSPL_CLEAR_ERROR(void);
#else
INLINE void  R_OSPL_CLEAR_ERROR(void) {}  /* QAC 3138 */
#endif


/**
* @brief   Returns the number of current error
*
* @par Parameters
*    None
* @return  The number of current error
*
* @par Description
*    This function does nothing, if "R_OSPL_ERROR_BREAK" macro was defined
*    to be 0. The following descriptions are available, if "R_OSPL_ERROR_BREAK"
*    macro was defined to be 1.
*
*    This function returns 0, if any errors were not raised.
*
*    This function returns 1, if first error was raised.
*
*    After that, this function returns 2, if second error was raised after
*    calling "R_OSPL_CLEAR_ERROR" function.
*    This function does not return 0 after that the error was cleared by
*    calling "R_OSPL_CLEAR_ERROR".
*    The number of current error is running number in the whole of system
*    (all threads).
*
*    Error is raised by following macros.
*    @code
*    IF, IF_D, ASSERT_R, ASSERT_D
*    @endcode
*    The process breaks at a moment of error raised, if the number of current
*    error was set to "R_OSPL_SET_BREAK_ERROR_ID" macro.
*/
#if R_OSPL_ERROR_BREAK
int_fast32_t  R_OSPL_GET_ERROR_ID(void);
#else
INLINE int_fast32_t  R_OSPL_GET_ERROR_ID(void)
{
    return -1;
}
#endif


/**
* @brief   Register to break at raising error at the moment
*
* @param   ID Breaking number of error
* @return  None
*
* @par Description
*    This function does nothing, if "R_OSPL_ERROR_BREAK" macro was defined
*    to be 0. The following descriptions are available, if "R_OSPL_ERROR_BREAK"
*    macro was defined to be 1.
*
*    Set a break point at "R_DebugBreak" function, when the process breaks
*    at the error raised code.
*
*    The number of "ErrorID" argument can be known by "R_DEBUG_BREAK_IF_ERROR"
*    macro or "R_OSPL_GET_ERROR_ID" macro.
*    - In multi - threading environment, the number of "ErrorID" argument is the
*    number of raised errors in all threads. But when "ErrorID" argument was
*    set to be over 2, call "R_OSPL_SET_DEBUG_WORK" function before calling
*    "R_OSPL_SET_BREAK_ERROR_ID" function.
*
*    The following code breaks at first error.
*    @code
*    R_OSPL_SET_BREAK_ERROR_ID( 1 );
*    @endcode
*
*    The following code breaks at next error after resuming from meny errors.
*    @code
*    R_OSPL_SET_BREAK_ERROR_ID( R_OSPL_GET_ERROR_ID() + 1 );
*    @endcode
*/
#if R_OSPL_ERROR_BREAK
void  R_OSPL_SET_BREAK_ERROR_ID( int_fast32_t ID );
#else
INLINE void  R_OSPL_SET_BREAK_ERROR_ID( int_fast32_t const ID )
{
    R_UNREFERENCED_VARIABLE( ID );
}
#endif


/**
* @brief   Set the debug work area
*
* @param   WorkArea Start address of work area
* @param   WorkAreaSize Size of work area (byte). See. <R_OSPL_DEBUG_WORK_SIZE>
* @return  None
*
* @par Description
*    This function does nothing, if "R_OSPL_ERROR_BREAK" macro was defined
*    to be 0. The following descriptions are available, if "R_OSPL_ERROR_BREAK"
*    macro was defined to be 1.
*
*    Set the debug work area, when "R_OSPL_SET_BREAK_ERROR_ID" function
*    supports multi thread. "E_NO_DEBUG_TLS" error is raised, if the debug
*    work area was not set, when errors was raised in 2 or more threads.
*    It is not necessary to call this function, if error handling did by one
*    thread only.
*
* @par Example
*    @code
*    #if R_OSPL_ERROR_BREAK
*    #define  GS_MAX_THREAD  10
*    static uint8_t  gs_DebugWorkArea[ R_OSPL_DEBUG_WORK_SIZE( GS_MAX_THREAD ) ];
*    #endif
*
*    R_OSPL_SET_DEBUG_WORK( gs_DebugWorkArea, sizeof(gs_DebugWorkArea) );
*    @endcode
*/
#if R_OSPL_ERROR_BREAK
void  R_OSPL_SET_DEBUG_WORK( void *WorkArea, uint32_t WorkAreaSize );
#else
INLINE void  R_OSPL_SET_DEBUG_WORK( const void *const WorkArea, uint32_t const WorkAreaSize )
{
    R_UNREFERENCED_VARIABLE_2( WorkArea, WorkAreaSize );
}
#endif


/**
* @brief   Returns debbug information of current thread.
*
* @par Parameters
*    None
* @return  Debbug information of current thread.
*/
#if R_OSPL_ERROR_BREAK
r_ospl_error_t  *R_OSPL_GetCurrentThreadError(void);
#endif


/**
* @brief   Modifies count of objects that current thread has locked.
*
* @param   Plus The value of adding to the counter.
* @return  None
*
* @par Description
*    The counter is subtracted, if this argument was minus.
*
*    Drivers calls this function.
*    This function is not called from OSPL.
*    This function does nothing, if "R_OSPL_ERROR_BREAK" macro is 0.
*/
#if R_OSPL_ERROR_BREAK
#if R_OSPL_IS_PREEMPTION
void         R_OSPL_MODIFY_THREAD_LOCKED_COUNT( int_fast32_t Plus );
#else
INLINE void  R_OSPL_MODIFY_THREAD_LOCKED_COUNT( int_fast32_t Plus ) {}
#endif
#else
INLINE void          R_OSPL_MODIFY_THREAD_LOCKED_COUNT( int_fast32_t Plus ) {}
#endif


/**
* @brief   Returns count of objects that current thread has locked.
*
* @par Parameters
*    None
* @return  Count of objects that current thread has locked
*
* @par Description
*    This function returns 0, if "R_OSPL_ERROR_BREAK" macro is 0.
*/
#if R_OSPL_ERROR_BREAK
#if R_OSPL_IS_PREEMPTION
int_fast32_t  R_OSPL_GET_THREAD_LOCKED_COUNT(void);
#else
INLINE int_fast32_t  R_OSPL_GET_THREAD_LOCKED_COUNT(void)
{
    return 0;
}
#endif
#else
INLINE int_fast32_t  R_OSPL_GET_THREAD_LOCKED_COUNT(void)
{
    return 0;
}
#endif


/* Section: Accessing to register bit field */
/**
* @brief   Reads modifies writes for bit field of 32bit register.
*
* @param   in_out_Register Address of accessing register
* @param   Mask Mask of accessing bit field
* @param   Shift Shift count. Lowest bit number
* @param   Value Writing value before shift to the bit field
* @return  None
*/
#if R_OSPL_BIT_FIELD_ACCESS_MACRO

/* ->SEC R3.6.2(QAC-3345) */
/*    Volatile access at left of "=" and right of "=". But this is not depend on compiler spcifications. */
/* ->SEC M1.2.2(QAC-1259) */
/*    If "Value" is signed, this is depend on CPU bit width. This expects 32bit CPU. But driver code is no problem. */

#define  R_OSPL_SET_TO_32_BIT_REGISTER( in_out_Register, Mask, Shift, Value ) \
	( *(volatile uint32_t*)(in_out_Register) = (uint32_t)( \
		( ((uint32_t) *(volatile uint32_t*)(in_out_Register)) & \
		~(Mask) ) | ( (Mask) & ( ( (uint_fast32_t)(Value) << (Shift) ) & (Mask) ) ) ) )
/* This code is optimized well. */

/* <-SEC M1.2.2(QAC-1259) */
/* <-SEC R3.6.2(QAC-3345) */

#else

INLINE void  R_OSPL_SET_TO_32_BIT_REGISTER( volatile uint32_t *const  Register,
        uint32_t const  Mask,  int_fast32_t const  Shift,  uint32_t const  Value )
{
    uint32_t  reg_value;

    IF_DQ ( Register == NULL ) {}
    else {
        reg_value = *Register;
        reg_value = ( reg_value & ~Mask ) | ( ( Value << Shift ) & Mask );
        *Register = reg_value;
    }
}

#endif


/**
* @brief   Reads modifies writes for bit field of 16bit register.
*
* @param   in_out_Register Address of accessing register
* @param   Mask Mask of accessing bit field
* @param   Shift Shift count. Lowest bit number
* @param   Value Writing value before shift to the bit field
* @return  None
*/
#if R_OSPL_BIT_FIELD_ACCESS_MACRO

/* ->SEC R3.6.2(QAC-3345) */
/*    Volatile access at left of "=" and right of "=". But this is not depend on compiler spcifications. */
/* ->SEC M1.2.2(QAC-1259) */
/*    If "Value" is signed, this is depend on CPU bit width. This expects 32bit CPU. But driver code is no problem. */

#define  R_OSPL_SET_TO_16_BIT_REGISTER( in_out_Register, Mask, Shift, Value ) \
	( *(volatile uint16_t*)(in_out_Register) = (uint16_t)( \
		( ((uint16_t) *(volatile uint16_t*)(in_out_Register)) & \
		~(Mask) ) | ( (Mask) & ( ( (uint_fast16_t)(Value) << (Shift) ) & (Mask) ) ) ) )
/* This code is optimized well. */


/* <-SEC M1.2.2(QAC-1259) */
/* <-SEC R3.6.2(QAC-3345) */

#else

INLINE void  R_OSPL_SET_TO_16_BIT_REGISTER( volatile uint16_t *const  Register,
        uint16_t const  Mask,  int_fast32_t const  Shift,  uint16_t const  Value )
{
    uint16_t  reg_value;

    IF_DQ ( Register == NULL ) {}
    else {
        reg_value = *Register;
        reg_value = (uint16_t)( ( (uint_fast32_t) reg_value & ~(uint_fast32_t) Mask ) |
                                ( ( (uint_fast32_t) Value << Shift ) & (uint_fast32_t) Mask ) );
        /* Cast is for SEC R2.4.2 */
        *Register = reg_value;
    }
}

#endif


/**
* @brief   Reads modifies writes for bit field of 8bit register.
*
* @param   in_out_Register Address of accessing register
* @param   Mask Mask of accessing bit field
* @param   Shift Shift count. Lowest bit number
* @param   Value Writing value before shift to the bit field
* @return  None
*/
#if R_OSPL_BIT_FIELD_ACCESS_MACRO

/* ->SEC R3.6.2(QAC-3345) */
/*    Volatile access at left of "=" and right of "=". But this is not depend on compiler spcifications. */
/* ->SEC M1.2.2(QAC-1259) */
/*    If "Value" is signed, this is depend on CPU bit width. This expects 32bit CPU. But driver code is no problem. */


#define  R_OSPL_SET_TO_8_BIT_REGISTER( in_out_Register, Mask, Shift, Value ) \
	( *(volatile uint8_t*)(in_out_Register) = (uint8_t)( \
		( ((uint8_t) *(volatile uint8_t*)(in_out_Register)) & \
		~(Mask) ) | ( (Mask) & ( ( (uint_fast8_t)(Value) << (Shift) ) & (Mask) ) ) ) )
/* This code is optimized well. */

/* <-SEC M1.2.2(QAC-1259) */
/* <-SEC R3.6.2(QAC-3345) */

#else

INLINE void  R_OSPL_SET_TO_8_BIT_REGISTER( volatile uint8_t *const  Register,
        uint8_t const  Mask,  int_fast32_t const  Shift,  uint8_t const  Value )
{
    uint8_t  reg_value;

    IF_DQ ( Register == NULL ) {}
    else {
        reg_value = *Register;
        reg_value = (uint8_t)( ( (uint_fast32_t) reg_value & ~(uint_fast32_t) Mask ) |
                               ( ( (uint_fast32_t) Value << Shift ) & (uint_fast32_t) Mask ) );
        /* Cast is for SEC R2.4.2 */
        *Register = reg_value;
    }
}

#endif


/**
* @brief   Reads for bit field of 32bit register.
*
* @param   RegisterValueAddress Address of accessing register
* @param   Mask Mask of accessing bit field
* @param   Shift Shift count. Lowest bit number
* @return  Read value after shift
*/
#if R_OSPL_BIT_FIELD_ACCESS_MACRO

/* ->SEC R3.6.2(QAC-3345) */
/*    Volatile access at &(get address), cast and *(memory load). But this is not double volatile access. */
/*    RegisterValueAddress is for avoid QAC-0310,QAC-3345 by cast code at caller. */

#define  R_OSPL_GET_FROM_32_BIT_REGISTER( RegisterValueAddress, Mask, Shift ) \
	( (uint32_t)( ( (uint32_t)*(volatile const uint32_t*) (RegisterValueAddress) \
		&  (uint_fast32_t)(Mask) ) >> (Shift) ) )
/* This code is optimized well. */

/* <-SEC R3.6.2(QAC-3345) */

#else  /* __QAC_ARM_H__ */  /* This code must be tested defined "__QAC_ARM_H__" */


/* This inline functions is not expanded on __CC_ARM 5.15 */
INLINE uint32_t  R_OSPL_GET_FROM_32_BIT_REGISTER( volatile const uint32_t *const  RegisterAddress,
        uint32_t const  Mask,  int_fast32_t const  Shift )
{
    uint32_t  reg_value;

    IF_DQ ( RegisterAddress == NULL ) {
        enum { num = 0x0EDEDEDE };  /* SEC M1.10.1 */
        reg_value = num;
    }
    else {
        reg_value = *RegisterAddress;
        reg_value = ( reg_value & Mask ) >> Shift;
    }
    return  reg_value;
}

#endif


/**
* @brief   Reads for bit field of 16bit register.
*
* @param   RegisterValueAddress Address of accessing register
* @param   Mask Mask of accessing bit field
* @param   Shift Shift count. Lowest bit number
* @return  Read value after shift
*/
#if R_OSPL_BIT_FIELD_ACCESS_MACRO

/* ->SEC R3.6.2(QAC-3345) */
/*    Volatile access at &(get address), cast and *(memory load). But this is not double volatile access. */
/*    RegisterValueAddress is for avoid QAC-0310,QAC-3345 by cast code at caller. */

#define  R_OSPL_GET_FROM_16_BIT_REGISTER( RegisterValueAddress, Mask, Shift ) \
	( (uint16_t)( ( (uint_fast32_t)*(volatile const uint16_t*) (RegisterValueAddress) \
		&  (uint_fast16_t)(Mask) ) >> (Shift) ) )
/* This code is optimized well. */

/* <-SEC R3.6.2(QAC-3345) */

#else  /* __QAC_ARM_H__ */  /* This code must be tested defined "__QAC_ARM_H__" */

/* This inline functions is not expanded on __CC_ARM 5.15 */
INLINE uint16_t  R_OSPL_GET_FROM_16_BIT_REGISTER( volatile const uint16_t *const  RegisterAddress,
        uint16_t const  Mask,  int_fast32_t const  Shift )
{
    uint16_t  reg_value;

    IF_DQ ( RegisterAddress == NULL ) {
        enum { num = 0xDEDE };  /* SEC M1.10.1 */
        reg_value = num;
    }
    else {
        reg_value = *RegisterAddress;
        reg_value = (uint16_t)( ( (uint_fast32_t) reg_value & (uint_fast32_t) Mask ) >> Shift );
        /* Cast is for SEC R2.4.2 */
    }
    return  reg_value;
}

#endif


/**
* @brief   Reads for bit field of 8bit register.
*
* @param   RegisterValueAddress Address of accessing register
* @param   Mask Mask of accessing bit field
* @param   Shift Shift count. Lowest bit number
* @return  Read value after shift
*/
#if R_OSPL_BIT_FIELD_ACCESS_MACRO

/* ->SEC R3.6.2(QAC-3345) */
/*    Volatile access at &(get address), cast and *(memory load). But this is not double volatile access. */
/*    RegisterValueAddress is for avoid QAC-0310,QAC-3345 by cast code at caller. */

#define  R_OSPL_GET_FROM_8_BIT_REGISTER( RegisterValueAddress, Mask, Shift ) \
	( (uint8_t)( ( (uint_fast32_t)*(volatile const uint8_t*) (RegisterValueAddress) \
		&  (uint_fast8_t)(Mask) ) >> (Shift) ) )
/* This code is optimized well. */

/* <-SEC R3.6.2(QAC-3345) */

#else  /* __QAC_ARM_H__ */  /* This code must be tested defined "__QAC_ARM_H__" */

/* This inline functions is not expanded on __CC_ARM 5.15 */
INLINE uint8_t  R_OSPL_GET_FROM_8_BIT_REGISTER( volatile const uint8_t *const  RegisterAddress,
        uint8_t const  Mask,  int_fast32_t const  Shift )
{
    uint8_t  reg_value;

    IF_DQ ( RegisterAddress == NULL ) {
        enum { num = 0xDE };  /* SEC M1.10.1 */
        reg_value = num;
    }
    else {
        reg_value = *RegisterAddress;
        reg_value = (uint8_t)( ( (uint_fast32_t) reg_value & (uint_fast32_t) Mask ) >> Shift );
        /* Cast is for SEC R2.4.2 */
    }
    return  reg_value;
}

#endif


/***********************************************************************
* End of File:
************************************************************************/

#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif /* R_OSPL_H */

