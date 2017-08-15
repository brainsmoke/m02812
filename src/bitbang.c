
#include "stm32f0xx.h"

/* scatter gather bits:
 *  
 *  I0 [ 00 01 02 03 04 05 06 07 ]
 *  I1 [ 10 11 12 13 14 15 16 17 ]
 *  I2 [ 20 21 22 23 24 25 26 27 ]
 *  I3 [ 30 31 32 33 34 35 36 37 ]
 *  I4 [ 40 41 42 43 44 45 46 47 ]
 *  I5 [ 50 51 52 53 54 55 56 57 ]
 *  I6 [ 60 61 62 63 64 65 66 67 ]
 *  I7 [ 70 71 72 73 74 75 76 77 ]
 *  
 *                ||
 *               \||/
 *                \/
 *  
 *  I0 [ 00 10 20 30 40 50 60 70 ]
 *  I1 [ 01 11 21 31 41 51 61 71 ]
 *  I2 [ 02 12 22 32 42 52 62 72 ]
 *  I3 [ 03 13 23 33 43 53 63 73 ]
 *  I4 [ 04 14 24 34 44 54 64 74 ]
 *  I5 [ 05 15 25 35 45 55 65 75 ]
 *  I6 [ 06 16 26 36 46 56 66 76 ]
 *  I7 [ 07 17 27 37 47 57 67 77 ]
 *  
 */

/*        |--- 1 bit ----|
 *
 *        |---T1H---|-T1L|
 *
 *        |-T0H|---T0L---|
 *          A    B    C
 *        |----|----|----|
 * ..      ____......    ____
 *   |    |    |    |    |
 *   |    |    |    |    |
 *   |    |    |    |    |
 * ..|____|    |....|____|
 *
 *              ||
 *             \||/
 *              \/
 *
 *        |---T1H---|-T1L|
 *
 *        |-T0H|---T0L---|
 *     C'   A    B   
 *   |----|----|----|
 * ..      ____......    ____
 *   |    |    |    |    |
 *   |    |    |    |    |
 *   |    |    |    |    |
 * ..|____|    |....|____|
 *
 *   |- 1 iteration-|
 * 
 */

uint8_t framebuf[100];
uint16_t gamma_map[256];
static uint8_t *outbuf;
uint8_t *frame;

#define GPIOA_BASE_STRING "0x48000000"

void bitbang(void)
{
	asm volatile (

//#define GPIOx_ODR (0x14)

#define ARM_PUSH_CALLEE_SAVED \
"mov r0, r8 \n" \
"mov r1, r9 \n" \
"mov r2, r10 \n" \
"mov r3, r11 \n" \
"push {r0-r7} \n"

#define ARM_POP_CALLEE_SAVED \
"pop {r0-r7} \n" \
"mov r8, r0 \n" \
"mov r9, r1 \n" \
"mov r10, r2 \n" \
"mov r11, r3 \n"

ARM_PUSH_CALLEE_SAVED

"movs r7, #0\n"
"ldr r6, =" GPIOA_BASE_STRING "\n"

/* r7 == 0, r0-r5,r8-r9 = [I0...I7] */

/* r0: I0
 * r1: I1
 * r2: I2
 * r3: I3
 * r4: I4
 * r5: I5
 * r6: &GPIOA_BASE
 * r7: scratch
 * r8: I6
 * r9: I7
 */

/* BITBANG BIT 7 */

/* phase C' (T1L) 'previous bit' */
                                                 /*  carry               register               */

/*  0 +  2 */ " strh r7, [r6, %[GPIOx_ODR]]  \n"                                                   /* r7 == 0   */
/*  2 +  1 */ " lsl r1, r1, #24              \n" /*          [ b0 b1 b2 b3 b4 b5 b6 b7 ... 0 ]  */
/*  3 +  1 */ " lsl r2, r2, #24              \n" /*          [ c0 c1 c2 c3 c4 c5 c6 c7 ... 0 ]  */
/*  4 +  1 */ " lsl r3, r3, #24              \n" /*          [ d0 d1 d2 d3 d4 d5 d6 d7 ... 0 ]  */
/*  5 +  1 */ " lsl r4, r4, #24              \n" /*          [ e0 e1 e2 e3 e4 e5 e6 e7 ... 0 ]  */
/*  6 +  1 */ " lsl r5, r5, #24              \n" /*          [ f0 f1 f2 f3 f4 f5 f6 f7 ... 0 ]  */
/*  7 +  1 */ " mov r7, r8                   \n"
/*  8 +  1 */ " lsl r7, r7, #24              \n" /*          [ g0 g1 g2 g3 g4 g5 g6 g7 ... 0 ]  */
/*  9 +  1 */ " mov r8, r7                   \n"
/* 10 +  1 */ " mov r7, r9                   \n"
/* 11 +  1 */ " lsl r7, r7, #24              \n" /*          [ h0 h1 h2 h3 h4 h5 h6 h7 ... 0 ]  */
/* 12 +  1 */ " mov r9, r7                   \n"
/* 13 +  1 */ " movs r7, #255                \n"
/* 14 +  1 */ " lsl r0, r0, #25              \n" /*  [ a0 ]  [ a1 a2 a3 a4 a5 a6 a7    ... 0 ]  */
/* 15 +  1 */ " adc r0, r0                   \n" /*  [ a1 ]  [ a2 a3 a4 a5 a6 a7      ... a0 ]  */
/* 16 +  1 */ " adc r1, r1                   \n" /*  [ b0 ]  [ b1 b2 b3 b4 b5 b6 b7 ...   a1 ]  */
/* 17 +  1 */ " adc r0, r0                   \n" /*  [ a2 ]  [ a3 a4 a5 a6 a7      ... a0 b0 ]  */

/* phase A (T0H) */

/* 18 +  2 */ " strh r7, [r6, %[GPIOx_ODR]]  \n"                                                   /* r7 == 255   */

/* 20 +  1 */ " adc r2, r2                   \n" /*  [ c0 ]  [ c1 c2 c3 c4 c5 c6 c7 ...   a2 ]  */
/* 21 +  1 */ " adc r0, r0                   \n" /*  [ a3 ]  [ a4 a5 a6 a7      ... a0 b0 c0 ]  */
/* 22 +  1 */ " adc r3, r3                   \n" /*  [ d0 ]  [ d1 d2 d3 d4 d5 d6 d7 ...   a3 ]  */
/* 23 +  1 */ " adc r0, r0                   \n" /*  [ a4 ]  [ a5 a6 a7      ... a0 b0 c0 d0 ]  */
/* 24 +  1 */ " adc r4, r4                   \n" /*  [ e0 ]  [ e1 e2 e3 e4 e5 e6 e7 ...   a4 ]  */
/* 25 +  1 */ " adc r0, r0                   \n" /*  [ a5 ]  [ a6 a7      ... a0 b0 c0 d0 e0 ]  */
/* 26 +  1 */ " adc r5, r5                   \n" /*  [ f0 ]  [ f1 f2 f3 f4 f5 f6 f7 ...   a5 ]  */
/* 27 +  1 */ " adc r0, r0                   \n" /*  [ a6 ]  [ a7      ... a0 b0 c0 d0 e0 f0 ]  */
/* 28 +  1 */ " mov r7, r8                   \n"
/* 29 +  1 */ " adc r7, r7                   \n" /*  [ g0 ]  [ g1 g2 g3 g4 g5 g6 g7 ...   a6 ]  */
/* 30 +  1 */ " adc r0, r0                   \n" /*  [ a7 ]  [ 0    ... a0 b0 c0 d0 e0 f0 g0 ]  */
/* 31 +  1 */ " mov r8, r7                   \n"
/* 32 +  1 */ " mov r7, r9                   \n"
/* 33 +  1 */ " adc r7, r7                   \n" /*  [ h0 ]  [ h1 h2 h3 h4 h5 h6 h7 ...   a7 ]  */
/* 34 +  1 */ " mov r9, r7                   \n"
/* 35 +  1 */ " adc r0, r0                   \n" /*  [  0 ]  [ 0 ... a0 b0 c0 d0 e0 f0 g0 h0 ]  */

/* phase B (T1H-T0H) */

/* r0: I0 (done) */

/* 36 +  2 */ " strh r0, [r6, %[GPIOx_ODR]]  \n"

/* r0: free */

/* 38 +  1 */ " add r1, r1                   \n" /*  [ b1 ]  [ b2 b3 b4 b5 b6 b7 ...   a1  0 ]  */
/* 39 +  1 */ " adc r1, r1                   \n" /*  [ b2 ]  [ b3 b4 b5 b6 b7 ...   a1  0 b1 ]  */
/* 40 +  1 */ " adc r2, r2                   \n" /*  [ c1 ]  [ c2 c3 c4 c5 c6 c7 ...   a2 b2 ]  */
/* 41 +  1 */ " adc r1, r1                   \n" /*  [ b3 ]  [ b4 b5 b6 b7 ...   a1  0 b1 c1 ]  */
/* 42 +  1 */ " adc r3, r3                   \n" /*  [ d1 ]  [ d2 d3 d4 d5 d6 d7 ...   a3 b3 ]  */
/* 43 +  1 */ " adc r1, r1                   \n" /*  [ b4 ]  [ b5 b6 b7 ...   a1  0 b1 c1 d1 ]  */
/* 44 +  1 */ " adc r4, r4                   \n" /*  [ e1 ]  [ e2 e3 e4 e5 e6 e7 ...   a4 b4 ]  */
/* 45 +  1 */ " adc r1, r1                   \n" /*  [ b5 ]  [ b6 b7 ...   a1  0 b1 c1 d1 e1 ]  */
/* 46 +  1 */ " adc r5, r5                   \n" /*  [ f1 ]  [ f2 f3 f4 f5 f6 f7 ...   a5 b5 ]  */
/* 47 +  1 */ " adc r1, r1                   \n" /*  [ b6 ]  [ b7 ...   a1  0 b1 c1 d1 e1 f1 ]  */
/* 48 +  1 */ " mov r7, r8                   \n"
/* 49 +  1 */ " adc r7, r7                   \n" /*  [ g1 ]  [ g2 g3 g4 g5 g6 g7 ...   a6 b6 ]  */
/* 50 +  1 */ " adc r1, r1                   \n" /*  [ b7 ]  [ ...   a1  0 b1 c1 d1 e1 f1 g1 ]  */
/* 51 +  1 */ " mov r8, r7                   \n"
/* 52 +  1 */ " mov r7, r9                   \n"
/* 53 +  1 */ " adc r7, r7                   \n" /*  [ h1 ]  [ h2 h3 h4 h5 h6 h7 ...   a7 b7 ]  */
/* 54 +  1 */ " mov r9, r7                   \n"
/* 55 +  1 */ " adc r1, r1                   \n" /*  [  0 ]  [ ...a1  0 b1 c1 d1 e1 f1 g1 h1 ]  */

/* 56 +  1 */ " uxtb r0, r1                  \n" /*          [ ...      b1 c1 d1 e1 f1 g1 h1 ]  */
/* 57 +  1 */ " add r1, r0                   \n" /*  [  0 ]  [ ...a1 b1 c1 d1 e1 f1 g1 h1  0 ]  */
/* 58 +  1 */ " lsr r1, r1, #1               \n" /*  [  0 ]  [ ...   a1 b1 c1 d1 e1 f1 g1 h1 ]  */
/* 59 +  1 */ " mov r0, #0                   \n"

/* r1: I1 (done) */

/* BITBANG BIT 6 */

/* phase C' (T1L) 'previous bit' */
                                                 /*  carry               register               */

/*  0 +  2 */ " strh r0, [r6, %[GPIOx_ODR]]  \n"                                                   /* r0 == 0   */

/*  2 +  1 */ " movs r0, #255                \n"
/*  3 +  1 */ " add r2, r2                   \n" /*  [ c2 ]  [ c3 c4 c5 c6 c7 ...   a2 b2  0 ]  */
/*  4 +  1 */ " adc r2, r2                   \n" /*  [ c3 ]  [ c4 c5 c6 c7 ...   a2 b2  0 c2 ]  */
/*  5 +  1 */ " adc r3, r3                   \n" /*  [ d2 ]  [ d3 d4 d5 d6 d7 ...   a3 b3 c3 ]  */
/*  6 +  1 */ " adc r2, r2                   \n" /*  [ c4 ]  [ c5 c6 c7 ...   a2 b2  0 c2 d2 ]  */
/*  7 +  1 */ " adc r4, r4                   \n" /*  [ e2 ]  [ e3 e4 e5 e6 e7 ...   a4 b4 c4 ]  */
/*  8 +  1 */ " adc r2, r2                   \n" /*  [ c5 ]  [ c6 c7 ...   a2 b2  0 c2 d2 e2 ]  */
/*  9 +  1 */ " adc r5, r5                   \n" /*  [ f2 ]  [ f3 f4 f5 f6 f7 ...   a5 b5 c5 ]  */
/* 10 +  1 */ " adc r2, r2                   \n" /*  [ c6 ]  [ c7 ...   a2 b2  0 c2 d2 e2 f2 ]  */
/* 11 +  1 */ " mov r7, r8                   \n"
/* 12 +  1 */ " adc r7, r7                   \n" /*  [ g2 ]  [ g3 g4 g5 g6 g7 ...   a6 b6 c6 ]  */
/* 13 +  1 */ " mov r8, r7                   \n"
/* 14 +  1 */ " adc r2, r2                   \n" /*  [ c7 ]  [ ...   a2 b2  0 c2 d2 e2 f2 g2 ]  */
/* 15 +  1 */ " mov r7, r9                   \n"
/* 16 +  1 */ " adc r7, r7                   \n" /*  [ h2 ]  [ h3 h4 h5 h6 h7 ...   a7 b7 c7 ]  */
/* 17 +  1 */ " adc r2, r2                   \n" /*  [  0 ]  [ ...a2 b2  0 c2 d2 e2 f2 g2 h2 ]  */

/* 18 +  2 */ " strh r0, [r6, %[GPIOx_ODR]]  \n"                                                   /* r0 == 255   */

/* 20 +  1 */ " lsl r2, r2, #1               \n" /*  [  0 ]  [ ...a2 b2  0 c2 d2 e2 f2 g2 h2 0 ]  */
/* 21 +  1 */ " uxtb r0, r2                  \n" /*          [ ...         c2 d2 e2 f2 g2 h2 0 ]  */
/* 22 +  1 */ " add r2, r0                   \n" /*  [  0 ]  [ ...a2 b2 c2 d2 e2 f2 g2 h2  0 0 ]  */
/* 23 +  1 */ " lsr r2, r2, #2               \n" /*  [  0 ]  [ ...   a2 b2 c2 d2 e2 f2 g2 h2 ]  */

/* 24 +  1 */ " mov r0, r8                   \n"

/* r0: I6
 * r1: I1 (done)
 * r2: I2 (done)
 * r3: I3
 * r4: I4
 * r5: I5
 * r6: &GPIOA_BASE
 * r7: I7
 */

/* 25 +  1 */ " add r3, r3                   \n" /*  [ d3 ]  [ d4 d5 d6 d7 ...   a3 b3 c3  0 ]  */
/* 26 +  1 */ " adc r3, r3                   \n" /*  [ d4 ]  [ d5 d6 d7 ...   a3 b3 c3  0 d3 ]  */
/* 27 +  1 */ " adc r4, r4                   \n" /*  [ e3 ]  [ e4 e5 e6 e7 ...   a4 b4 c4 d4 ]  */
/* 28 +  1 */ " adc r3, r3                   \n" /*  [ d5 ]  [ d6 d7 ...   a3 b3 c3  0 d3 e3 ]  */
/* 29 +  1 */ " adc r5, r5                   \n" /*  [ f3 ]  [ f4 f5 f6 f7 ...   a5 b5 c5 d5 ]  */
/* 30 +  1 */ " adc r3, r3                   \n" /*  [ d6 ]  [ d7 ...   a3 b3 c3  0 d3 e3 f3 ]  */
/* 31 +  1 */ " adc r0, r0                   \n" /*  [ g3 ]  [ g4 g5 g6 g7 ...   a6 b6 c6 d6 ]  */
/* 32 +  1 */ " adc r3, r3                   \n" /*  [ d7 ]  [ ...   a3 b3 c3  0 d3 e3 f3 g3 ]  */
/* 33 +  1 */ " adc r7, r7                   \n" /*  [ h3 ]  [ h4 h5 h6 h7 ...   a7 b7 c7 d7 ]  */
/* 34 +  1 */ " adc r3, r3                   \n" /*  [  0 ]  [ ...a3 b3 c3  0 d3 e3 f3 g3 h3 ]  */
/* 35 +  1 */ " lsl r3, r3, #2               \n" /*  [  0 ]  [ ...a3 b3 c3  0 d3 e3 f3 g3 h3 0 0 ]  */

/* 36 +  2 */ " strh r1, [r6, %[GPIOx_ODR]]  \n"

/* r1: free */

/* 38 +  1 */ " uxtb r1, r3                  \n" /*          [ ...            d3 e3 f3 g3 h3 0 0 ]  */
/* 39 +  1 */ " add r3, r1                   \n" /*  [  0 ]  [ ...a3 b3 c3 d3 e3 f3 g3 h3  0 0 0 ]  */
/* 40 +  1 */ " lsr r3, r3, #3               \n" /*  [  0 ]  [ ...       a3 b3 c3 d3 e3 f3 g3 h3 ]  */

/* r3: I3 (done) */

/* 41 +  1 */ " add r4, r4                   \n" /*  [ e4 ]  [ e5 e6 e7 ...   a4 b4 c4 d4  0 ]  */
/* 42 +  1 */ " adc r4, r4                   \n" /*  [ e5 ]  [ e6 e7 ...   a4 b4 c4 d4  0 e4 ]  */
/* 43 +  1 */ " adc r5, r5                   \n" /*  [ f4 ]  [ f5 f6 f7 ...   a5 b5 c5 d5 e5 ]  */
/* 44 +  1 */ " adc r4, r4                   \n" /*  [ e6 ]  [ e7 ...   a4 b4 c4 d4  0 e4 f4 ]  */
/* 45 +  1 */ " adc r0, r0                   \n" /*  [ g4 ]  [ g5 g6 g7 ...   a6 b6 c6 d6 e6 ]  */
/* 46 +  1 */ " adc r4, r4                   \n" /*  [ e7 ]  [ ...   a4 b4 c4 d4  0 e4 f4 g4 ]  */
/* 47 +  1 */ " adc r7, r7                   \n" /*  [ h4 ]  [ h5 h6 h7 ...   a7 b7 c7 d7 e7 ]  */
/* 48 +  1 */ " adc r4, r4                   \n" /*  [  0 ]  [ ...a4 b4 c4 d4  0 e4 f4 g4 h4 ]  */

/* 49 +  1 */ " lsl r4, r4, #3               \n" /*  [  0 ]  [ ...a4 b4 c4 d4  0 e4 f4 g4 h4 0 0 0 ]  */
/* 50 +  1 */ " uxtb r1, r4                  \n" /*          [ ...               e4 f4 g4 h4 0 0 0 ]  */
/* 51 +  1 */ " add r4, r1                   \n" /*  [  0 ]  [ ...a4 b4 c4 d4 e4 f4 g4 h4  0 0 0 0 ]  */
/* 52 +  1 */ " lsr r4, r4, #4               \n" /*  [  0 ]  [ ...         a4 b4 c4 d4 e4 f4 g4 h4 ]  */

/* 53 +  1 */ " movs r1, #0                  \n"
/* r4: I4 (done) */

/* 54 +  1 */ " add r5, r5                   \n" /*  [ f5 ]  [ f6 f7 ...   a5 b5 c5 d5 e5  0 ]  */
/* 55 +  1 */ " adc r5, r5                   \n" /*  [ f6 ]  [ f7 ...   a5 b5 c5 d5 e5  0 f5 ]  */
/* 56 +  1 */ " adc r0, r0                   \n" /*  [ g5 ]  [ g6 g7 ...   a6 b6 c6 d6 e6 f6 ]  */
/* 57 +  1 */ " adc r5, r5                   \n" /*  [ f7 ]  [ ...   a5 b5 c5 d5 e5  0 f5 g5 ]  */
/* 58 +  1 */ " adc r7, r7                   \n" /*  [ h5 ]  [ h6 h7 ...   a7 b7 c7 d7 e7 f7 ]  */
/* 59 +  1 */ " adc r5, r5                   \n" /*  [  0 ]  [ ...a5 b5 c5 d5 e5  0 f5 g5 h5 ]  */


/* BITBANG BIT 5 */

/* phase C' (T1L) 'previous bit' */
                                                 /*  carry               register               */

/*  0 +  2 */ " strh r1, [r6, %[GPIOx_ODR]]  \n"                                                   /* r0 == 0   */


/*  2 +  1 */ " lsl r5, r5, #4               \n" /*  [  0 ]  [ ...a5 b5 c5 d5 e5  0 f5 g5 h5 0 0 0 0 ]  */
/*  3 +  1 */ " uxtb r1, r5                  \n" /*          [ ...                  f5 g5 h5 0 0 0 0 ]  */
/*  4 +  1 */ " add r5, r1                   \n" /*  [  0 ]  [ ...a5 b5 c5 d5 e5 f5 g5 h5  0 0 0 0 0 ]  */
/*  5 +  1 */ " lsr r5, r5, #5               \n" /*  [  0 ]  [ ...           a5 b5 c5 d5 e5 f5 g5 h5 ]  */

/* r0: I6
 * r1: free
 * r2: I2 (done)
 * r3: I3 (done)
 * r4: I4 (done)
 * r5: I5 (done)
 * r6: &GPIOA_BASE
 * r7: I7
 */

/*  6 +  1 */ " add r0, r0                   \n" /*  [ g6 ]  [ g7 ...   a6 b6 c6 d6 e6 f6  0 ]  */
/*  7 +  1 */ " adc r0, r0                   \n" /*  [ g7 ]  [ ...   a6 b6 c6 d6 e6 f6  0 g6 ]  */
/*  8 +  1 */ " adc r7, r7                   \n" /*  [ h6 ]  [ h7 ...   a7 b7 c7 d7 e7 f7 g7 ]  */
/*  9 +  1 */ " adc r0, r0                   \n" /*  [  0 ]  [ ...a6 b6 c6 d6 e6 f6  0 g6 h6 ]  */
/* 10 +  1 */ " lsl r0, r0, #5               \n" /*  [  0 ]  [ ...a6 b6 c6 d6 e6 f6  0 g6 h6 0 0 0 0 0 ]  */
/* 11 +  1 */ " uxtb r1, r0                  \n" /*          [ ...                     g6 h6 0 0 0 0 0 ]  */
/* 12 +  1 */ " add r0, r1                   \n" /*  [  0 ]  [ ...a6 b6 c6 d6 e6 f6 g6 h6  0 0 0 0 0 0 ]  */
/* 13 +  1 */ " lsr r0, r0, #6               \n" /*  [  0 ]  [ ...             a6 b6 c6 d6 e6 f6 g6 h6 ]  */
/* 14 +  1 */ " lsl r1, r7, #1               \n" /*  [ h7 ]                                               */
/* 15 +  1 */ " adc r7, r7                   \n" /*  [ h7 ]  [ ...   a7 b7 c7 d7 e7 f7 g7 h7 ]  */
/* 16 +  1 */ " movs r1, #255                \n"

/* 17 +  1 */ " mov r1, r1                   \n" /* nop */
/* 18 +  2 */ " strh r1, [r6, %[GPIOx_ODR]]  \n"                                                   /* r0 == 255 */

/* 20 +  2 */ " ldr r1,=outbuf               \n"
/* 22 +  2 */ " ldr r1,[r1]                  \n"

/* r0: I6 (done)
 * r1: free
 * r2: I2 (done)
 * r3: I3 (done)
 * r4: I4 (done)
 * r5: I5 (done)
 * r6: &GPIOA_BASE
 * r7: I7 (done)
 */


ARM_POP_CALLEE_SAVED
 :
 :
	[GPIOx_ODR] "I" (0x14)
);

}

void main(void)
{
	bitbang();
}

