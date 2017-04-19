;/*******************************************************************************
;* DISCLAIMER
;* This software is supplied by Renesas Electronics Corporation and is only
;* intended for use with Renesas products. No other uses are authorized. This
;* software is owned by Renesas Electronics Corporation and is protected under
;* all applicable laws, including copyright laws.
;* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
;* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
;* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
;* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
;* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
;* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
;* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
;* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
;* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
;* Renesas reserves the right, without notice, to make changes to this software
;* and to discontinue the availability of this software. By using this software,
;* you agree to the additional terms and conditions found by accessing the
;* following link:
;* http://www.renesas.com/disclaimer
;* Copyright (C) 2012 - 2014 Renesas Electronics Corporation. All rights reserved.
;*******************************************************************************/
;/*******************************************************************************
;* $FileName: r_ospl_os_less_asm.s $
;* $Module: OSPL $ $PublicVersion: 0.83 $ (=R_OSPL_VERSION)
;* $Rev: 35 $
;* $Date:: 2014-04-15 21:38:18 +0900#$
;* Description : OS Porting Layer API
;******************************************************************************/


@   PRESERVE8
	.section  .text @, CODE, READONLY


;/******************************************************************************
;* Function Name: [R_OSPL_MEMORY_Flush_Sub]
;* Description  :
;* Arguments    : None
;* Return Value : None
;******************************************************************************/

	.global  R_OSPL_MEMORY_Flush_Sub

R_OSPL_MEMORY_Flush_Sub:

	PUSH    {r4-r7}

	MRC     p15, 1, r7, c0, c0, 1   ;// r7 = p15(CLIDR);  // [r4] CLIDR(LoC) * 2  // LoC: level of coherency
	MOV     r4, r7, LSR #23         ;// r4 = r7 >> 23;    //   :
	ANDS    r4, r4, #0xE            ;// r4 &= 0xE;        //   :
	BEQ     Fin                     ;// if ( r4 == 0 ) goto Fin;  // if LoC == 0, no need to clean

	MOV     r3, #0                  ;// for ( r3 = 0;  r3 < r4;  r3 += 2 ) { // [r3]: current_cache_level * 2
Loop1:
	ADD     r2, r3, r3, LSR #1      ;// r2 = r3 + (r3 >> 1);  // r2 = current_cache_level * 3
	MOV     r0, r7, LSR r2          ;// r0 = (r7 >> r2);  // r0 = CLIDR(Ctype#)
	AND     r0, r0, #7              ;// r0 &= 7;          //  :
	CMP     r0, #2                  ;// if ( r0 < 2 ) goto Continue1;
	BLT     Continue1               ;//  :        // If no cache or only instruction cache at this level

	MCR     p15, 2, r3, c0, c0, 0   ;// p15(CSSELR) = r3;    // CSSELR: Cache Size Selection Register
	ISB                             ;// Guarantee MCR operation
	MRC     p15, 1, r0, c0, c0, 0   ;// r0 = p15(CCSIDR);    // CCSIDR: Cache Size ID Registers
	AND     r6, r0, #7              ;// r6 = r0 & 7;         // CCSIDR(LineSize): log2(cache_line_byte)-4
	ADD     r6, r6, #4              ;// r6 += 4;             // [r6] log2(cache_line_byte)
	LDR     r7, =0x3FF              ;// r7 = 0x3FF;          // [r7] CCSIDR(Associativity): associativity_value - 1
	AND     r7, r7, r0, LSR #3      ;// r7 = r7 & r0 >> 3;   //   :
	CLZ     r5, r7                  ;// r5 = CLZ( r7 );      // [r5] log2(WaySize), if exponential 2
	LDR     r2, =0x7FFF             ;// r2 = 0x7FFF;         // CCSIDR(NumSets) : max number of the index size - 1
	AND     r2, r2, r0, LSR #13     ;// r2 = r2 & r0 >> 13;  //  :
	                                ;// for ( /*r2*/;   r2 >= 0;  r2 -= 1 ) {  // [r2] Set number
Loop2:
	MOV     r1, r7                  ;// for ( r1 = r7;  r1 >= 0;  r1 -= 1 ) {  // [r1] Way number
Loop3:
	ORR     r0, r3, r1, LSL r5      ;//     r0 = r3 | (r1 << r5)
	ORR     r0, r0, r2, LSL r6      ;//             | (r2 << r6);
	MCR     p15, 0, r0, c7, c14, 2  ;//     p15(DCCISW) = r0;  // Clean(WriteBack) and Invalidate
	SUBS    r1, r1, #1              ;// }  /* End of r1 loop */
	BGE     Loop3                   ;//  :
	SUBS    r2, r2, #1              ;// }  /* End of r2 loop */
	BGE     Loop2                   ;//  :
Continue1:
	ADD     r3, r3, #2              ;// }  /* End of r3 loop */
	CMP     r4, r3                  ;//  :
	BGT     Loop1                   ;//  :

Fin:
	DSB
	POP    {r4-r7}
	BX     lr


;/******************************************************************************
;* Function Name: [R_OSPL_MEMORY_GetCacheLineSize]
;* Description  :
;* Arguments    : None
;* Return Value : None
;******************************************************************************/

	.global  R_OSPL_MEMORY_GetCacheLineSize

R_OSPL_MEMORY_GetCacheLineSize:

	MRC     p15, 1, r0, c0, c0, 0   ;// r0 = p15(CCSIDR);    // CCSIDR: Cache Size ID Registers
	AND     r0, r0, #7              ;// r0 = r0 & 7;         // CCSIDR.LineSize: log2(cache_line_byte)-4
	ADD     r0, r0, #4              ;// r0 += 4;             //
	MOV     r1, #1                  ;// r0 = 1 << r0;        // [r0] cache_line_byte
	MOV     r0, r1, LSL r0          ;//    :

	BX     lr


;/******************************************************************************
;* Function Name: [R_OSPL_MEMORY_RangeFlush_Sub]
;* Description  :
;* Arguments    : r0: Start address
;*              : r1: Over address (Last + 1)
;*              : r2: Cache line size
;* Return Value : None
;******************************************************************************/

	.global  R_OSPL_MEMORY_RangeFlush_Sub

R_OSPL_MEMORY_RangeFlush_Sub:

Loop:
	CMP     r0, r1                  ;// for ( r0 = r0;  r0 < r1;  r0 += r2 ) {
	BGE     Fin2                    ;//  :
	MCR     p15, 0, r0, c7, c6, 1   ;//     p15(DCIMVAC) = r0;  // Invalidate
	ADD     r0, r0, r2              ;// }
	B       Loop                    ;//  :
Fin2:
	DSB
	BX     lr

	.END



