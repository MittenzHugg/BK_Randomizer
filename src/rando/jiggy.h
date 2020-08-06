/*************************************************************************
*   Copyright (C) 2020 Michael (Mittenz) Salino-Hugg
*   Contact: Twitter: @MittenzPhD
**************************************************************************
*	jiggy.h
*
*   This file is part of BK_Randomizer.
*
*   BK_Randomizer is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   BK_Randomizer is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with BK_Randomizer.  If not, see <https://www.gnu.org/licenses/>.
*************************************************************************/

#include "simptypes.h"
#include "warps.h"


typedef enum bk_jiggy bk_jiggy_e;
#define JIG_MAX_INDEX 100

enum bk_jiggy{
	JIG_MM_JINJO        =  1,
	JIG_MM_TICKER       =  2,
    JIG_MM_MUMBO_SKULL  =  3,
    JIG_MM_JUJU         =  4,
    JIG_MM_HUTS         =  5,
    JIG_MM_RUINS        =  6,
    JIG_MM_HILL         =  7,
    JIG_MM_ORANGE_PADS  =  8,
    JIG_MM_CHIMPY       =  9,
    JIG_MM_CONGA        = 10,
    JIG_TTC_JINJO       = 11,
    JIG_TTC_LIGHTHOUSE  = 12,
    JIG_TTC_ALCOVE_1    = 13,
    JIG_TTC_ALCOVE_2    = 14,
    JIG_TTC_POOL        = 15,
    JIG_TTC_SANDCASTLE  = 16,
    JIG_TTC_RED_X       = 17,
    JIG_TTC_NIPPER      = 18,
    JIG_TTC_LOCKUP      = 19,
    JIG_TTC_BLUBBER     = 20,
    JIG_CC_JINJO        = 21,
    JIG_CC_SNIPPETS     = 22,
    JIG_CC_BACK         = 23,
    JIG_CC_BOLT         = 24,
    JIG_CC_TAIL         = 25,
    JIG_CC_LONG_PIPE    = 26,
    JIG_CC_TOOTH        = 27,
    JIG_CC_RINGS        = 28,
    JIG_CC_SLOW_SAWS    = 29,
    JIG_CC_FAST_SAWS    = 30,
    JIG_BGS_JINJO       = 31,
    JIG_BGS_WALKWAY     = 32,
    JIG_BGS_EGG         = 33,
    JIG_BGS_CROCTUS     = 34,
    JIG_BGS_MUD_HUTS    = 35,
    JIG_BGS_FLIBBITS    = 36,
    JIG_BGS_MAZE        = 37,
    JIG_BGS_TANKTUP     = 38,
    JIG_BGS_TIPTUP      = 39,
    JIG_BGS_VILE        = 40,
    JIG_FP_JINJO        = 41,
    JIG_FP_BOGGY_1      = 42,
    JIG_FP_PIPE         = 43,
    JIG_FP_BOGGY_3      = 44,
    JIG_FP_SNOWMAN_BUTTONS  = 45,
    JIG_FP_PRESENTS     = 46,
    JIG_FP_XMAS_TREE    = 47,
    JIG_FP_BOGGY_2      = 48,
    JIG_FP_SIR_SLUSH    = 49,
    JIG_FP_WOZZA        = 50,
    JIG_LAIR_1ST        = 51,
    JIG_LAIR_MM_WS      = 52,
    JIG_LAIR_CC_WS      = 53,
    JIG_LAIR_TTC_WS     = 54,
    JIG_LAIR_BGS_WS     = 55,
    JIG_LAIR_FP_WS      = 56,
    JIG_LAIR_MMM_WS     = 57,
    JIG_LAIR_GV_WS      = 58,
    JIG_LAIR_RBB_WS     = 59,
    JIG_LAIR_CCW_WS     = 60,
    JIG_GV_JINJO        = 61,
    JIG_GV_GRABBA       = 62,
    JIG_GV_JINXY        = 63,
    JIG_GV_MATCHING     = 64,
    JIG_GV_SANDYBUTT    = 65,
    JIG_GV_WATER        = 66,
    JIG_GV_HISTUP       = 67,
    JIG_GV_GOBI_1       = 68,
    JIG_GV_GOBI_2       = 69,
    JIG_GV_ANCIENT_ONES = 70,
    JIG_CCW_JINJO       = 71,
    JIG_CCW_HOUSE       = 72,
    JIG_CCW_Eyrie       = 73,
    JIG_CCW_NABNUT      = 74,
    JIG_CCW_GNAWTY      = 75,
    JIG_CCW_ZUBBAS      = 76,
    JIG_CCW_FLOWER      = 77,
    JIG_CCW_SUM_LEAVES  = 78,
    JIG_CCW_TREE_TOP    = 79,
    JIG_CCW_TOP_ROOM    = 80,
    JIG_RBB_JINJO       = 81,
    JIG_RBB_WAREHOUSE   = 82,
    JIG_RBB_SNORKEL     = 83, 
    JIG_RBB_WHISTLE     = 84,
    JIG_RBB_FUNNEL      = 85,
    JIG_RBB_BOSS_BOOMBOX    = 86,
    JIG_RBB_PROPELLOR   = 87,
    JIG_RBB_CPT_QRTRS   = 88,
    JIG_RBB_CRANE       = 89,
    JIG_RBB_ENGINE_ROOM = 90,
    JIG_MMM_JINJO       = 91,
    JIG_MMM_WELL        = 92,
    JIG_MMM_NAPPER      = 93,
    JIG_MMM_CELLAR      = 94,
    JIG_MMM_CHURCH_ROOF = 95,
    JIG_MMM_MOTZHAND    = 96,
    JIG_MMM_RAIN_BARREL = 97,
    JIG_MMM_TUMBLAR     = 98,
    JIG_MMM_FLOWER_POTS = 99,
    JIG_MMM_LOGGO       = 100,
};