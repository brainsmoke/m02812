#ifndef FSM_H
#define FSM_H

#define GOOD          0
#define GOOD_00       1
#define GOOD_01_FE    2
#define GOOD_FF       3
#define GOOD_FFFF     4
#define GOOD_FFFFFF   5
#define BAD           6
#define BAD_FF        7
#define BAD_FFFF      8
#define BAD_FFFFFF    9

#define STATE_COUNT  10

#define GOOD_RETURN  11
#define BAD_RETURN   12



#define IN_00     0
#define IN_01_EF  1
#define IN_F0     2
#define IN_F1_FE  3
#define IN_FF     4

#ifndef __ASSEMBLER__

#include <stdint.h>

extern const uint8_t fsm[STATE_COUNT][8];

#endif

#endif //FSM_H
