/******************************************************************************
 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the "Company") for its PICmicro(r) Microcontroller is intended and
 supplied to you, the Company's customer, for use solely and
 exclusively on Microchip PICmicro Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
********************************************************************/

;***************************************************************************
; FILE          ret_codes.inc
;
; DESCRIPTION   This file contains the asm return codes for the
;               routines in the dsPIC Cryptographic Library.
;
; AUTHOR        M. H. Etzel, NTRU Cryptosystems, Inc.
;
; DATE          6/20/2003
;
; NOTES
;               - THIS FILE MUST BE IN SYNC WITH ret_codes.h FOR C.
;
; CHANGES
;
;

; return code definitions

.equ MCL_SUCCESS,               0       ;successful completion
.equ MCL_DRBG_NOT_INITIALIZED,  1       ;DRBG not initialized
.equ MCL_ILLEGAL_SIZE,          2       ;illegal size parameter
.equ MCL_ILLEGAL_PADDING,       3       ;illegal padding
.equ MCL_ILLEGAL_PARAMETER,     4       ;illegal parameter
.equ MCL_INVALID_MAC,           5       ;invalid MAC
.equ MCL_INVALID_SIGNATURE,     6       ;invalid signature
.equ MCL_INVALID_CIPHERTEXT,    7       ;invalid ciphertext
