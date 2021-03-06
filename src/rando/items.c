/*************************************************************************
*   Copyright (C) 2020 Michael (Mittenz) Salino-Hugg
*   Contact: Twitter: @MittenzPhD
**************************************************************************
*	items.c
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

#include "items.h"
#include "bk.h"

items_data_t items_table[]={
    { BK_ITEM_MUSIC_NOTE        , "music notes"},
    { BK_ITEM_EGG               , "eggs"},
    { BK_ITEM_JIGGY_LEVEL       , "jiggies"},
    { BK_ITEM_RED_FEATHER       , "red feathers"},
    { BK_ITEM_GOLD_FEATHER      , "gold feathers"},
    //{ BK_ITEM_JINJO                  = 0x12, ""}, //handle differently
    { BK_ITEM_EMPTY_HONEYCOMB   , "empty honeycomb"},
    { BK_ITEM_HEALTH            , "health"},
    { BK_ITEM_HEALTH_MAX        , "health (total)"},
    { BK_ITEM_LIFE              , "lives"},
    { BK_ITEM_AIR               , "air"},
    { BK_ITEM_MUMBO_TOKEN_UNUSED, "mumbo tokens"},
    { BK_ITEM_MUMBO_TOKEN_TOTAL , "mumbo tokens (total)"},
    { BK_ITEM_JIGGY_TOTAL       , "jiggies (total)"},
    { 0, NULL},
};

items_data_t level_items_table[]={
    { BK_ITEM_BLUBBERS_GOLD     , "blubber's gold"},
    { BK_ITEM_CHIMPYS_ORANGE    , "chimpy's orange"},
    //{ BK_ITEM_VILE_SCORE_PLAYER , "player's vile score"},
    //{ BK_ITEM_VILE_SCORE_VILE   , "vile's vile score"},
    //{ BK_ITEM_GRUMBLIE          , "grumblie"},
    //{ BK_ITEM_YUMBLIE           , "yumblie"},
    { BK_ITEM_PRESENT_GREEN     , "present (green)"},
    { BK_ITEM_PRESENT_BLUE      , "present (blue)"},
    { BK_ITEM_PRESENT_RED       , "present (red)"},
    { BK_ITEM_CATEPILLAR        , "caterpillars"},
    { BK_ITEM_ACORN             , "acorns"},
    //{ BK_ITEM_TWINKLIE          , "twinklie score"},
    { BK_ITEM_JOKER_CARD        , "joker cards"},
    { 0, NULL},
};

items_data_t timer_items_table[]={
    { BK_ITEM_SKULL_HOURGLASS   , "skull hourglass"},
    { BK_ITEM_PROPELLOR_TIMER   , "propellor timer"},
    //{ BK_ITEM_CHRISTMAS_TREE_TIMER   = 0x5, ""}, //crashes game
    { BK_ITEM_HOURGLASS         , "hourglass"},
    { 0, NULL},
};



jiggy_data_t jiggy_table[]={
    /*MM*/
    { 0x01, 1, "jinjo"},
    { 0x02, 1, "ticker's tower"},
    { 0x03, 1, "mumbo's skull"},
    { 0x04, 1, "juju"},
    { 0x05, 1, "huts"},
    { 0x06, 1, "ruins"},
    { 0x07, 1, "hill"},
    { 0x08, 1, "orange switch"},
    { 0x09, 1, "chimpy"},
    { 0x0A, 1, "conga"},
    /*TTC*/
    { 0x0B, 2, "jinjo"},
    { 0x0C, 2, "lighthouse"},
    { 0x0D, 2, "alcove 1"},
    { 0x0E, 2, "alcove 2"},
    { 0x0F, 2, "pool"},
    { 0x10, 2, "sandcastle"},
    { 0x11, 2, "red x's"},
    { 0x12, 2, "nipper"},
    { 0x13, 2, "lockup"},
    { 0x14, 2, "blubber"},
    /*CC*/
    { 0x15, 3, "jinjo"},
    { 0x16, 3, "snippets"},
    { 0x17, 3, "back"},
    { 0x18, 3, "bolt"},
    { 0x19, 3, "tail"},
    { 0x1A, 3, "long pipe"},
    { 0x1B, 3, "tooth"},
    { 0x1C, 3, "rings"},
    { 0x1D, 3, "slow sawblades"},
    { 0x1E, 3, "fast sawblades"},
    /*BGS*/
    { 0x1F, 4, "jinjo"},
    { 0x20, 4, "elevated Walkway"},
    { 0x21, 4, "egg"},
    { 0x22, 4, "croctus"},
    { 0x23, 4, "mud huts"},
    { 0x24, 4, "flibbits"},
    { 0x25, 4, "maze"},
    { 0x26, 4, "tanktup"},
    { 0x27, 4, "tiptup"},
    { 0x28, 4, "mr. vile"},
    /*FP*/
    { 0x29, 5, "jinjo"},
    { 0x2A, 5, "boggy 1"},
    { 0x2B, 5, "pipe"},
    { 0x2B, 5, "boggy 3"},
    { 0x2D, 5, "snowman buttons"},
    { 0x2E, 5, "presents"},
    { 0x2F, 5, "christmas tree"},
    { 0x30, 5, "boggy 2"},
    { 0x31, 5, "sir slush"},
    { 0x32, 5, "wozza"},
    /*Lair*/
    { 0x33, 6, "first jiggy"},
    { 0x34, 6, "mumbo's mountain witch switch"},
    { 0x35, 6, "clanker's cavern witch switch"},
    { 0x36, 6, "treasure trove cove witch switch"},
    { 0x37, 6, "bubblegloop swamp witch switch"},
    { 0x38, 6, "freezeezy peak witch switch"},
    { 0x39, 6, "mad monster mansion witch switch"},
    { 0x3A, 6, "gobi's valley witch switch"},
    { 0x3B, 6, "rusty bucket bay witch switch"},
    { 0x3C, 6, "click clock wood witch switch"},
    /*GV*/
    { 0x3D, 7, "jinjo"},
    { 0x3E, 7, "grabba"},
    { 0x3F, 7, "jinxy"},
    { 0x40, 7, "matching puzzle"},
    { 0x41, 7, "maze"},
    { 0x42, 7, "water pyramid"},
    { 0x43, 7, "histup"},
    { 0x44, 7, "gobi 1"},
    { 0x45, 7, "gobi 2"},
    { 0x46, 7, "ancient ones"},
    /*CCW*/
    { 0x47, 8, "jinjo"},
    { 0x48, 8, "house"},
    { 0x49, 8, "eeyrie"},
    { 0x4A, 8, "nabnut"},
    { 0x4B, 8, "gnawty"},
    { 0x4C, 8, "zubbas"},
    { 0x4D, 8, "flower"},
    { 0x4E, 8, "summer leaf jumps"},
    { 0x4F, 8, "tree top"},
    { 0x50, 8, "top room"},
    /*RBB*/
    { 0x51, 9, "jinjo"},
    { 0x52, 9, "warehouse"},
    { 0x53, 9, "snorkel"},
    { 0x54, 9, "whistle"},
    { 0x55, 9, "funnel"},
    { 0x56, 9, "boss boom box"},
    { 0x57, 9, "propellor"},
    { 0x58, 9, "captains cabin"},
    { 0x59, 9, "crane cage"},
    { 0x5A, 9, "engine room"},
    /*MMM*/
    { 0x5B, 10, "jinjo"},
    { 0x5C, 10, "well"},
    { 0x5D, 10, "napper"},
    { 0x5E, 10, "cellar"},
    { 0x5F, 10, "church roof"},
    { 0x60, 10, "motzhand"},
    { 0x61, 10, "rain barrel"},
    { 0x62, 10, "tumblar"},
    { 0x63, 10, "flower pots"},
    { 0x64, 10, "loggo"},
    { 0, 0, NULL},
};


mumbo_token_data_t mumbo_token_table[]={
    /*MM*/
    { 0x01, 1, "by conga"},
    { 0x02, 1, "behind ruins"},
    { 0x03, 1, "mumbo's skull ramp"},
    { 0x04, 1, "by purple jinjo"},
    { 0x05, 1, "inside ticker's tower"},
    /*TTC*/
    { 0x06, 2, "inside ship"},
    { 0x07, 2, "inside lockup 1"},
    { 0x08, 2, "inside lockup 2"},
    { 0x09, 2, "ship mast"},
    { 0x0A, 2, "lighthouse"},
    { 0x0B, 2, "floating box"},
    { 0x0C, 2, "by last x"},
    { 0x0D, 2, "pool"},
    { 0x0E, 2, "shock spring pad"},
    { 0x0F, 2, "behind nipper"},
    /*CC*/
    { 0x10, 3, "tail chompa"},
    { 0x11, 3, "above world exit"},
    { 0x12, 3, "underwater alcove"},
    { 0x13, 3, "window"},
    { 0x14, 3, "clanker's mouth"},
    /*BGS*/
    { 0x15, 4, "under mud huts 1"},
    { 0x16, 4, "under mud huts 2"},
    { 0x17, 4, "above cattail"},
    { 0x18, 4, "by yellow jinjo"},
    { 0x19, 4, "above mud huts"},
    { 0x1a, 4, "behind mumbo's"},
    { 0x1b, 4, "elevated walkway"},
    { 0x1c, 4, "in tanktup"},
    { 0x1d, 4, "mr. vile"},
    { 0x1e, 4, "in mumbo's"},
    /*FP*/
    { 0x1f, 5, "snowman leg 1"},
    { 0x20, 5, "snowman leg 2m"},
    { 0x21, 5, "present stack"},
    { 0x22, 5, "chimney fly pad"},
    { 0x23, 5, "sir slush island"},
    { 0x24, 5, "sir slush present"},
    { 0x25, 5, "xmas tree base"},
    { 0x26, 5, "scarf sled"},
    { 0x27, 5, "water by wozza"},
    { 0x28, 5, "in boggy's igloo"},
    /*GV*/
    { 0x29, 7, "jinxy's nose"},
    { 0x2a, 7, "in sand by jinxy"},
    { 0x2b, 7, "moat"},
    { 0x2c, 7, "over maze pyramid"},
    { 0x2d, 7, "water temple door"},
    { 0x2e, 7, "matching pyramid"},
    { 0x2f, 7, "in maze pyramid"},
    { 0x30, 7, "in water pyramid"},
    { 0x31, 7, "rubee's pyramid"},
    { 0x32, 7, "in jinxy"},
    /*MMM*/
    { 0x33, 10, "by fountain"},
    { 0x34, 10, "by tumblar shed"},
    { 0x35, 10, "church roof"},
    { 0x36, 10, "hedges by ramp"},
    { 0x37, 10, "hedge maze"},
    { 0x38, 10, "cemetary"},
    { 0x39, 10, "in fountain whip"},
    { 0x3a, 10, "church rafters"},
    { 0x3b, 10, "organ stool"},
    { 0x3c, 10, "tumblar shed roof"},
    { 0x3d, 10, "cellar"},
    { 0x3d, 10, "loggo"},
    { 0x3e, 10, "dinning room"},
    { 0x3f, 10, "well"},
    { 0x40, 10, "bedroom"},
    { 0x41, 10, "bathroom"},
    /*RBB*/
    { 0x42, 9, "top of funnel"},
    { 0x43, 9, "front of ship"},
    { 0x44, 9, "lifeboat"},
    { 0x45, 9, "above tollway"},
    { 0x46, 9, "toxic pool"},
    { 0x47, 9, "in front of chompa"},
    { 0x48, 9, "left container"},
    { 0x49, 9, "middle container"},
    { 0x4a, 9, "crew cabin"},
    { 0x4b, 9, "navigation room"},
    { 0x4c, 9, "kitchen oven"},
    { 0x4d, 9, "engine room left"},
    { 0x4e, 9, "engine room right"},
    { 0x4f, 9, "engine room mid"},
    { 0x50, 9, "store room"},
    /*lair*/
    { 0x51, 6, "purple cauldron"},
    { 0x52, 6, "by ccw puzzle"},
    { 0x53, 6, "red cauldron"},
    { 0x54, 6, "above cc entrance"},
    { 0x55, 6, "behind gv coffin"},
    { 0x56, 6, "advent calendar"},
    { 0x57, 6, "in crypt"},
    { 0x58, 6, "640 note door"},
    { 0x59, 6, "rbb entrance"},
    { 0x5a, 6, "mmm puzzle"},
    /*ccw*/
    { 0x5b, 8, "spring house"},
    { 0x5c, 8, "spring low branch"},
	//0x5d unused. supposed to be brambles or eyrie???
    { 0x5e, 8, "spring brambles"},
    { 0x5e, 8, "spring eyrie"},
    { 0x5f, 8, "spring garden snare"},
    { 0x60, 8, "spring entrance"},
    { 0x61, 8, "spring hive"},
    { 0x62, 8, "nabnut's house"},
    { 0x63, 8, "summer eyrie"},
    { 0x64, 8, "summer garden corner"},
    { 0x65, 8, "summer bramble snare"},
    { 0x66, 8, "summer low branch"},
    { 0x67, 8, "summer gnawty's"},
    { 0x68, 8, "summer leaf jumps"},
    { 0x69, 8, "summer in mumbo's"},
    { 0x6a, 8, "autumn leaf jumps"},
    { 0x6b, 8, "autumn entrance"},
    { 0x6c, 8, "autumn top"},
    { 0x6c, 8, "autumn by house"},
    { 0x6e, 8, "autumn low branch"},
    { 0x70, 8, "winter flower"},
    { 0x70, 8, "winter river fly pad"},
    { 0x71, 8, "winter hive"},
    { 0x72, 8, "winter nabnut"},
    { 0x73, 8, "winter sir slush"},
    { 0, 0, NULL}
};

honeycomb_data_t empty_honeycomb_table[]={
    { 0x01, 1, "hill"},
    { 0x02, 1, "juju"},
    { 0x03, 2, "underwater"},
    { 0x04, 2, "floating box"},
    { 0x05, 3, "underwater"},
    { 0x06, 3, "above water"},
    { 0x07, 4, "in mumbo's"},
    { 0x08, 4, "in tanktup"},
    { 0x09, 5, "in wozza's cave"},
    { 0x0a, 5, "sir slush"},
    { 0x0b, 7, "cactus"},
    { 0x0c, 7, "gobi 3"},
    { 0x0d, 8, "gnawty's"},
    { 0x0e, 8, "nabnut's"},
    { 0x0f, 9, "boat house"},
    { 0x10, 9, "engine room"},
    { 0x11, 10, "church rafters"},
    { 0x12, 10, "floorboard"},
    { 0x13, 11, "stump"},
    { 0x14, 11, "waterfall"},
    { 0x15, 11, "underwater"},
    { 0x16, 11, "tree"},
    { 0x17, 11, "coliwobble"},
    { 0x18, 11, "quarries"},
    {0,0,NULL}
};