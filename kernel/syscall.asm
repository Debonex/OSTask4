
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                               syscall.asm
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                                                     Forrest Yu, 2005
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

%include "sconst.inc"

_NR_get_ticks       equ 0 ; 要跟 global.c 中 sys_call_table 的定义相对应！
_NR_delay			equ 1
_NR_print			equ 2
_NR_P				equ 3
_NR_V				equ 4
INT_VECTOR_SYS_CALL equ 0x90

; 导出符号
global	get_ticks
global	syscall_delay
global	syscall_print
global	syscall_P
global  syscall_V

bits 32
[section .text]

; ====================================================================
;                              get_ticks
; ====================================================================
get_ticks:
	mov	eax, _NR_get_ticks
	int	INT_VECTOR_SYS_CALL
	ret


; ====================================================================
;                              sys_delay
; ====================================================================
syscall_delay:
	push ebx
	mov ebx, [esp + 8]
	mov	eax, _NR_delay
	int	INT_VECTOR_SYS_CALL
	pop ebx
	ret

; ====================================================================
;                              sys_print
; ====================================================================
syscall_print:
	push ebx
	mov ebx, [esp + 8]
	mov	eax, _NR_print
	int	INT_VECTOR_SYS_CALL
	pop ebx
	ret

; ====================================================================
;                              syscall_P
; ====================================================================
syscall_P:
	push ebx
	mov ebx, [esp + 8]
	mov	eax, _NR_P
	int	INT_VECTOR_SYS_CALL
	pop ebx
	ret

; ====================================================================
;                              syscall_V
; ====================================================================
syscall_V:
	push ebx
	mov ebx, [esp + 8]
	mov	eax, _NR_V
	int	INT_VECTOR_SYS_CALL
	pop ebx
	ret