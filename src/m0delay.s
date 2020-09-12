// cycles == n * 4 - 1
.macro delay_loop_4n_m1 reg iter
movs \reg, #(\iter)
0:
subs \reg, #1
bne 0b
.endm

// cycles == n * 5 - 1
.macro delay_loop_5n_m1 reg iter
movs \reg, #(\iter)
0:
nop
subs \reg, #1
bne 0b
.endm

// cycles == n * 7 - 1
.macro delay_loop_7n_m1 reg iter
movs \reg, #(\iter)
0:
b 1f
1:
subs \reg, #1
bne 0b
.endm

.macro delay reg num
	.ifge \num-11
		delay_loop \reg, \num

	.else
		.ifge \num-4
			b 1f
			1:
			delay \reg, (\num-3)
		.else
			.ifge \num-1
				delay \reg, (\num-1)
				movs \reg, #0

			.endif
		.endif
;	.endif
.endm

.macro delay_loop reg num
	.ifeq (\num+1)%4
		delay_loop_4n_m1 \reg, (\num+1)/4

	.else
		.ifeq (\num+1)%5
			delay_loop_5n_m1 \reg, (\num+1)/5

		.else
			.ifeq (\num+1)%7
				delay_loop_7n_m1 \reg, (\num+1)/7

			.else
				delay \reg, (\num+1)%4
				delay_loop_4n_m1 \reg, (\num+1)/4
			.endif
		.endif
	.endif
.endm


/* WIP
.macro delay_4_plus_reg reg scratch


                                //        [alignment] [reg&3==0] [reg&3==1] [reg&3==2] [reg&3==3]
100:
.balignw 4, 0x0000              // 2         0 or 1
101:
   lsrs \scratch, \reg, 1       // 0                       1          1          1          1
   bcs 1f                       // 2                       1          3          1          3
   nop                          // 0                       1                     1
1: lsrs \scratch, \scratch, 1   // 2                       1          1          1          1
   bcs 2f                       // 0                       1          1          3          3
2: subs \scratch, 1             // 2                                    reg>>2
   bne 2b                       // 0                                 3*(reg>>2)-2
   .ifeq (100b - 101b)
   nop                          // 2         0 or 1
   .endif
                                //                       total:    reg + 4 , reg >= 9
.endm
*/
