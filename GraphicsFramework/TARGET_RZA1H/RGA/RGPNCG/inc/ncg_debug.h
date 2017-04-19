/******************************************************************************
* Copyright(c) 2010-2012 Renesas Electronics Corporation. All rights reserved.
*
* brief  : NCG debug header.
*
* author : Renesas Electronics Corporation
*
* history: 2010.10.08
*          - Created the initial code.
*          2012.08.21
*          - Added debugging messase function.
*          2012.08.22
*          - Updated coding format.
*          - Added debug macro.
*            - NCG_DEBUG_MAKE_MSG
*            - NCG_DEBUG_PRINT_MSG
*          2013.01.08
*            - Fixed release code.
*
*******************************************************************************/

#ifndef NCG_DEBUG_H
#define NCG_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------------------------*/
#ifdef NCG_DEBUG
/*-----------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#define NCG_ASSERT(EXPR)                            if (!(EXPR)) { NCGSYS_Abort(__FILE__, __LINE__, #EXPR); }

#define NCG_DEBUG_MAKE_MSG_START( buff, format )    sprintf( (buff), format
#define NCG_DEBUG_MAKE_MSG_PARAMETER( param )           , (param)
#define NCG_DEBUG_MAKE_MSG_END(  )                  )
#define NCG_DEBUG_PRINT_MSG( msg )                  printf( msg )

#ifdef NCG_DEBUG_API_LOG

#define NCG_DEBUG_PRINT_STRING( msg )               printf( "%s%s", (msg), NCG_CRLF )

#else /* #ifdef NCG_DEBUG_API_LOG */

#define NCG_DEBUG_PRINT_STRING( msg )               /* No process */

#endif /* #ifdef NCG_DEBUG_API_LOG */

#define NCG_DEBUG_MSG_BUFFER_SIZE                   512

#define NCG_CRLF                                    "\n"

extern void NCGSYS_Abort(const NCGchar *filename, NCGint32 line, const NCGchar *condition);

extern char NCG_G_MASSAGE_BUFF[NCG_DEBUG_MSG_BUFFER_SIZE];

/*-----------------------------------------------------------------------------------*/
#else   /* NCG_DEBUG */
/*-----------------------------------------------------------------------------------*/

#define NCG_ASSERT(EXPR)                                /* No process */
#define NCG_DEBUG_MAKE_MSG_START( buff, format )        /* No process */
#define NCG_DEBUG_MAKE_MSG_PARAMETER( param )           /* No process */
#define NCG_DEBUG_MAKE_MSG_END(  )                      /* No process */
#define NCG_DEBUG_PRINT_MSG( msg )                      /* No process */

#define NCG_DEBUG_PRINT_STRING( msg )                   /* No process */

/*-----------------------------------------------------------------------------------*/
#endif  /* NCG_DEBUG */
/*-----------------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* NCG_DEBUG_H */
