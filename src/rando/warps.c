#include "warps.h"

//node info

const warp_t vanilla_pairs[] = {

    // SM,sm
    //todo handle SM differently
    {{0x01,0x12}, {0x8C,0x01}, 0, 0}, //SM,sm <- SM,banjo_house
    {{0x01,0x13}, {0x69,0x12}, 0, 0}, //SM,sm <- lair,mm_loby

    //MM,mm
    {{0x02,0x01}, {0x0E,0x01}, 0, 0}, //MM,mm <- MM,mm_mumbos
    {{0x02,0x02}, {0x0C,0x02}, 0, 0}, //MM,mm <- MM,mm_tickers_lower
    {{0x02,0x03}, {0x0C,0x01}, 0, WARP_FLAG_INACCESSIBLE}, //MM,mm <- MM,mm_tickers_upper
    //{{0x02,0x04}, {,}, WARP_FLAG_ONEWAY}, //MM,witchswitch
    {{0x02,0x04}, {0x69,0x02}, 0, 0}, //MM,mm <- lair,mm_lobby

    //TTC,blubber_ship
    {{0x05, 0x05}, {0x07,0x06}, 0, 0}, //TTC,blubber_ship_top <- TTC,ttc
    {{0x05, 0x06}, {0x07,0x07}, 0, 0}, //TTC,blubber_ship_bottom <- TTC,ttc

    //TTC,nipper_shell
    {{0x06, 0x01}, {0x07,0x0A}, 0, 0}, //TTC, nipper_shell <- TTC,ttc

    //TTC, ttc
    {{0x07,0x03}, {0x0A, 0x01}, 0, 0}, //TTC,ttc <- TTC,sandcastle
    {{0x07,0x04}, {0x6D, 0x04}, 0, 0}, //TTC,ttc <- lair,ttc_lobby
    {{0x07,0x06}, {0x05, 0x05}, 0, WARP_FLAG_BEAK_BUST | WARP_FLAG_EGGS}, //TTC,ttc <- TTC,blubber_ship_top
    {{0x07,0x07}, {0x05, 0x06}, 0, 0},      //TTC,ttc <- TTC,blubber_ship_bottom
    {{0x07,0x08}, {0x07, 0x0C}, 0, WARP_FLAG_FLIGHT },      //TTC,lighthouse_top <- TTC,lighthouse_bottom
    {{0x07,0x0A}, {0x06, 0x01}, 0, 0}, //TTC,ttc <- TTC,nipper_shell
    {{0x07,0x0C}, {0x07, 0x08}, 0, WARP_FLAG_FLIGHT },      //TTC,stairs_top <- TTC,stairs_bottom
    {{0x07,0x0E}, {0x07, 0x0F}, 0, WARP_FLAG_FLIGHT },      //TTC,stairs_bottom <- TTC,stairs_top
    {{0x07,0x0F}, {0x07, 0x0E}, 0, WARP_FLAG_FLIGHT },      //TTC,lighthouse_bottom <- TTC,lighthouse_top
    //0x14 witch_switch
    //0x80 sharkfood_island


    //TTC, sandcastle
    {{0x0A, 0x01}, {0x07,0x03}, 0, 0}, //TTC,sandcastle <- TTC,ttc

    //CC, cc
    {{0x0B, 0x01}, {0x22,0x07}, WARP_FLAG_EGGS, 0}, //CC,cc <- CC,inside_left_tooth
    {{0x0B, 0x02}, {0x22,0x06}, WARP_FLAG_EGGS, 0}, //CC,cc <- CC,inside_right_tooth
    {{0x0B, 0x03}, {0x22,0x05}, 0, 0}, //CC,cc <- CC,inside_left_gil
    {{0x0B, 0x04}, {0x22,0x04}, 0, 0}, //CC,cc <- CC,inside_right_gil
    {{0x0B, 0x05}, {0x70,0x02}, 0, 0}, //CC,cc <- lair,cc_lobby
    {{0x0B, NULL}, {0x21,0x01}, 0, 0}, ////CC,witch_switch_room <- CC,cc


    //MM,mm_tickers
    {{0x0C,0x02}, {0x02,0x02}, 0, 0},     //MM,mm_tickers_lower <- MM,mm 
    {{0x0C,0x01}, {0x02,0x03}, 0, WARP_FLAG_TERMITE | WARP_FLAG_BEE | WARP_FLAG_CROC | WARP_FLAG_WALRUS | WARP_FLAG_PUMPKIN},  //MM,mm_tickers_upper <- MM,mm 

    //BGS, bgs
    {{0x0D,0x02}, {0x72,0x02}, 0, 0},     //BGS,bgs <- lair,bgs_lobby
    {{0x0D,0x03}, {0x11,0x01}, WARP_FLAG_BEAK_BUST, 0},     //BGS,bgs <- BGS,tanktup
    {{0x0D,0x04}, {0x10,0x04}, WARP_FLAG_CROC, 0},     //BGS,bgs <- BGS,vile_right  
    {{0x0D,0x05}, {0x10,0x05}, WARP_FLAG_CROC, 0},     //BGS,bgs <- BGS,vile_left  
    {{0x0D,0x06}, {0x47,0x01}, WARP_FLAG_WADING_BOOTS, 0},     //BGS,bgs <- BGS,bgs_mumbos  
    //0x14 witchswitch

    //MM,mm_mumbos
    {{0x0E,0x01}, {0x02,0x01}, 0, 0}, //MM,mm_mumbos <- MM,mm

    //BGS, vile
    {{0x10,0x04}, {0x0D,0x04}, 0, 0},     //BGS,vile_right <- BGS,bgs   
    {{0x10,0x05}, {0x0D,0x05}, 0, 0},     //BGS,vile_left <- BGS,bgs  

    //BGS,tanktup
    {{0x11,0x01}, {0x0D,0x03}, 0, 0},     //BGS,tanktup <- BGS,bgs  

    //GV,gv
    {{0x12,0x02}, {0x1A,0x01}, WARP_FLAG_EGGS, 0},      //GV,gv <- GV,jinxy
    {{0x12,0x03}, {0x13,0x01}, WARP_FLAG_BEAK_BUST, 0}, //GV,gv <- GV,matching_game
    {{0x12,0x04}, {0x14,0x01}, WARP_FLAG_EGGS, 0},      //GV,gv <- GV,sandybutt_front
    {{0x12,0x05}, {0x15,0x06}, WARP_FLAG_GV_WATER, 0},  //GV,gv <- GV,water_pyramid
    {{0x12,0x06}, {0x16,0x07}, WARP_FLAG_BEAK_BOMB | WARP_FLAG_FLIGHT, 0},  //GV,gv <- GV,rubee
    {{0x12,0x07}, {0x14,NULL}, WARP_FLAG_ONEWAY, 0},    //GV,gv <- GV,sandybutt_back 
    {{0x12,0x08}, {0x6E,0x03}, 0, 0},      //GV,gv <- lair, GV_Lobby
    {{0x12,NULL}, {0x15,0x02}, 0, WARP_FLAG_TURBO_TALON | WARP_FLAG_FLIGHT},    //GV,gv <- GV,water_pyramid_top 
    //0xA sns chamber

    //GV,matching_game
    {{0x13,0x01}, {0x12,0x03}, 0, 0}, //GV,matching_game <- GV,gv 

    //GV,sandybutt
    {{0x14,0x01}, {0x12,0x04}, 0, 0}, //sandybutt_front <- GV,gv 
    {{0x14,NULL}, {0x12,0x07}, 0, 0}, //GV,sandybutt_back <- GV,gv  

    //GV,water_pyramid
    //0x01 cutscene return
    {{0x15,0x02}, {0x12,NULL}, WARP_FLAG_ONEWAY, 0}, //GV,water_pyramid_top <- GV,gv
    {{0x15,0x06}, {0x12,0x05}, 0, 0}, //GV,water_pyramid_front <- GV,gv
    
    //GV,rubee
    {{0x16,0x07}, {0x12,0x06},  0, 0}, //GV,rubee <- GV,gv

    //GV,jinxy
    {{0x1A,0x01}, {0x12,0x02}, 0, 0}, //GV,jinxy <- GV,gv

    //MMM,mmm
    {{0x1B,0x01}, {0x26,0x01}, 0, 0}, //MMM,mmm <- MMM,dining_room_front
    {{0x1B,NULL}, {0x26,0x02}, 0, 0}, //MMM,dining_room_chimney 
    {{0x1B,0x03}, {0x25,0x01}, 0, 0}, //MMM,mmm <- MMM,well_top
    {{0x1B,0x04}, {0x24,0x01}, 0, 0}, //MMM,mmm <- MMM,tumblar
    {{0x1B,0x05}, {0x1C,0x01}, WARP_FLAG_BEAK_BUST | WARP_FLAG_TURBO_TALON, 0}, //MMM,mmm <- MMM,church
    {{0x1B,0x06}, {0x2B,0x01}, 0, 0}, //MMM,mmm <- MMM,church_secret
    {{0x1B,0x08}, {0x2F,0x02}, WARP_FLAG_PUMPKIN, 0}, //MMM,mmm <- MMM,rain_barrel_bottom
    {{0x1B,NULL}, {0x2F,0x01}, WARP_FLAG_PUMPKIN, 0}, //MMM,rain_barrel_top
    {{0x1B,0x09}, {0x1D,0x01}, 0, 0}, //MMM,mmm <- MMM,cellar
    {{0x1B,0x0A}, {0x2A,0x01}, 0, 0}, //MMM,mmm <- MMM,red_feather_room
    {{0x1B,0x0B}, {0x28,0x01}, 0, 0}, //MMM,mmm <- MMM,blue_egg_room
    {{0x1B,0x0C}, {0x2C,0x01}, 0, 0}, //MMM,mmm <- MMM,bathroom
    {{0x1B,0x0D}, {0x2E,0x01}, 0, 0}, //MMM,mmm <- MMM,honey_carrier_room
    {{0x1B,0x0E}, {0x2D,0x01}, 0, 0}, //MMM,mmm <- MMM,bedroom
    {{0x1B,0x0F}, {0x29,0x01}, 0, 0}, //MMM,mmm <- MMM,music_note_room
    {{0x1B,0x10}, {0x1B,0x11}, 0, 0}, //MMM,mmm_clock_bottom <- MMM,mmm_clock_top
    {{0x1B,0x11}, {0x1B,0x10}, 0, WARP_FLAG_INACCESSIBLE | WARP_FLAG_BEE}, //MMM,mmm_clock_top <- MMM,mmm_clock_bottom
    {{0x1B,0x12}, {0x30,0x01}, 0, 0}, //MMM,mmm <- MMM,mmm_mumbos
    {{0x1B,0x13}, {0x25,0x04}, WARP_FLAG_PUMPKIN, 0}, //MMM,mmm <- MMM,well_bottom
    {{0x1B,0x14}, {0x75,0x02}, 0, 0}, //MMM,mmm <- lair,mmm_lobby

    //MMM,church
    {{0x1C,0x01}, {0x1B,0x05}, 0, 0}, //MMM,church <- MMM,mmm

    //MMM, cellar
    {{0x1D,0x01}, {0x1B,0x09}, 0, 0}, //MMM,cellar <- MMM,mmm

    //CC, witch_switch_room
    {{0x21,0x01}, {0x0B,NULL}, WARP_FLAG_ONEWAY, 0}, //CC,witch_switch_room <- CC,cc
    {{0x21,NULL}, {0x22,0x01}, 0, 0}, // CC,inside_jiggy
    {{0x21,NULL}, {0x22,0x02}, 0, 0}, // CC,inside_ws

    //CC, inside
    {{0x22,0x01}, {0x21,NULL}, WARP_FLAG_ONEWAY, 0}, //CC,inside <-CC,ws_jiggy
    {{0x22,0x02}, {0x21,NULL}, WARP_FLAG_ONEWAY, 0},  //CC,inside <-CC,ws_ws
    {{0x22,0x03}, {0x23,0x01}, WARP_FLAG_FLIGHT, 0},  //CC,inside <-CC,wonderwing
    {{0x22,0x04}, {0x0B,0x04}, 0, 0},         //CC,inside_right_gil       <- CC,cc
    {{0x22,0x05}, {0x0B,0x03}, 0, 0},         //CC,inside_left_gil       <- CC,cc
    {{0x22,0x06}, {0x0B,0x02}, WARP_FLAG_INACCESSIBLE, 0}, //CC,inside_right_tooth    <- CC,cc
    {{0x22,0x07}, {0x0B,0x01}, WARP_FLAG_INACCESSIBLE, 0}, //CC,inside_left_tooth     <- CC,cc
    
    //CC,wonderwing
    {{0x23,0x01}, {0x22,0x03}, 0, 0},  //CC,wonderwing <- CC,inside

    //MMM,tumblar
    {{0x24,0x01}, {0x1B,0x04}, 0, 0}, //MMM,tumblar <- MMM,mmm 

    //MMM,well_top
    {{0x25,0x01}, {0x1B,0x03}, 0, 0}, //MMM,well_top <- MMM,mmm
    {{0x25,0x04}, {0x1B,0x13}, WARP_FLAG_PUMPKIN, 0}, //MMM,well_bottom <- MMM,mmm

    //MMM,dining_room
    {{0x26,0x01}, {0x1B,0x01}, 0, 0},     //MMM,dining_room_front <- MMM,mmm 
    {{0x26,0x02}, {0x1B,NULL}, WARP_FLAG_ONEWAY, 0},     //MMM,dining_room_chimney <- MMM,mmm 

    //FP,fp
    {{0x27,0x01}, {0x6F,0x06}, WARP_FLAG_TALON_TROT, 0}, //FP,fp <- FP,lobby 
    {{0x27,0x06}, {0x7F,0x01}, WARP_FLAG_WALRUS, 0},     //FP,fp <- FP,wozza 
    {{0x27,0x07}, {0x48,0x01}, 0, 0}, //FP,fp <- FP,mumbos
    {{0x27,0x08}, {0x41,0x01}, WARP_FLAG_TALON_TROT, 0}, //FP,fp <- FP,igloo 
    {{0x27,0x09}, {0x53,0x01}, 0, 0}, //FP,fp <- FP,tree 

    //MMM,blue_egg_room
    {{0x28,0x01}, {0x1B,0x0B}, 0, 0}, //MMM,blue_egg_room <- MMM,mmm

    //MMM,music_note_room
    {{0x29,0x01}, {0x1B,0x0F}, 0, 0}, //MMM,music_note_room <- MMM,mmm

    //MMM,red_feather_room
    {{0x2A,0x01}, {0x1B,0x0A}, 0, 0}, //MMM,red_feather_room <- MMM,mmm 

    //MMM,chruch_secret
    {{0x2B,0x01}, {0x1B,0x06}, 0, 0}, //MMM,church_secret <- MMM,mmm

    //MMM,bathroom
    {{0x2C,0x01}, {0x1B,0x0C}, 0, 0}, //MMM,bathroom <- MMM,mmm
    {{0x2C,0x04}, {0x8D,0x04}, WARP_FLAG_PUMPKIN, 0}, //MMM,bathroom <- MMM,logo

    //MMM,bedroom
    {{0x2D,0x01}, {0x1B,0x0E}, 0, 0}, //MMM,bedroom <- MMM,mmm

    //MMM,honey_carrier_room
    {{0x2E,0x01}, {0x1B,0x0D}, 0, 0}, //MMM,honey_carrier_room <- MMM,mmm

    //MMM,rain_barrel
    {{0x2F,0x01}, {0x1B,NULL}, WARP_FLAG_ONEWAY, 0}, //MMM,mmm <- MMM,rain_barrel_top
    {{0x2F,0x02}, {0x1B,0x06}, 0, 0}, //MMM,mmm <- MMM,rain_barrel

    //MMM,mmm_mumbos
    {{0x30,0x01}, {0x1B,0x12}, 0, 0}, //MMM,mmm_mumbos <- MMM,mmm

    //RBB, rbb
    {{0x31,0x01}, {0x3F,0x01}, 0, 0}, //RBB,rbb <- RBB,capt_quarters
    {{0x31,0x02}, {0x39,0x01}, 0, 0}, //RBB,rbb <- RBB,crew_quarters
    {{0x31,0x03}, {0x34,0x04}, 0, 0}, //RBB,rbb <- RBB,engine_button
    {{0x31,0x04}, {0x3C,0x01}, 0, 0}, //RBB,rbb <- RBB,kitchen
    {{0x31,0x05}, {0x3D,0x01}, 0, 0}, //RBB,rbb <- RBB,navigation
    {{0x31,0x06}, {0x3B,0x01}, 0, 0}, //RBB,rbb <- RBB,hull
    {{0x31,0x07}, {0x34,0x01}, 0, 0}, //RBB,rbb <- RBB,engine_gears
    {{0x31,0x08}, {0x35,0x01}, 0, 0}, //RBB,rbb <- RBB,boat_house
    {{0x31,0x09}, {0x37,0x01}, 0, 0}, //RBB,rbb <- RBB,left_container
    {{0x31,0x0A}, {0x3E,0x01}, 0, 0}, //RBB,rbb <- RBB,middle_container
    {{0x31,0x0B}, {0x38,0x01}, 0, 0}, //RBB,rbb <- RBB,right_container
    {{0x31,0x0C}, {0x3A,0x01}, 0, 0}, //RBB,rbb <- RBB,boss_boombox
    {{0x31,0x0D}, {0x36,0x01}, 0, 0}, //RBB,rbb <- RBB,warehouse_front
    {{0x31,NULL}, {0x36,0x02}, 0, 0}, //RBB,rbb <- RBB,warehouse_skylight
    {{0x31,0x10}, {0x77,0x02}, 0, 0}, //RBB,rbb <- lair,rbb_lobby
    {{0x31,0x13}, {0x8B,0x04}, 0, 0}, //RBB,rbb <- RBB,anchor_room

    //RBB,engine
    {{0x34,0x01}, {0x31,0x07}, 0, 0}, //RBB,engine_gears <- RBB,rbb
    {{0x34,0x04}, {0x31,0x03}, 0, 0}, //RBB,engine_button <- RBB,rbb 

    //RBB,boat_house
    {{0x35,0x01}, {0x31,0x08}, 0, 0}, //RBB,boat_house <- RBB,rbb 

    //RBB,warehouse
    {{0x36,0x01}, {0x31,0x0d}, 0, 0},   //RBB,warehouse_front <- RBB,rbb
    {{0x36,0x02}, {0x31,NULL}, WARP_FLAG_ONEWAY, 0}, //RBB,warehouse_skylight <- RBB,rbb

    //RBB,left_container
    {{0x37,0x01}, {0x31,0x09}, 0, 0}, //RBB,left_container <- RBB,rbb 

    //RBB, right_container
    {{0x38,0x01}, {0x31,0x0B}, 0, 0}, //RBB,right_container <- RBB,rbb

    //RBB,crew_quarters
    {{0x39,0x01}, {0x31,0x02}, 0, 0}, //RBB,crew_quarters <- RBB,rbb
    
    //RBB,boss_boombox
    {{0x3A,0x01}, {0x31,0x0C}, 0, 0}, //RBB,boss_boombox <- RBB,rbb

    //RBB,hull
    {{0x3B,0x01}, {0x31,0x06}, 0, 0}, //RBB,hull <- RBB,rbb 

    //RBB,kitchen
    {{0x3C,0x01}, {0x31,0x04}, 0, 0}, //RBB,kitchen <- RBB,rbb

    //RBB navigation
    {{0x3D,0x01}, {0x31,0x05}, 0, 0}, //RBB,navigation <- RBB,rbb 

    //RBB,middle_container
    {{0x3E,0x01}, {0x31,0x0A}, 0, 0}, //RBB,rbb <- RBB,middle_container

    //RBB,capt_quarters
    {{0x3F,0x01}, {0x31,0x01}, 0, 0}, //RBB,capt_quarters <- RBB,rbb

    //CCW,hub
    {{0x40,0x01}, {0x46,0x01}, WARP_FLAG_WINTER_OPEN, 0}, //CCW,hub <- CCW,winter
    {{0x40,0x02}, {0x43,0x01}, WARP_FLAG_SPRING_OPEN, 0}, //CCW,hub <- CCW,spring
    {{0x40,0x03}, {0x44,0x01}, WARP_FLAG_SUMMER_OPEN, 0}, //CCW,hub <- CCW,summer
    {{0x40,0x04}, {0x45,0x01}, WARP_FLAG_AUTUMN_OPEN, 0}, //CCW,hub <- CCW,autumn
    {{0x40,0x07}, {0x79,0x06}, 0, 0}, //CCW,hub <- lair,ccw_lobby

    //FP,igloo
    {{0x41,0x01}, {0x27,0x08}, 0, 0}, //FP,igloo <- FP,fp

    //CCW_Spring
    {{0x43,0x01}, {0x40,0x02}, 0, 0}, //CCW,spring <- CCW,hub
    {{0x43,0x04}, {0x5E,0x02}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,sp_nabnuts_window
    {{0x43,0x06}, {0x5A,0x01}, WARP_FLAG_BEE, 0}, //CCW,spring <- CCW,sp_zubbas
    {{0x43,0x07}, {0x5E,0x01}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,sp_nabnuts_door
    {{0x43,0x08}, {0x65,0x01}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,sp_top_room
    {{0x43,0x09}, {0x4A,0x01}, 0, 0}, //CCW,spring <- CCW,sp_mumbos

    //CCW_Summer
    {{0x44,0x01}, {0x40,0x03}, 0, 0}, //CCW,summer <- CCW,hub
    {{0x44,0x04}, {0x5F,0x02}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,su_nabnuts_window
    {{0x44,0x06}, {0x5B,0x02}, WARP_FLAG_BEAK_BUST, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,su_zubbas
    {{0x44,0x07}, {0x5F,0x01}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,su_nabnuts_door
    {{0x44,0x08}, {0x66,0x01}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,su_top_room
    {{0x44,0x09}, {0x4B,0x01}, 0, 0}, //CCW,spring <- CCW,su_mumbos

    //CCW_AUTUMN
    {{0x45,0x01}, {0x40,0x04}, 0, 0}, //CCW,autumn <- CCW,hub
    {{0x45,0x04}, {0x60,0x02}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,au_nabnuts_window
    {{0x45,0x05}, {0x5C,0x01}, WARP_FLAG_BEAK_BUST, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,au_zubbas
    {{0x45,0x06}, {0x63,0x02}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,au_flooded_room
    {{0x45,0x07}, {0x60,0x01}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,au_nabnuts_door
    {{0x45,0x08}, {0x67,0x01}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_SHOCK_SPRING}, //CCW,spring <- CCW,au_top_room
    {{0x45,0x09}, {0x4C,0x01}, 0, 0}, //CCW,autumn <- CCW,au_mumbos

    //CCW,winter
    {{0x46,0x01}, {0x40,0x01}, 0, 0}, //CCW,winter <- CCW,hub
    {{0x46,0x04}, {0x61,0x02}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_FLIGHT}, //CCW,spring <- CCW,au_nabnuts_window
    {{0x46,0x05}, {0x64,0x01}, WARP_FLAG_BEAK_BUST, WARP_FLAG_TALON_TROT | WARP_FLAG_FLIGHT}, //CCW,spring <- CCW,au_zubbas
    {{0x46,0x06}, {0x62,0x01}, 0, WARP_FLAG_FLIGHT}, //CCW,spring <- CCW,au_flooded_room
    {{0x46,0x08}, {0x68,0x01}, 0, WARP_FLAG_TALON_TROT | WARP_FLAG_FLIGHT}, //CCW,spring <- CCW,au_top_room
    {{0x46,0x09}, {0x4D,0x01}, 0, 0}, //CCW,autumn <- CCW,au_mumbos

    //BGS, bgs_mumbos
    {{0x47,0x01}, {0x0D,0x06}, 0, 0}, //BGS,bgs_mumbos <- BGS,bgs

    //FP, fp_mumbos
    {{0x48,0x01}, {0x27,0x07}, 0, 0}, //FP,fp <- FP,mumbos

    //CCW,sp_mumbos
    {{0x4A,0x01}, {0x43,0x09}, 0, 0}, //CCW,spring <- CCW,sp_mumbos
    //CCW,su_mumbos
    {{0x4B,0x01}, {0x44,0x09}, 0, 0}, //CCW,spring <- CCW,su_mumbos
    //CCW,au_mumbos
    {{0x4C,0x01}, {0x45,0x09}, 0, 0}, //CCW,spring <- CCW,au_mumbos
    //CCW,wi_mumbos
    {{0x4D,0x01}, {0x46,0x09}, 0, 0}, //CCW,spring <- CCW,au_mumbos

    //FP,tree
    {{0x53,0x01}, {0x27,0x09}, 0, 0}, //FP,tree <- FP,fp

    //CCW,zubbas
    {{0x5A,0x01}, {0x43,0x06}, WARP_FLAG_BEE, 0}, //CCW,sp_zubbas <- CCW,spring
    {{0x5B,0x02}, {0x44,0x06}, 0, 0}, //CCW,su_zubbas <- CCW,summmer
    {{0x5C,0x01}, {0x45,0x05}, 0, 0}, //CCW,au_zubbas <- CCW,autumn


    //CCW,sp_nabuts
    {{0x5E,0x01}, {0x43,0x07}, 0, 0}, //CCW,door <- CCW,spring
    {{0x5E,0x02}, {0x43,0x04}, 0, 0}, //CCW,window <- CCW,spring
    //CCW,su_nabuts
    {{0x5F,0x01}, {0x44,0x07}, 0, 0}, //CCW,door <- CCW,summer
    {{0x5F,0x02}, {0x44,0x04}, 0, 0}, //CCW,window <- CCW,summer
    //CCW,au_nabuts
    {{0x60,0x01}, {0x45,0x07}, 0, 0}, //CCW,door <- CCW,summer
    {{0x60,0x02}, {0x45,0x04}, 0, 0}, //CCW,window <- CCW,summer
    //CCW,wi_nabuts
    {{0x61,0x01}, {0x46,0x07}, 0, 0}, //CCW,door <- CCW,summer
    {{0x61,0x02}, {0x46,0x04}, 0, 0}, //CCW,window <- CCW,summer

    //CCW,wi_nabnut_hoard
    {{0x62,0x01}, {0x46,0x06}, 0, 0}, //CCW,au_flooded_room <- CCW,autumn

    //CCW,au_flooded_room
    {{0x63,0x01}, {0x45,0x06}, 0, 0}, //CCW,au_flooded_room <- CCW,autumn
    //CCW,wi_flooded_room
    {{0x64,0x01}, {0x46,0x05}, 0, 0}, //CCW,au_flooded_room <- CCW,autumn


    //CCW,sp_top_room
    {{0x65,0x01}, {0x43,0x08}, 0, 0}, //CCW,spring <- CCW,sp_top_room
    //CCW,su_top_room
    {{0x66,0x01}, {0x44,0x08}, 0, 0}, //CCW,summer <- CCW,su_top_room
    //CCW,au_top_room
    {{0x67,0x01}, {0x45,0x08}, 0, 0}, //CCW,autumn <- CCW,au_top_room
    //CCW,wi_top_room
    {{0x68,0x01}, {0x46,0x08}, 0, 0}, //CCW,autumn <- CCW,au_top_room

    //lair,mm_lobby
    {{0x69,0x01}, {0x6A,0x01}, WARP_FLAG_50_NOTES, 0}, //lair,mm_lobby <- lair,ttc_cc_puzzles 
    {{0x69,0x02}, {0x02,0x04}, WARP_FLAG_MM_OPEN, 0}, //lair,mm_lobby <- MM,mm 
    {{0x69,0x12}, {0x01,0x13}, 0,0}, //lair,mm_loby <- SM,sm

    //lair, ttc_cc_puzzles
    {{0x6A,0x01}, {0x69,0x01}, 0, 0},//ttc_cc_puzzles <- mm_lobby
    {{0x6A,0x02}, {0x6B,0x01}, 0, 0},//ttc_cc_puzzles <- ccw_puzzle
    //0x08 purple cauldron

    //lair,ccw_puzzle
    {{0x6B,0x01},{0x6A,0x02}, 0, 0},//ccw_puzzle <- ttc_cc_puzzles
    {{0x6B,0x02},{0x6C,0x01}, 0, 0},//ccw_puzzle <- red_cauldron_room
    {{0x6B,0x03},{0x6D,0x01}, 0, 0},//ccw_puzzle <- ttc_lobby
    {{0x6B,0x04},{0x70,0x01}, 0, 0},//ccw_puzzle <- cc_lobby
    {{0x6B,0x05},{0x71,0x02}, WARP_FLAG_180_NOTES | WARP_FLAG_TALON_TROT, 0},//ccw_puzzle <- 260_nd

    //lair,red_cauldron_room
    {{0x6C,0x01}, {0x6B,0x02}, 0, 0},//red_cauldron_room <- ccw_puzzle

    //lair,ttc_lobby
    {{0x6D,0x01},{0x6B,0x03}, 0, 0},//ttc_lobby <- ccw_puzzle
    {{0x6D,0x04},{0x07,0x04}, WARP_FLAG_TTC_OPEN, 0},//ttc_lobby <- TTC,ttc

    //lair, gv_lobby
    {{0x6E,0x01}, {0x71,0x01}, 0, 0},    //lair, GV_Lobby <- lair,260 note door
    {{0x6E,0x02}, {0x6F,0x01}, 0, 0},    //lair, GV_Lobby <- lair,fp_lobby
    {{0x6E,0x03}, {0x12,0x08}, WARP_FLAG_GV_OPEN, 0}, //lair, GV_Lobby <- GV,gv
    {{0x6E,NULL}, {0x71,0x04}, 0, 0},//gv_lobby_vase - 260_nd 

    //lair, fp_lobby
    {{0x6F,0x01}, {0x6E,0x02}, 0, 0},    //lair,fp_lobby <- lair, GV_Lobby
    {{0x6F,0x02}, {0x76,0x01}, WARP_FLAG_450_NOTES, 0},    //lair,fp_lobby <- lair, 640_nd
    {{0x6F,0x05}, {0x73,0x02}, WARP_FLAG_350_NOTES, 0},    //lair,fp_lobby <- lair, gv_puzzle
    {{0x6F,0x06}, {0x27,0x01}, WARP_FLAG_FP_OPEN, 0},    //lair,fp_lobby <- FP,fp
    //0x08 purple_cauldron
    //0x09 green_cauldron

    //lair,cc_lobby
    {{0x70,0x01}, {0x6B, 0x04}, 0, 0}, //lair,cc_lobby <- CC,cc
    {{0x70,0x02}, {0x0B, 0x05}, WARP_FLAG_BEAK_BUST | WARP_FLAG_CC_OPEN, 0}, //lair,cc_lobby <- CC,cc

    //260 note door
    {{0x71,0x01}, {0x6E,0x01}, WARP_FLAG_SHOCK_SPRING | WARP_FLAG_260_NOTES, 0},//260_nd <- gv_lobby_260door
    {{0x71,0x02}, {0x6B,0x05}, 0, 0},//260_nd <- ccw_puzzle
    {{0x71,0x03}, {0x72,0x01}, 0, 0},//260_nd <- bgs_lobby
    {{0x71,0x04}, {0x6E,NULL}, WARP_FLAG_ONEWAY, 0},//260_nd <- gv_lobby_vase

    //lair,bgs_lobby
    {{0x72,0x01}, {0x71,0x03}, 0, 0},//bgs_lobby <- 260_nd 
    {{0x72,0x02}, {0x0D,0x02}, WARP_FLAG_BGS_OPEN, 0}, //lair,bgs_lobby <- BGS,bgs

    //lair,gv_puzzle
    {{0x74,0x01}, {0x6F,0x05}, 0, 0}, //lair,gv_puzzle <- lair,fp_lobby
    {{0x74,0x02}, {0x75,0x01}, 0, 0}, //lair,gv_puzzle <- lair,mmm_lobby

    //lair,mmm_lobby
    {{0x75,0x01}, {0x74,0x02}, 0, 0}, //lair,gv_puzzle <- lair,mmm_lobby
    {{0x75,0x02}, {0x1B,0x14}, WARP_FLAG_MMM_OPEN, 0}, //lair,mmm_lobby <- MMM,mmm
    {{0x75,0x03}, {0x7A,0x01}, WARP_FLAG_PUMPKIN, 0}, //lair,mmm_lobby <- MMM,crypt

    //lair, 640_nd
    {{0x76,0x01}, {0x6F,0x02}, 0, 0}, //lair, 640_nd <- lair,fp_lobby
    {{0x76,0x02}, {0x79,0x02}, WARP_FLAG_INACCESSIBLE, 0}, //lair, 640_nd <- lair,rbb_lobby
    {{0x76,0x03}, {0x79,0x01}, WARP_FLAG_640_NOTES | WARP_FLAG_WATER_LVL3, 0}, //lair, 640_nd <- lair,rbb_lobby 
    {{0x76,0x04}, {0x77,0x01}, 0, 0}, //lair, 640_nd <- lair,rbb_lobby

    //lair,rbb_lobby
    {{0x77,0x01}, {0x76,0x04}, 0, 0}, //lair, 640_nd <- lair,rbb_lobby
    {{0x77,0x02}, {0x31,0x10}, WARP_FLAG_RBB_OPEN | WARP_FLAG_WATER_LVL2, 0}, //lair,rbb_lobby <- RBB,rbb
    {{0x77,0x03}, {0x78,0x02}, WARP_FLAG_WATER_LVL3, 0}, //lair,rbb_lobby <- RBB,rbb_puzzle_upper
    {{0x77,0x04}, {0x78,0x01}, 0, 0}, //lair,rbb_lobby <- RBB,rbb_puzzle_mmm

    //rbb_mmm_puzzles
    //CHANCE TO SOFTLOCK (ALWAYS DESPAWN GRATE)
    {{0x78,0x01}, {0x77,0x03}, WARP_FLAG_WATER_LVL2, 0}, //lair,rbb_lobby <- RBB,rbb_puzzle_rbb
    {{0x78,0x02}, {0x77,0x04}, 0, 0}, //lair,rbb_lobby <- RBB,rbb_puzzle_mmm

    //lair,ccw_lobby
    {{0x79,0x01}, {0x76,0x03}, 0, 0}, //lair, ccw_lobby <- lair, 640_nd
    {{0x79,0x02}, {0x76,0x02}, 0, 0}, //lair,ccw_lobby <- lair, 640_nd_upper
    {{0x79,0x03}, {0x80,0x01}, WARP_FLAG_765_NOTES, 0}, //lair,ccw_lobby <- lair,ff_lobby
    {{0x79,0x06}, {0x40,0x07}, WARP_FLAG_CCW_OPEN | WARP_FLAG_TALON_TROT, 0}, //lair,ccw_lobby <- CCW,hub
    //0x08 red_cauldron

    //lair,crypt
    {{0x7A,0x01}, {0x75,0x03}, WARP_FLAG_PUMPKIN, 0}, //MMM,crypt <- lair,mmm_lobby 

    //FP,wozza_cave
    {{0x7F,0x01}, {0x27,0x06}, WARP_FLAG_WALRUS, 0},     //FP,fp <- FP,wozza 

    //lair,ff_lobby
    {{0x80,0x01}, {0x79,0x03}, 0, 0}, //lair, 640_nd_upper <- lair,ff_lobby
    {{0x80,0x02}, {0x8E,0x01}, 0, 0}, //lair, 640_nd_upper <- lair,ff_lobby

    //RBB,anchor_room
    {{0x8B,0x04}, {0x31,0x13}, 0, 0}, //RBB,anchor_room <- RBB,rbb

    //SM,banjo_house
    {{0x8C,0x01},{0x01,0x12}, 0, 0}, //SM,banjo_house <- SM,sm 

    //MMM,logo
    {{0x8D,0x04}, {0x2C,0x04}, WARP_FLAG_PUMPKIN, 0}, //MMM,logo <- MMM,bathroom

    //FF
    {{0x8E,0x01}, {0x80,0x02}, 0, 0}, //lair, ff <- lair,ff_lobby
    {{0x8E,0x05}, {0x93,0x05}, 0, 0}, //lair, ff <- lair,810_nd
    //yellow_cauldron

    //FIGHT
    //TODO handle seperate
    {{0x90,0x01}, {0x93,0x01}, 0, 0}, //lair,810_nd <- FIGHT

    //lair 810_nd
    //TODO this room sucks
    {{0x93,0x01}, {0x90,0x01}, WARP_FLAG_DOG_OPEN, 0}, //lair,810_nd <- FIGHT
    {{0x93,0x05}, {0x8E,0x05}, WARP_FLAG_810_NOTES, 0}, //lair,810_nd <- lair, ff
    //0x08 yellow cauldron

    {{0,0}, {0,0}, 0,0}
};