

#include "fsm.h"
#include "util.h"
#include "relayball_init.h"
#include "bitbang_relay.h"

#define DMA_CHANNEL3_CNDTR (0x00000030+0x00020000+0x40000000+4)

	.cpu cortex-m0
	.fpu softvfp
	.thumb
	.section	.text.bitbang,"ax",%progbits
	.balign	2
	.global	main
	.code	16
	.thumb_func
	.type	main, %function
	.syntax unified
	.thumb

main:

bl init
ldr r0, =(F_SYS_TICK_CLK/400)
bl enable_sys_tick



#define rSCRATCH  r0
#define rI        r1
#define rPREAD    r2
#define rDATA     r3
#define rNEXT     r4
#define rSTATE    r5

#define rBUFEND   r6
#define rPCNT     r7

#define rFF00     r8
#define rFRAMESZ  r9
#define rCUR      r10
#define rFSM      r11

ldr rSCRATCH, =(recv_buf+RECV_BUF_SZ)
mov rBUFEND, rSCRATCH

ldr rPCNT, =DMA_CHANNEL3_CNDTR

ldr rPREAD, =recv_buf

movs rSCRATCH, #0xff
lsls rSCRATCH, #8
mov rFF00, rSCRATCH

ldr rSCRATCH, =N_BYTES
mov rFRAMESZ, rSCRATCH

ldr rNEXT, =frame_a

ldr rSCRATCH, =frame_b
mov rCUR, rSCRATCH

ldr rSCRATCH, =fsm
mov rFSM, rSCRATCH

next_frame:
	ldr rSCRATCH, =cur
	str rNEXT, [rSCRATCH]
	mov rSCRATCH, rNEXT
	mov rNEXT, rCUR
	mov rCUR, rSCRATCH

ignore_frame:

	movs rI, #0
	read_loop:

	0:
	ldr rSCRATCH, [rPCNT]
	subs rSCRATCH, rBUFEND, rSCRATCH
	cmp rSCRATCH, rPREAD
	beq 0b

    ldrb rDATA, [rPREAD]

	adds rPREAD, #1
	cmp rPREAD, rBUFEND
	bne 1f
	ldr rPREAD, =recv_buf
	1:

	0:
	ldr rSCRATCH, [rPCNT]
	subs rSCRATCH, rBUFEND, rSCRATCH
	cmp rSCRATCH, rPREAD
	beq 0b

    ldrb rSCRATCH, [rPREAD]
	lsls rSCRATCH, #8
	orrs rDATA, rSCRATCH

	adds rPREAD, #1
	cmp rPREAD, rBUFEND
	bne 1f
	ldr rPREAD, =recv_buf
	1:

	cmp rDATA, rFF00
	bgt frame_end

	strh rDATA, [rNEXT, rI]
	adds rI, #2

	cmp rI, rFRAMESZ
	bne read_loop

	frame_end:
	movs rSCRATCH, #0
	0:
	cmp rI, rFRAMESZ
	beq 1f

	strh rSCRATCH, [rNEXT, rI]
	adds rI, #2
	b 0b
	1:

	movs rSTATE, #GOOD

	cmp rDATA, rFF00
	ble 1f
		movs rSTATE, #GOOD_FFFF
		uxtb rDATA, rDATA
		cmp rDATA, #0xff
		beq 1f
		movs rSTATE, #BAD_FF
	1:

	state_loop:

	0:
	ldr rSCRATCH, [rPCNT]
	subs rSCRATCH, rBUFEND, rSCRATCH
	cmp rSCRATCH, rPREAD
	beq 0b

    ldrb rDATA, [rPREAD]

	adds rPREAD, #1
	cmp rPREAD, rBUFEND
	bne 1f
	ldr rPREAD, =recv_buf
	1:

	adds rSTATE, #IN_00
	cmp rDATA, #0x00
	beq 1f

	adds rSTATE, #(IN_01_EF-IN_00)
	cmp rDATA, #0xEF
	ble 1f

	adds rSTATE, #(IN_F0-IN_01_EF)
	cmp rDATA, #0xF0
	ble 1f

	adds rSTATE, #(IN_F1-IN_F0)
	cmp rDATA, #0xF1
	ble 1f

	adds rSTATE, #(IN_F2-IN_F1)
	cmp rDATA, #0xF2
	ble 1f

	adds rSTATE, #(IN_F3_FE-IN_F2)
	cmp rDATA, #0xFE
	ble 1f

	adds rSTATE, #(IN_FF-IN_F3_FE)
	1:

	add rSTATE, rFSM
	ldrb rSTATE, [rSTATE]
	cmp rSTATE, #GOOD_RETURN
	beq next_frame
	cmp rSTATE, #BAD_RETURN
	beq ignore_frame
	cmp rSTATE, #ROUTE
	beq ignore_frame
	cmp rSTATE, #TIMING
	beq ignore_frame

	b state_loop

	.size	main, .-main

