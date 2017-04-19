/******************************************************************************
* Copyright(c) 2010-2012 Renesas Electronics Corporation. All rights reserved.
*
* brief  : NCG State variable control functions.
*
* author : Renesas Electronics Corporation
*
* history: 2010.10.08
*          - Created the initial code.
*          2013.05.20
*          - Applied to CMSIS-RTOS RTX.
*          2014.12.10
*          - Applied to OSPL and RGA.
*
*******************************************************************************/

/*=============================================================================
 * Includes
 */
#include "Project_Config.h"
#include "r_ospl.h"

#include    "ncg_defs.h"
#include    "ncg_debug.h"
#include    "ncg_state.h"

#ifdef  RGAH_VERSION

/*=============================================================================
 * Internal definitions
 */

#define     NCG_MAX_STATE_OBJECTS   (1U)
#define     NCG_MAX_THREAD_ID       (1U)

struct NCG_STATE_T {
    r_ospl_thread_id_t  id[NCG_MAX_THREAD_ID];
    NCGboolean      bInUse;
    int32_t         flgbit;
    NCGuint32       ui32State;
} ;


/*=============================================================================
 *  Prototyping of internal functions
 */

static NCGint32 ncg_init_states( void ) ;
static NCGuint32 ncg_state_search( NCGvoid *pObj );


/*=============================================================================
 *  Private global variables and functions
 */

volatile static struct NCG_STATE_T tStates[NCG_MAX_STATE_OBJECTS];

static NCGboolean ncg_state_initialized = NCG_FALSE ;
static struct NCG_STATE_T *ncg_last_created_state;


/*=============================================================================
 *  Global Function
 */

/*----------------------------------------------------------------------------
 NAME       : NCGSYS_CreateState
 FUNCTION   : Create a state control object.
 PARAMETERS : ppObj       : [OUT] The pointer to the created state control object.
              ui32StateID : [IN ] The identifier of the state variable.
 RETURN     : Error code of the NCG.
------------------------------------------------------------------------------*/
NCGint32
NCGSYS_CreateState(
    NCGvoid         **ppObj,
    NCGuint32       ui32StateID )
{
    struct NCG_STATE_T  *p_state;
    NCGuint32           state_idx;
    NCGuint32           thread_idx;

    NCG_DEBUG_PRINT_STRING( "[LOG] NCGSYS_CreateState" );

    NCG_UNREFERENCED_PARAMETER(ui32StateID);

    if( ppObj == NCG_NULL ) {
        NCG_DEBUG_MAKE_MSG_START( NCG_G_MASSAGE_BUFF, "[NCG][ERROR]NCGSYS_CreateState(1) invalid ppObj.%s" )
        NCG_DEBUG_MAKE_MSG_PARAMETER( NCG_CRLF )
        NCG_DEBUG_MAKE_MSG_END();
        NCG_DEBUG_PRINT_MSG( NCG_G_MASSAGE_BUFF );
        return NCG_err_invalid_handle ;
    }

    if( ncg_state_initialized == NCG_FALSE ) {
        ncg_init_states();
    }

    p_state = NULL;
    for ( state_idx = 0 ; state_idx < NCG_MAX_STATE_OBJECTS ; state_idx++ ) {
        if ( tStates[state_idx].bInUse == NCG_FALSE ) {
            p_state = (struct NCG_STATE_T *)&tStates[state_idx];
            break ;
        }
    }
    if ( p_state == NULL ) {
        NCG_DEBUG_MAKE_MSG_START( NCG_G_MASSAGE_BUFF, "[NCG][ERROR]NCGSYS_CreateState(2) Object over flow.%s" )
        NCG_DEBUG_MAKE_MSG_PARAMETER( NCG_CRLF )
        NCG_DEBUG_MAKE_MSG_END();
        NCG_DEBUG_PRINT_MSG( NCG_G_MASSAGE_BUFF );
        return NCG_err_resource_exceeded ;
    }

    /* set initial value */
    for ( thread_idx = 1U ; thread_idx < NCG_MAX_THREAD_ID ; thread_idx++ ) {
        p_state->id[thread_idx] = NULL;
    }
    p_state->id[0]     = R_OSPL_THREAD_GetCurrentId();    /* [0] is current thread id */
    p_state->bInUse    = NCG_TRUE ;
    p_state->ui32State = 0U ;

    *ppObj = ( NCGvoid *)p_state ;
    ncg_last_created_state = p_state;

    return NCG_no_err ;
}

/*----------------------------------------------------------------------------
 NAME       : NCGSYS_DestroyState
 FUNCTION   : Destroy the given state control object.
 PARAMETERS : pObj : [IN ] The pointer to the state control object.
 RETURN     : Error code of the NCG.
------------------------------------------------------------------------------*/
NCGint32
NCGSYS_DestroyState(
    NCGvoid *pObj )
{
    volatile struct NCG_STATE_T *p_state;
    NCGuint32                   state_idx;
    NCGuint32                   thread_idx;
    NCGboolean bDestroyFlg ;

    NCG_DEBUG_PRINT_STRING( "[LOG] NCGSYS_DestroyState" );

    p_state = (struct NCG_STATE_T *)pObj;
    if( p_state == NCG_NULL ) {
        NCG_DEBUG_MAKE_MSG_START( NCG_G_MASSAGE_BUFF, "[NCG][ERROR]NCGSYS_DestroyState(1) Failed: pObj is NULL.%s" )
        NCG_DEBUG_MAKE_MSG_PARAMETER( NCG_CRLF )
        NCG_DEBUG_MAKE_MSG_END();
        NCG_DEBUG_PRINT_MSG( NCG_G_MASSAGE_BUFF );
        return NCG_err_invalid_handle ;
    }

    state_idx = ncg_state_search( pObj );
    if ( state_idx >= NCG_MAX_STATE_OBJECTS ) {
        NCG_DEBUG_MAKE_MSG_START( NCG_G_MASSAGE_BUFF, "[NCG][ERROR]NCGSYS_DestroyState(2) Failed: pObj is not found.%s" )
        NCG_DEBUG_MAKE_MSG_PARAMETER( NCG_CRLF )
        NCG_DEBUG_MAKE_MSG_END();
        NCG_DEBUG_PRINT_MSG( NCG_G_MASSAGE_BUFF );
        return NCG_err_not_found ;
    }

    if( p_state->bInUse == NCG_FALSE ) {
        NCG_DEBUG_MAKE_MSG_START( NCG_G_MASSAGE_BUFF, "[NCG][ERROR]NCGSYS_DestroyState(3) Failed: pObj is not used.%s" )
        NCG_DEBUG_MAKE_MSG_PARAMETER( NCG_CRLF )
        NCG_DEBUG_MAKE_MSG_END();
        NCG_DEBUG_PRINT_MSG( NCG_G_MASSAGE_BUFF );
        return NCG_err_bad_parameter ;
    }

    /* clear status parameters */
    for ( thread_idx = 0 ; thread_idx < NCG_MAX_THREAD_ID ; thread_idx++ ) {
        p_state->id[thread_idx] = NULL;
    }
    p_state->bInUse    = NCG_FALSE;
    p_state->ui32State = 0U;

    bDestroyFlg = NCG_TRUE ;
    for ( state_idx = 0 ; state_idx < NCG_MAX_STATE_OBJECTS ; state_idx++ ) {
        if ( tStates[state_idx].bInUse != NCG_FALSE ) {
            bDestroyFlg = NCG_FALSE ;
            break ;
        }
    }
    if ( bDestroyFlg != NCG_FALSE ) {
        ncg_state_initialized = NCG_FALSE ;
    }
    return NCG_no_err ;
}

/*----------------------------------------------------------------------------
 NAME       : NCGSYS_SetState
 FUNCTION   : Set the state control object into the given state.
 PARAMETERS : pObj      : [IN ] The pointer to the state control object.
              ui32State : [IN ] The state value to set.
              ui32Flags : [IN ] Flags.
 RETURN     : Error code of the NCG.
------------------------------------------------------------------------------*/
NCGint32
NCGSYS_SetState(
    NCGvoid         *pObj,
    NCGuint32       ui32State,
    NCGuint32       ui32Flags )
{
    volatile struct NCG_STATE_T *p_state = (volatile struct NCG_STATE_T *)pObj ;

    NCGuint32                   uiSetMask;
    NCGuint32                   thread_idx;

    NCG_DEBUG_PRINT_STRING( "[LOG] NCGSYS_SetState" );

    if( pObj == NCG_NULL ) {
        NCG_DEBUG_MAKE_MSG_START( NCG_G_MASSAGE_BUFF, "[NCG][ERROR]NCGSYS_SetState(1) Failed:pObj is NULL.%s" )
        NCG_DEBUG_MAKE_MSG_PARAMETER( NCG_CRLF )
        NCG_DEBUG_MAKE_MSG_END();
        NCG_DEBUG_PRINT_MSG( NCG_G_MASSAGE_BUFF );
        return NCG_err_unknown ;
    }

    if ( ncg_state_search( pObj ) >= NCG_MAX_STATE_OBJECTS ) {
        NCG_DEBUG_MAKE_MSG_START( NCG_G_MASSAGE_BUFF, "[NCG][ERROR]NCGSYS_SetState(2) Failed: pObj is not found.%s" )
        NCG_DEBUG_MAKE_MSG_PARAMETER( NCG_CRLF )
        NCG_DEBUG_MAKE_MSG_END();
        NCG_DEBUG_PRINT_MSG( NCG_G_MASSAGE_BUFF );
        return NCG_err_not_found ;
    }

    p_state = (volatile struct NCG_STATE_T *)pObj;
    if ( p_state->bInUse == NCG_FALSE ) {
        NCG_DEBUG_MAKE_MSG_START( NCG_G_MASSAGE_BUFF, "[NCG][ERROR]NCGSYS_SetState(3) Failed: pObj is not used.%s" )
        NCG_DEBUG_MAKE_MSG_PARAMETER( NCG_CRLF )
        NCG_DEBUG_MAKE_MSG_END();
        NCG_DEBUG_PRINT_MSG( NCG_G_MASSAGE_BUFF );
        return NCG_err_bad_parameter ;
    }

    uiSetMask = ui32Flags & NCGSYS_STATE_SET_MASK;
    switch( uiSetMask ) {
        case NCGSYS_STATE_SET_AND :
            p_state->ui32State &= ui32State ;
            break ;

        case NCGSYS_STATE_SET_OR :
            p_state->ui32State |= ui32State ;
            break ;

        case NCGSYS_STATE_SET_SET :
            p_state->ui32State = ui32State ;
            break ;

        default :
            NCG_DEBUG_MAKE_MSG_START( NCG_G_MASSAGE_BUFF, "[NCG][ERROR]NCGSYS_SetState(5) Failed:ui32Flags (0x%08x).%s" )
            NCG_DEBUG_MAKE_MSG_PARAMETER( ui32Flags )
            NCG_DEBUG_MAKE_MSG_PARAMETER( NCG_CRLF )
            NCG_DEBUG_MAKE_MSG_END();
            NCG_DEBUG_PRINT_MSG( NCG_G_MASSAGE_BUFF );
            return NCG_err_bad_parameter ;
    }

    if ( uiSetMask == NCGSYS_STATE_SET_AND ) {
        for ( thread_idx = 0 ; thread_idx < NCG_MAX_THREAD_ID ; thread_idx++ ) {
            if ( p_state->id[thread_idx] != NULL ) {
                r_ospl_thread_id_t  thread = p_state->id[thread_idx];  /* ICCARM Pa082 : volatile's order */
                R_OSPL_EVENT_Clear( thread, p_state->flgbit );
            }
        }
    } else {
        for ( thread_idx = 0 ; thread_idx < NCG_MAX_THREAD_ID ; thread_idx++ ) {
            if ( p_state->id[thread_idx] != NULL ) {
                r_ospl_thread_id_t  thread = p_state->id[thread_idx];  /* ICCARM Pa082 : volatile's order */
                R_OSPL_EVENT_Set( thread, p_state->flgbit );
            }
        }
    }

    return NCG_no_err ;
}

/*----------------------------------------------------------------------------
 NAME       : NCGSYS_GetState
 FUNCTION   : Get the current state value from the state control object.
 PARAMETERS : pObj       : [IN ] The pointer to the state control object.
              ui32Flags  : [IN ] Flags.
 RETURN     : Error code of the NCG.
------------------------------------------------------------------------------*/
NCGuint32
NCGSYS_GetState(
    NCGvoid         *pObj,
    NCGuint32       ui32Flags )
{
    volatile struct NCG_STATE_T *p_state;

    NCG_DEBUG_PRINT_STRING( "[LOG] NCGSYS_GetState" );

    NCG_UNREFERENCED_PARAMETER(ui32Flags);

    if ( pObj == NCG_NULL ) {
        NCG_DEBUG_MAKE_MSG_START( NCG_G_MASSAGE_BUFF, "[NCG][ERROR]NCGSYS_GetState(1) Failed: pObj.%s" )
        NCG_DEBUG_MAKE_MSG_PARAMETER( NCG_CRLF )
        NCG_DEBUG_MAKE_MSG_END();
        NCG_DEBUG_PRINT_MSG( NCG_G_MASSAGE_BUFF );
        return (NCGuint32)NCG_err_unknown ;
    }

    if ( ncg_state_search( pObj ) >= NCG_MAX_STATE_OBJECTS ) {
        NCG_DEBUG_MAKE_MSG_START( NCG_G_MASSAGE_BUFF, "[NCG][ERROR]NCGSYS_GetState(2) Failed: pObj is not found.%s" )
        NCG_DEBUG_MAKE_MSG_PARAMETER( NCG_CRLF )
        NCG_DEBUG_MAKE_MSG_END();
        NCG_DEBUG_PRINT_MSG( NCG_G_MASSAGE_BUFF );
        return (NCGuint32)NCG_err_not_found ;
    }

    p_state = (volatile struct NCG_STATE_T *)pObj;
    if( p_state->bInUse == NCG_FALSE ) {
        NCG_DEBUG_MAKE_MSG_START( NCG_G_MASSAGE_BUFF, "[NCG][ERROR]NCGSYS_GetState(3) Failed: pObj is not used.%s" )
        NCG_DEBUG_MAKE_MSG_PARAMETER( NCG_CRLF )
        NCG_DEBUG_MAKE_MSG_END();
        NCG_DEBUG_PRINT_MSG( NCG_G_MASSAGE_BUFF );
        return (NCGuint32)NCG_err_bad_parameter ;
    }
    return ( NCGuint32 )p_state->ui32State ;
}

/*----------------------------------------------------------------------------
 NAME       : NCGSYS_WaitState
 FUNCTION   : Wait for the state control object to be in the given state.
 PARAMETERS : pObj        : [IN ] The pointer to the state control object.
              ui32State   : [IN ] The state value to wait for.
              ui32Flags   : [IN ] Flags.
              ui32Timeout : [IN ] Timeout value in milli seconds.
 RETURN     : Error code of the NCG.
------------------------------------------------------------------------------*/
NCGint32
NCGSYS_WaitState(
    NCGvoid         *pObj,
    NCGuint32       ui32State,
    NCGuint32       ui32Flags,
    NCGuint32       ui32Timeout )
{
    volatile struct NCG_STATE_T     *p_state;
    NCGuint32                       wait_mode;
    NCGuint32                       thread_idx;
    NCGint32                        func_ret;
    uint32_t                        time_out;
    errnum_t                        e;


    NCG_DEBUG_PRINT_STRING( "[LOG] NCGSYS_WaitState" );

    p_state    = (volatile struct NCG_STATE_T *)pObj;
    wait_mode = (ui32Flags & NCGSYS_STATE_WAIT_MASK);

    if ( p_state == NCG_NULL ) {
        NCG_DEBUG_MAKE_MSG_START( NCG_G_MASSAGE_BUFF, "[NCG][ERROR]NCGSYS_WaitState(1) Failed: pObj is NULL.%s" )
        NCG_DEBUG_MAKE_MSG_PARAMETER( NCG_CRLF )
        NCG_DEBUG_MAKE_MSG_END();
        NCG_DEBUG_PRINT_MSG( NCG_G_MASSAGE_BUFF );
        return NCG_err_unknown ;
    }
    if ( p_state->bInUse == NCG_FALSE ) {
        NCG_DEBUG_MAKE_MSG_START( NCG_G_MASSAGE_BUFF, "[NCG][ERROR]NCGSYS_WaitState(2) Failed: pObj.%s" )
        NCG_DEBUG_MAKE_MSG_PARAMETER( NCG_CRLF )
        NCG_DEBUG_MAKE_MSG_END();
        NCG_DEBUG_PRINT_MSG( NCG_G_MASSAGE_BUFF );
        return NCG_err_unknown ;
    }
    if (( ui32Timeout != NCG_TIMEOUT_INFINITE ) && ( ui32Timeout > 0x7fffffffU )) {
        NCG_DEBUG_MAKE_MSG_START( NCG_G_MASSAGE_BUFF, "[NCG][ERROR]NCGSYS_WaitState(3) Failed: ui32Timeout.%s" )
        NCG_DEBUG_MAKE_MSG_PARAMETER( NCG_CRLF )
        NCG_DEBUG_MAKE_MSG_END();
        NCG_DEBUG_PRINT_MSG( NCG_G_MASSAGE_BUFF );
        return NCG_err_unknown ;
    }

    if ( ui32Timeout == NCG_TIMEOUT_INFINITE ) {
        time_out = R_OSPL_INFINITE;
    } else {
        time_out = ui32Timeout;
    }

    for ( thread_idx = 0 ; thread_idx < NCG_MAX_THREAD_ID ; thread_idx++ ) {
        if ( p_state->id[thread_idx] == R_OSPL_THREAD_GetCurrentId() ) {
            break;
        }
        if ( p_state->id[thread_idx] == NULL ) {
            /* add thread ID */
            p_state->id[thread_idx] = R_OSPL_THREAD_GetCurrentId();
            break;
        }
    }

    while (1) {
        if ( wait_mode == NCGSYS_STATE_WAIT_OR ) {
            if ( (p_state->ui32State & ui32State) != 0 ) {
                func_ret = NCG_no_err;
                break;
            }
        } else {
            /* wait_mode == NCGSYS_STATE_WAIT_AND */
            if ( (p_state->ui32State & ui32State) == ui32State ) {
                func_ret = NCG_no_err;
                break;
            }
        }
        e = R_OSPL_EVENT_Wait( p_state->flgbit, NULL, time_out );
        IF ( e != 0 ) {
            func_ret = NCG_err_unknown;
            break;
        }
    }

    for ( thread_idx = 1 ; thread_idx < NCG_MAX_THREAD_ID ; thread_idx++ ) {
        p_state->id[thread_idx] = NULL;
    }

    return func_ret;
}


/*=============================================================================
 *  Internal functions
 */

/*----------------------------------------------------------------------------
 NAME       : ncg_init_states
 FUNCTION   : Initialize global variable in NCG state.
 PARAMETERS : None.
 RETURN     : None.
------------------------------------------------------------------------------*/
static NCGint32
ncg_init_states( void )
{
    NCGuint32   i ;
    NCGuint32   thread_idx;

    for( i = 0 ; i < NCG_MAX_STATE_OBJECTS ; i++ ) {
        for ( thread_idx = 0 ; thread_idx < NCG_MAX_THREAD_ID ; thread_idx++ ) {
            tStates[i].id[thread_idx] = NULL;
        }
        tStates[i].bInUse    = NCG_FALSE ;
        tStates[i].flgbit    = (int32_t)(1 << i) ;
        tStates[ i ].ui32State = 0U ;
    }
    ncg_state_initialized = NCG_TRUE ;

    return NCG_no_err;
}

/*----------------------------------------------------------------------------
 NAME       : ncg_state_search
 FUNCTION   : Search status object.
 PARAMETERS : pObj :[IN ] Handle to the status object.
 RETURN     : Index of the status object.
------------------------------------------------------------------------------*/
static NCGuint32 ncg_state_search(
    NCGvoid     *pObj
)
{
    NCGuint32               state_idx;

    for ( state_idx = 0U ; state_idx < NCG_MAX_STATE_OBJECTS ; state_idx++ ) {
        if ( pObj == &tStates[state_idx] ) {
            break;
        }
    }

    return state_idx;
}

/*----------------------------------------------------------------------------
 NAME       : NCGSYS_SetStateEventValue
 FUNCTION   : Set the value of thread attached event.
 PARAMETERS : pObj           : [IN ] The pointer to the state control object.
              ui32EventValue : [IN ] The value of thread attached event.
 RETURN     : None.
------------------------------------------------------------------------------*/
NCGvoid
NCGSYS_SetStateEventValue(
    NCGvoid         *pObj,
    NCGuint32       ui32EventValue )
{
    volatile struct NCG_STATE_T *p_state = (volatile struct NCG_STATE_T *)pObj ;

    p_state->flgbit = ui32EventValue;
}

/*----------------------------------------------------------------------------
 NAME       : NCGSYS_GetLastCreatedState
 FUNCTION   : GetLastCreatedState.
 PARAMETERS : None.
 RETURN     : LastCreatedState.
------------------------------------------------------------------------------*/
NCGvoid *
NCGSYS_GetLastCreatedState(void)
{
    return  (NCGvoid *) ncg_last_created_state;
}

#endif
/* -- end of file -- */
