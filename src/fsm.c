
#include "fsm.h"

const uint8_t fsm[STATE_COUNT][8] =
{
	[GOOD]        = {  [IN_00] = GOOD_00, [IN_01_EF] = GOOD_01_FE, [IN_F0] = GOOD_01_FE , [IN_F1_FE] = GOOD_01_FE, [IN_FF] = GOOD_FF      , },
	[GOOD_00]     = {  [IN_00] = GOOD   , [IN_01_EF] = GOOD      , [IN_F0] = GOOD       , [IN_F1_FE] = GOOD      , [IN_FF] = GOOD         , },
	[GOOD_01_FE]  = {  [IN_00] = GOOD   , [IN_01_EF] = GOOD      , [IN_F0] = GOOD       , [IN_F1_FE] = GOOD      , [IN_FF] = BAD_FF       , },
	[GOOD_FF]     = {  [IN_00] = GOOD   , [IN_01_EF] = GOOD      , [IN_F0] = GOOD       , [IN_F1_FE] = GOOD      , [IN_FF] = GOOD_FFFF    , },
	[GOOD_FFFF]   = {  [IN_00] = BAD    , [IN_01_EF] = BAD       , [IN_F0] = BAD        , [IN_F1_FE] = BAD       , [IN_FF] = GOOD_FFFFFF  , },
	[GOOD_FFFFFF] = {  [IN_00] = BAD    , [IN_01_EF] = BAD       , [IN_F0] = GOOD_RETURN, [IN_F1_FE] = BAD       , [IN_FF] = BAD_FFFFFF   , },
	[BAD]         = {  [IN_00] = BAD    , [IN_01_EF] = BAD       , [IN_F0] = BAD        , [IN_F1_FE] = BAD       , [IN_FF] = BAD_FF       , },
	[BAD_FF]      = {  [IN_00] = BAD    , [IN_01_EF] = BAD       , [IN_F0] = BAD        , [IN_F1_FE] = BAD       , [IN_FF] = BAD_FFFF     , },
	[BAD_FFFF]    = {  [IN_00] = BAD    , [IN_01_EF] = BAD       , [IN_F0] = BAD        , [IN_F1_FE] = BAD       , [IN_FF] = BAD_FFFFFF   , },
	[BAD_FFFFFF]  = {  [IN_00] = BAD    , [IN_01_EF] = BAD       , [IN_F0] = BAD_RETURN , [IN_F1_FE] = BAD       , [IN_FF] = BAD_FFFFFF   , },

};

