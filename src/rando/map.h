#ifndef _MAP_H_
#define _MAP_H_

#include "warps.h"

typedef enum map map_e;

#define MAP_MAX_INDEX  0x100

enum map{
    SM_SM           = 0x01,
    MM_MM           = 0x02,
    TTC_SHIP_BOTTOM = 0x03, //user-defined split of 0x05
    TTC_SHIP_TOP    = 0x04, //user-defined split of 0x05
    TTC_SHIP        = 0x05,
    TTC_NIPPER      = 0x06,
    TTC_TTC         = 0x07,

    TTC_SANDCASTLE  = 0x0A,
    CC_CC           = 0x0B,
    MM_TICKERS      = 0x0C,
    BGS_BGS         = 0x0D,
    MM_MUMBOS       = 0x0E,

    BGS_VILE        = 0x10,
    BGS_TANKTUP     = 0x11,
    GV_GV           = 0x12,
    GV_MATCHING     = 0x13,
    GV_SANDYBUTT    = 0x14,
    GV_WATER        = 0x15,
    GV_RUBEE        = 0x16,

    GV_JINXY        = 0x1A,

    MMM_MMM         = 0x1B,

    MMM_CHURCH      = 0x1C,
    MMM_CELLAR      = 0x1D,

    CC_WITCH_SWITCH = 0x21,
    CC_INSIDE       = 0x22,
    CC_WONDERWING   = 0x23,
    MMM_TUMBLAR     = 0x24,
    MMM_WELL        = 0x25,
    MMM_DINING_ROOM = 0x26,
    FP_FP           = 0x27,
    MMM_BLUE_EGGS   = 0x28,
    MMM_MUSIC_NOTS  = 0x29,
    MMM_RED_FEATHER = 0x2A,
    MMM_CHURCH_SECRET   = 0x2B,
    MMM_BATHROOM    = 0x2C,
    MMM_BEDROOM     = 0x2D,
    MMM_EMPTY_HONEY = 0x2E,
    MMM_RAIN_BARREL = 0x2F,
    MMM_MUMBOS      = 0x30,
    RBB_RBB         = 0x31,
    RBB_ENGINE_BUTTON   = 0x32, //user-defined split of 0x34
    RBB_ENGINE_GEARS    = 0x33, //user-defined split of 0x32
    RBB_ENGINE      = 0x34,
    RBB_BOAT_HOSUE  = 0x35,
    RBB_WAREHOUSE   = 0x36,
    RBB_L_CONTAINER = 0x37,
    RBB_R_CONTAINER = 0x38,
    RBB_CREW_QRTRS  = 0x39,
    RBB_BOSS_BOOMBOX = 0x3A,
    RBB_HULL        = 0x3B,
    RBB_KITCHEN     = 0x3C,
    RBB_NAVIGATION  = 0x3D,
    RBB_M_CONTAINER = 0x3E,
    RBB_CAPT_QRTRS  = 0x3F,
    CCW_HUB         = 0x40,
    FP_IGLOO        = 0x41,

    CCW_SPRING      = 0x43,
    CCW_SUMMER      = 0x44,
    CCW_AUTUMN      = 0x45,
    CCW_WINTER      = 0x46,
    BGS_MUMBOS      = 0x47,
    FP_MUMBOS       = 0x48,

    CCW_SP_MUMBOS   = 0x4A,
    CCW_SU_MUMBOS   = 0x4B,
    CCW_AU_MUMBOS   = 0x4C,
    CCW_WI_MUMBOS   = 0x4D,

    FP_XMAS_TREE    = 0x53,

    CCW_SP_ZUBBAS   = 0x5A,
    CCW_SU_ZUBBAS   = 0x5B,
    CCW_AU_ZUBBAS   = 0x5C,

    CCW_SP_NABNUTS  = 0x5E,
    CCW_SU_NABNUTS  = 0x5F,
    CCW_AU_NABNUTS  = 0x60,
    CCW_WI_NABNUTS  = 0x61,
    CCW_WI_NABNUT_HOARD = 0x62,
    CCW_AU_FLOODED_ROOM = 0x63,
    CCW_WI_FLOODED_ROOM = 0x64,
    CCW_SP_TOP_ROOM = 0x65,
    CCW_SU_TOP_ROOM = 0x66,
    CCW_AU_TOP_ROOM = 0x67,
    CCW_WI_TOP_ROOM = 0x68,
    LAIR_MM_LOBBY   = 0x69,
    LAIR_TTC_CC_PUZ = 0x6A,
    LAIR_CCW_PUZ    = 0x6B,
    LAIR_RED_CAULD  = 0x6C,
    LAIR_TTC_LOBBY  = 0x6D,
    LAIR_GV_LOBBY   = 0x6E,
    LAIR_FP_LOBBY   = 0x6F,
    LAIR_CC_LOBBY   = 0x70,
    LAIR_260_ND     = 0x71,
    LAIR_BGS_LOBBY  = 0x72,

    LAIR_GV_PUZ     = 0x74,
    LAIR_MMM_LOBBY  = 0x75,
    LAIR_640_ND     = 0x76,
    LAIR_RBB_LOBBY  = 0x77,
    LAIR_RBB_MMM_PUZ = 0x78,
    LAIR_CCW_LOBBY  = 0x79,
    LAIR_CRYPT      = 0x7A,

    FP_WOZZA_CAVE   = 0x7F,
    LAIR_FF_LOBBY   = 0x80,

    RBB_ANCHOR      = 0x8B,
    SM_HOME         = 0x8C,
    MMM_LOGGO       = 0x8D,
    LAIR_FF         = 0x8E,

    FIGHT           = 0x90,

    LAIR_810_ND     = 0x93,
};

#endif