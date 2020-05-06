#include "tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "jiggy.h"

#ifdef DEBUG
#define D_PRINTF(f_, ...) printf((f_), ##__VA_ARGS__)
#else
#define D_PRINTF(f_, ...) while(false)
#endif
static map_e _warp_get_map_index(exit_t* warp){
    map_e imap = warp->map;
    if(imap == TTC_SHIP){
        if(warp->exit == 0x05){
            imap = TTC_SHIP_TOP;
        } else if(warp->exit == 0x06){
            imap = TTC_SHIP_BOTTOM;
        }
    } else if (imap == RBB_ENGINE){
        if(warp->exit == 0x01){
            imap = RBB_ENGINE_GEARS;
        } else if(warp->exit == 0x04 ){
            imap = RBB_ENGINE_BUTTON;
        }
    }
    return imap;
}

void _tree_update_maps(tree_t* this, exit_t* warp){
    map_e imap = _warp_get_map_index(warp);
    bitfield_set(this->maps, imap, true);
}

static u8  _map_get_note_count(map_e m){
    switch (m)
    {
    case MMM_MUMBOS:        //MMM mumbos
        return 2;
    case RBB_CAPT_QRTRS:    //RBB capt qrts
    case CCW_AU_NABNUTS:    //CCW au_nabnut
        return 3;
    case TTC_SHIP_BOTTOM:
    case TTC_SHIP_TOP:
    case TTC_SANDCASTLE:    //TTC sandcastle
    case MM_MUMBOS:         //MM mumbos
    case GV_MATCHING:       //GV matching
    case GV_WATER:          //GV water
    case MMM_CELLAR:        //MMM cellar
    case MMM_TUMBLAR:       //MMM tumblar
    case MMM_BEDROOM:       //MMM bedroom
    case RBB_WAREHOUSE:     //RBB warehouse
    case RBB_R_CONTAINER:   //RBB right_container
    case RBB_CREW_QRTRS:    //RBB crew quarters
    case RBB_NAVIGATION:    //RBB navigation
    case CCW_HUB:           //CCW hub
    case CCW_AU_MUMBOS:     //CCW au_mumbo
    case CCW_AU_ZUBBAS:     //CCW au_zubba
    case RBB_ANCHOR:        //RBB anchor
    case RBB_ENGINE_BUTTON:
        return 4;
    case MMM_RAIN_BARREL:   //MMM rainbarrel
    case RBB_HULL:          //RBB hull
    case RBB_KITCHEN:       //RBB kitchen
        return 5;
    case TTC_NIPPER:        //TTC nipper
    case MM_TICKERS:        //MM tickers
    case BGS_VILE:          //BGS vile
    case BGS_TANKTUP:       //BGS tiptup
    case CC_WITCH_SWITCH:   //CC ws room
    case CC_WONDERWING:     //CC wonderwitn room
    case FP_MUMBOS:         //FP mumbo's
        return 6;
    case 0x14:              //GV sandybutt
    case 0x1A:              //GV jinxy
    case 0x25:              //MMM well
        return 7;
    case 0x16:              //GV rubee
    case 0x26:              //MMM dining room
    case 0x37:              //RBB left_container
        return 8;
    case 0x29:              //MMM note door
        return 9;
    case 0x1C:              //MMM church
        return 10;
    case 0x53:              //FP xmas trees
    case RBB_ENGINE_GEARS:
        return 12;
    case 0x22:              //CC insides
    case 0x43:              //CCW spring
    case 0x44:              //CCW summer
    case 0x46:              //CCW winter
        return 16;
    case 0x47:              //CCW autumn
        return 37;
    case 0x31:              //RBB, rbb
        return 43;
    case 0x1B:              //MMM, mmm
        return 47;
    case 0x12:              //GV, gv
        return 70;
    case 0x0B:              //CC,cc
        return 72;
    case 0x07:              //TTC ttc
    case 0x27:              //FP fp
        return 82;
    case 0x0D:              //BGS bgs
        return 88;
    case 0x02:              //MM, mm
        return 90;
    default:
        return 0;
    }
}

static void _tree_update_notes(tree_t* this){
    this->note_count = 0;
    for(int i = 0; i<MAP_MAX_INDEX; i++){
        if(bitfield_get(this->maps,i+1))
            this->note_count += _map_get_note_count(i+1);
    }
}

static void _tree_update_moves(tree_t* this){
    for(int i = 0; i < MOVES_MAX_INDEX; i++ ){
        if(!bitfield_get(this->moves, i + 1)){
            switch (i+1)
            {
            case MOVE_TALON_TROT:
            case MOVE_BEAK_BUST:
            case MOVE_EGGS:
                if(bitfield_get(this->maps, MM_MM)){
                    bitfield_set(this->moves, i+1, true);
                    D_PRINTF("    FLAG: TALON TROT, BEAK_BUST, OR EGGS LEARNED\n");
                }
                break;
            case MOVE_FLIGHT:
                if ( bitfield_get(this->maps, TTC_TTC)
                    && bitfield_get(this->moves, TTC_TTC)){
                    bitfield_set(this->moves, i+1, true);
                    D_PRINTF("    FLAG: FLIGHT LEARNED\n");
                }
                break;
            case MOVE_SHOCK_SPRING:
                if(bitfield_get(this->maps, TTC_TTC)){
                    bitfield_set(this->moves, i+1, true);
                    D_PRINTF("    FLAG: SHOCK SPRING JUMP LEARNED\n");
                }
                break;
            case MOVE_WADING_BOOTS:
                if(bitfield_get(this->maps, BGS_BGS)){
                    bitfield_set(this->moves, i+1, true);
                    D_PRINTF("    FLAG: WADING BOOTS LEARNED\n");
                }
                break;
            case MOVE_BEAK_BOMB:
                if(bitfield_get(this->maps, FP_FP)){
                    bitfield_set(this->moves, i+1, true);
                    D_PRINTF("    FLAG: BEAK BOMB LEARNED\n");
                }
                break;
            case MOVE_TURBO_TRAINERS:
                if(bitfield_get(this->maps, GV_GV)
                  && (bitfield_get(this->moves, MOVE_TALON_TROT))){
                    bitfield_set(this->moves, i+1, true);
                    D_PRINTF("    FLAG: TURBO TRAINERS LEARNED\n");
                }
                break;
            case MOVE_WONDERWING:
                if(bitfield_get(this->maps, CC_WONDERWING)){
                    bitfield_set(this->moves, i+1, true);
                    D_PRINTF("    FLAG: WONDERWING LEARNED\n");
                }
                break;
            default:
                break;
            }
        }
    }
    return;
}

static u8  _map_get_token_count(map_e m){
    switch (m)
    {
    case 0x05: //TTC, blubbers
    case 0x0C: //MM, tickers
    case 0x10: //BSG, vile
    case 0x11: //BSG, tanktup
    case 0x13: //GV, matching
    case 0x14: //GV, sandybutt
    case 0x15: //GV, water
    case 0x16: //GV, rubee
    case 0x1A: //GV, jinxy
    case 0x1D: //MMM, cellar
    case 0x22: //CC, inside
    case 0x25: //MMM, well
    case 0x26: //MMM, dining room
    case 0x2C: //MMM, bathroom
    case 0x2D: //MMM, bedroom
    case 0x38: //RBB, left container
    case 0x39: //RBB, crew quarters
    case 0x3B: //RBB, storeroom
    case 0x3C: //RBB, kitchen
    case 0x3D: //RBB, Navigation
    case 0x3E: //RBB, middle container 
    case 0x41: //FP, igloo
    case 0x47: //BGS, mumbos
    case 0x4B: //CCW, summer_mumbos
    case 0x61: //CCW, winter nabnut
    case 0x6A: //lair, ttc_cc_puzzle
    case 0x6B: //lair, ccw_puzzle
    case 0x6C: //lair, red_cauldron_room
    case 0x6E: //lair, GV_lobby
    case 0x6F: //lair, FP_lobby
    case 0x70: //lair, cc_lobby
    case 0x76: //lair, 640_nd
    case 0x77: //lair, rbb_lobby
    case 0x78: //lair, mmm_puzzle
    case 0x7A: //lair, crypt
        return 1;
    case 0x1C: //MMM, church
        return 2;
    case 0x34: //RBB, engine
        return 3;
    case 0x02: //MM, mm
    case 0x0B: //CC, cc
    case 0x46: //CCW, winter
        return 4;
    case 0x12: //GV, gv
    case 0x45: //CCW, autumn
        return 5;
    case 0x31: //RBB, rbb
    case 0x43: //CCW, spring
    case 0x44: //CCW, summer
        return 6;
    case 0x0D: //BGS, bgs
        return 7;
    case 0x1B: //MMM, mmm
        return 8;
    case 0x07: //TTC, ttc
    case 0x27: //FP, fp
        return 9;
    default:
        return 0;
    }
    return 0;
}

static void _tree_update_tokens(tree_t* this, exit_t* warp){
    this->token_count += _map_get_token_count(_warp_get_map_index(warp)); 
}

static void _tree_update_transformations(tree_t* this){ 
    for(int i = 0; i < 6; i++){
        if(!bitfield_get(this->tranformations, i+1)){
            switch (i+1)
            {
            case TRANS_BANJO:
                bitfield_set(this->tranformations, i+1,true);
                break;
            case TRANS_TERMITE:
                if(this->token_count - this->token_spent >= 5){
                    if(bitfield_get(this->maps, MM_MUMBOS)){
                        bitfield_set(this->tranformations, i+1,true);
                        this->token_spent += 5;
                        D_PRINTF("    FLAG: TERMITE UNLOCKED\n");
                    }
                }
                break;
            case TRANS_CROC:
                if(this->token_count - this->token_spent >= 10){
                    if(bitfield_get(this->maps, BGS_MUMBOS)){
                        bitfield_set(this->tranformations, i+1,true);
                        this->token_spent += 10;
                        D_PRINTF("    FLAG: CROC UNLOCKED\n");
                    }
                }
                break;
            case TRANS_WALRUS:
                if(this->token_count - this->token_spent >= 15){
                    if(bitfield_get(this->maps, FP_MUMBOS)){
                        bitfield_set(this->tranformations, i+1,true);
                        this->token_spent += 15;
                        D_PRINTF("    FLAG: WALRUS UNLOCKED\n");
                    }
                }
                break;
            case TRANS_PUMPKIN:
                if(bitfield_get(this->maps, LAIR_CRYPT)){
                    bitfield_set(this->tranformations, i+1,true);
                    D_PRINTF("    FLAG: PUMPKIN UNLOCKED\n");
                }
                else if (bitfield_get(this->maps, MMM_MUMBOS)){
                    if(this->token_count - this->token_spent >= 20){
                        bitfield_set(this->tranformations, i+1,true);
                        this->token_spent += 20;
                        D_PRINTF("    FLAG: PUMPKIN UNLOCKED\n");
                    }
                }
                break;
            case TRANS_BEE:
                if(this->token_count - this->token_spent >= 25){
                    if(bitfield_get(this->maps, CCW_SP_MUMBOS)){
                        bitfield_set(this->tranformations, i+1,true);
                        this->token_spent += 25;
                        D_PRINTF("    FLAG: BEE UNLOCKED\n");
                    }
                }
                break;
            default:
                break;
            }
        }
    }
}

static void _tree_update_warp_flags(tree_t* this){
    //HARD - LEVEL OPEN
    u16* hard_open =  &(this->hard_f.level_open);
    for(int i = 0; i < 14; i++){
        if(!((*hard_open) & BIT(i))){
            switch (i)
            {
            case WF_OPEN_MM:
                if(this->jiggy_count - this->jiggy_spent >= 1){
                    if(bitfield_get(this->maps, LAIR_MM_LOBBY)){
                        D_PRINTF("    FLAG: MM OPENED\n");
                        *hard_open |= BIT(i);
                        this->jiggy_spent += 1;
                    }
                }
                break;
            case WF_OPEN_TTC:
                if(this->jiggy_count - this->jiggy_spent >= 2){
                    if(bitfield_get(this->maps, LAIR_TTC_CC_PUZ)){
                        D_PRINTF("    FLAG: TTC OPENED\n");
                        *hard_open |= BIT(i);
                        this->jiggy_spent += 2;
                    }
                }
                break;
            case WF_OPEN_CC:
                if(this->jiggy_count - this->jiggy_spent >= 5){
                    if(bitfield_get(this->maps, LAIR_TTC_CC_PUZ)){
                        if(bitfield_get(this->moves, MOVE_SHOCK_SPRING)){
                            D_PRINTF("    FLAG: CC OPENED\n");
                            *hard_open |= BIT(i);
                            this->jiggy_spent += 5;
                        }
                    }
                }
                break;
            case WF_OPEN_BGS:
                if(this->jiggy_count - this->jiggy_spent >= 7){
                    if(bitfield_get(this->maps, LAIR_CC_LOBBY)){
                        if(bitfield_get(this->moves, MOVE_BEAK_BUST)){
                            D_PRINTF("    FLAG: BGS OPENED\n");
                            *hard_open |= BIT(i);
                            this->jiggy_spent += 7;
                        }
                    }
                }
                break;
            case WF_OPEN_FP:
                if(this->jiggy_count - this->jiggy_spent >= 8){
                    if(bitfield_get(this->maps, LAIR_BGS_LOBBY)){
                        if(bitfield_get(this->moves, MOVE_WADING_BOOTS)){
                            D_PRINTF("    FLAG: FP OPENED\n");
                            *hard_open |= BIT(i);
                            this->jiggy_spent += 8;
                        }
                    }
                }
                break;
            case WF_OPEN_GV:
                if(this->jiggy_count - this->jiggy_spent >= 9){
                    if(bitfield_get(this->maps, LAIR_GV_PUZ)){
                        D_PRINTF("    FLAG: GV OPENED\n");
                        *hard_open |= BIT(i);
                        this->jiggy_spent += 9;
                    }
                }
                break;
            case WF_OPEN_MMM:
                if(this->jiggy_count - this->jiggy_spent >= 10){
                    if(bitfield_get(this->maps, LAIR_RBB_MMM_PUZ)
                    && bitfield_get(this->maps, LAIR_CRYPT) //water lvl 2, lvl 3 requires lvl 
                    && bitfield_get(this->moves, MOVE_BEAK_BUST)){
                        D_PRINTF("    FLAG: MMM OPENED\n");
                        *hard_open |= BIT(i);
                        this->jiggy_spent += 10;
                    }
                }
                break;
            case WF_OPEN_RBB:
                if(this->jiggy_count - this->jiggy_spent >= 12){
                    if(bitfield_get(this->maps, LAIR_RBB_MMM_PUZ)
                    && bitfield_get(this->maps, LAIR_CRYPT) //water lvl 2, lvl 3 requires lvl 
                    && bitfield_get(this->moves, MOVE_BEAK_BUST)
                    && bitfield_get(this->maps, LAIR_RBB_LOBBY)){
                        D_PRINTF("    FLAG: RBB OPENED\n");
                        *hard_open |= BIT(i);
                        this->jiggy_spent += 12;
                    }
                }
                break;
            case WF_OPEN_CCW:
                if(this->jiggy_count - this->jiggy_spent >= 15){
                    if(bitfield_get(this->maps, LAIR_CCW_PUZ)){
                        D_PRINTF("    FLAG: CCW OPENED\n");
                        *hard_open |= BIT(i);
                        this->jiggy_spent += 15;
                    }
                }
                break;
            case WF_OPEN_SPRING:
                if(bitfield_get(this->maps, CCW_HUB)
                && bitfield_get(this->moves, MOVE_BEAK_BUST)){
                    D_PRINTF("    FLAG: SPRING OPENED\n");
                    *hard_open |= BIT(i);
                }
                break;
            case WF_OPEN_SUMMER:
                if(bitfield_get(this->maps, CCW_SPRING)
                && bitfield_get(this->moves, MOVE_BEAK_BUST)){
                    if(bitfield_get(this->moves, MOVE_TALON_TROT)
                    || bitfield_get(this->moves, MOVE_SHOCK_SPRING)){
                        D_PRINTF("    FLAG: SUMMER OPENED\n");
                        *hard_open |= BIT(i);
                    }
                }
                break;
            case WF_OPEN_AUTUMN:
                if(bitfield_get(this->maps, CCW_SUMMER)
                && bitfield_get(this->moves, MOVE_BEAK_BUST)){
                    D_PRINTF("    FLAG: AUTUMN OPENED\n");
                    *hard_open |= BIT(i);
                }
                break;
            case WF_OPEN_WINTER:
                if(bitfield_get(this->maps, CCW_AUTUMN)
                && bitfield_get(this->moves, MOVE_BEAK_BUST)){
                    if(bitfield_get(this->moves, MOVE_TALON_TROT)
                    || bitfield_get(this->moves, MOVE_SHOCK_SPRING)){
                        D_PRINTF("    FLAG: WINTER OPENED\n");
                        *hard_open |= BIT(i);
                    }
                }
                break;
            case WF_OPEN_DOG:
                if(this->jiggy_count - this->jiggy_spent >= 25){
                    if(bitfield_get(this->maps, LAIR_810_ND)){
                        if(this->note_count >= 810){
                            D_PRINTF("    FLAG: DOG OPENED\n");
                            *hard_open |= BIT(i);
                            this->jiggy_spent += 25;
                        }
                    }
                }
                break;
            default:
                break;
            }
        }
    }
    //NOTES
    u8* hard_notes =  &(this->hard_f.note_door);
    if(this->note_count >= 810 && !(*hard_notes & BIT(WF_810_NOTES))){
        D_PRINTF("    FLAG: 810 NOTE DOOR OPENED\n");
        *hard_notes |=   BIT(WF_810_NOTES) | BIT(WF_765_NOTES) | BIT(WF_640_NOTES)
                       | BIT(WF_450_NOTES) | BIT(WF_350_NOTES) | BIT(WF_260_NOTES)
                       | BIT(WF_180_NOTES) | BIT(WF_50_NOTES);
    }
    else if(this->note_count >= 765 && !(*hard_notes & BIT(WF_765_NOTES))){
        D_PRINTF("    FLAG: 765 NOTE DOOR OPENED\n");
        *hard_notes |=   BIT(WF_765_NOTES) | BIT(WF_640_NOTES)
                       | BIT(WF_450_NOTES) | BIT(WF_350_NOTES) | BIT(WF_260_NOTES)
                       | BIT(WF_180_NOTES) | BIT(WF_50_NOTES);
    }
    else if(this->note_count >= 640 && !(*hard_notes & BIT(WF_640_NOTES))){
        D_PRINTF("    FLAG: 640 NOTE DOOR OPENED\n");
        *hard_notes |=   BIT(WF_640_NOTES)
                       | BIT(WF_450_NOTES) | BIT(WF_350_NOTES) | BIT(WF_260_NOTES)
                       | BIT(WF_180_NOTES) | BIT(WF_50_NOTES);
    }
    else if(this->note_count >= 450 && !(*hard_notes & BIT(WF_450_NOTES))){
        D_PRINTF("    FLAG: 450 NOTE DOOR OPENED\n");
        *hard_notes |=   BIT(WF_450_NOTES) | BIT(WF_350_NOTES) | BIT(WF_260_NOTES)
                       | BIT(WF_180_NOTES) | BIT(WF_50_NOTES);
    }
    else if(this->note_count >= 350 && !(*hard_notes & BIT(WF_350_NOTES))){
        D_PRINTF("    FLAG: 350 NOTE DOOR OPENED\n");
        *hard_notes |=   BIT(WF_350_NOTES) | BIT(WF_260_NOTES)
                       | BIT(WF_180_NOTES) | BIT(WF_50_NOTES);
    }
    else if(this->note_count >= 260 && !(*hard_notes & BIT(WF_260_NOTES))){
        D_PRINTF("    FLAG: 260 NOTE DOOR OPENED\n");
        *hard_notes |=   BIT(WF_260_NOTES) | BIT(WF_180_NOTES) | BIT(WF_50_NOTES);
    }
    else if(this->note_count >= 180 && !(*hard_notes & BIT(WF_180_NOTES))){
        D_PRINTF("    FLAG: 180 NOTE DOOR OPENED\n");
        *hard_notes |=   BIT(WF_180_NOTES) | BIT(WF_50_NOTES);
    }
    else if(this->note_count >= 50 && !(*hard_notes & BIT(WF_50_NOTES))){
        D_PRINTF("    FLAG: 50 NOTE DOOR OPENED\n");
        *hard_notes |=   BIT(WF_50_NOTES);
    }

    //MOVES
    u8* hard_moves =  &(this->hard_f.moves);
    for(int i = 0; i < 8; i++){
        if(!((*hard_moves) & BIT(i))){
            if(bitfield_get(this->moves, i+1))
                *hard_moves |= BIT(i);
        }
    }
    

    //SOFT FLAGS
        //MOVES
    this->soft_f.moves = *hard_moves;
        //TRANS
    u8* soft_trans =  &(this->soft_f.transform);
    for(int i = 0; i < 5; i++){
        if(!((*soft_trans) & BIT(i))){
            if(bitfield_get(this->tranformations, i+2))
                *soft_trans |= BIT(i);
        }
    }

    //MISC
    u8* soft_misc =  &(this->soft_f.misc);
    for(int i = 3; i < 6; i++){
        if(!((*soft_misc) & BIT(i))){
            switch (i)
            {
            case WF_GV_WATER:
                if(bitfield_get(this->maps, GV_WATER)){
                    *soft_misc |= BIT(i);
                }
                break;
            case WF_WATER_LVL2:
                if(bitfield_get(this->maps, LAIR_CRYPT)){
                    if(bitfield_get(this->moves, MOVE_BEAK_BUST))
                        *soft_misc |= BIT(i);
                }
                break;
            case WF_WATER_LVL3:
                if(*soft_misc & BIT(WF_WATER_LVL2)){
                    if(bitfield_get(this->maps, LAIR_RBB_LOBBY)){
                        *soft_misc |= BIT(i);
                    }
                }
                break;
            default:
                break;
            }
        }
    }
}

static void _tree_update_jiggies(tree_t* this){
    for(u32 i =0; i< JIG_MAX_INDEX; i++){
        u32 ijig = i+1;
        if(!bitfield_get(this->jiggies, ijig)){
            switch (ijig)
            {
            case JIG_MM_JINJO:
            case JIG_MM_TICKER:
            case JIG_MM_MUMBO_SKULL:
            case JIG_MM_RUINS:
            case JIG_MM_HILL:
            case JIG_MM_ORANGE_PADS:
            case JIG_MM_CHIMPY:
                if(bitfield_get(this->maps, MM_MM)){
                    if(bitfield_get(this->moves, MOVE_TALON_TROT))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_MM_CONGA:
            case JIG_MM_JUJU:
                if(bitfield_get(this->maps, MM_MM)){
                    if(bitfield_get(this->moves, MOVE_EGGS))
                        bitfield_set(this->jiggies, ijig, true);  
                }
                break;
            case JIG_MM_HUTS:
                if(bitfield_get(this->maps, MM_MM)){
                    if(bitfield_get(this->moves, MOVE_BEAK_BUST))
                        bitfield_set(this->jiggies, ijig, true);  
                }
                break;
            case JIG_TTC_JINJO:
            case JIG_TTC_LIGHTHOUSE:
            case JIG_TTC_LOCKUP:
                if(bitfield_get(this->maps, TTC_TTC)){
                    if(bitfield_get(this->moves, MOVE_FLIGHT))
                        bitfield_set(this->jiggies, ijig, true);  
                }
                break;
            case JIG_TTC_ALCOVE_1:
            case JIG_TTC_POOL:
                if(bitfield_get(this->maps, TTC_TTC)){
                    bitfield_set(this->jiggies, ijig, true);  
                }
                break;
            case JIG_TTC_ALCOVE_2:
                if(bitfield_get(this->maps, TTC_TTC)){
                    if(bitfield_get(this->moves, MOVE_FLIGHT)
                    || bitfield_get(this->moves, MOVE_SHOCK_SPRING))
                        bitfield_set(this->jiggies, ijig, true);  
                }
                break;
            case JIG_TTC_SANDCASTLE:
                if(bitfield_get(this->maps, TTC_TTC)
                && bitfield_get(this->maps, TTC_SANDCASTLE)){
                    if(bitfield_get(this->moves, MOVE_BEAK_BUST)
                      && bitfield_get(this->moves, MOVE_EGGS)
                      )
                        bitfield_set(this->jiggies, ijig, true);  
                }
                break;
            case JIG_TTC_RED_X:
                if(bitfield_get(this->maps, TTC_TTC)){
                    if( bitfield_get(this->moves, MOVE_BEAK_BUST)
                    &&  ( bitfield_get(this->moves, MOVE_FLIGHT)
                        || bitfield_get(this->moves, MOVE_SHOCK_SPRING)))
                        bitfield_set(this->jiggies, ijig, true);  
                }
                break;
            case JIG_TTC_NIPPER:
                if(bitfield_get(this->maps, TTC_NIPPER)){
                    bitfield_set(this->jiggies, ijig, true);  
                }
                break;
            case JIG_TTC_BLUBBER:
                if(bitfield_get(this->maps, TTC_TTC)
                && bitfield_get(this->maps, TTC_SHIP_TOP)
                && bitfield_get(this->maps, TTC_SHIP_BOTTOM))
                    bitfield_set(this->jiggies, ijig, true); 
                break;
            case JIG_CC_JINJO:
                break;
            case JIG_CC_SNIPPETS:
            case JIG_CC_BACK:
            case JIG_CC_BOLT:
            case JIG_CC_TAIL:
            case JIG_CC_LONG_PIPE:
                if(bitfield_get(this->maps, CC_CC)){
                    bitfield_set(this->jiggies, ijig, true);  
                }
                break;
            case JIG_CC_TOOTH:
                if(bitfield_get(this->maps, CC_CC)
                && bitfield_get(this->maps, CC_INSIDE)){
                    if(bitfield_get(this->moves, MOVE_EGGS))
                        bitfield_set(this->jiggies, ijig, true);  
                }
                break;
            case JIG_CC_RINGS:
                if(bitfield_get(this->maps, CC_INSIDE)){
                    bitfield_set(this->jiggies, ijig, true);  
                }
                break;
            case JIG_CC_SLOW_SAWS:
                if(bitfield_get(this->maps,CC_WITCH_SWITCH)){
                    bitfield_set(this->jiggies, ijig, true);  
                }
                break;
            case JIG_CC_FAST_SAWS:
                if(bitfield_get(this->maps,CC_WONDERWING)){
                    bitfield_set(this->jiggies, ijig, true);  
                }
                break;
            case JIG_BGS_JINJO:
            case JIG_BGS_FLIBBITS:
                if(bitfield_get(this->maps,BGS_BGS)){
                    bitfield_set(this->jiggies, ijig, true);  
                }
                break;
            case JIG_BGS_WALKWAY:
            case JIG_BGS_EGG:
            case JIG_BGS_TANKTUP:
                if(bitfield_get(this->maps,BGS_BGS)){
                     if(bitfield_get(this->moves, MOVE_BEAK_BUST))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_BGS_CROCTUS:
                if(bitfield_get(this->maps,BGS_BGS)){
                     if(bitfield_get(this->moves, MOVE_EGGS)
                     && bitfield_get(this->moves, MOVE_SHOCK_SPRING))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_BGS_MUD_HUTS:
                if(bitfield_get(this->maps,BGS_BGS)){
                     if(bitfield_get(this->moves, MOVE_BEAK_BUST)
                     && bitfield_get(this->moves, MOVE_SHOCK_SPRING))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_BGS_MAZE:
                if(bitfield_get(this->maps,BGS_BGS)){
                     if(bitfield_get(this->moves, MOVE_BEAK_BUST)
                     && bitfield_get(this->moves, MOVE_WADING_BOOTS))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_BGS_TIPTUP:
                if(bitfield_get(this->maps,BGS_TANKTUP)){
                     if(bitfield_get(this->moves, MOVE_BEAK_BUST))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_BGS_VILE:
                if(bitfield_get(this->maps,BGS_VILE)){
                     if(bitfield_get(this->tranformations, TRANS_CROC))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_FP_JINJO:
                if(bitfield_get(this->maps,FP_FP)
                && bitfield_get(this->maps,FP_WOZZA_CAVE)
                && bitfield_get(this->maps,FP_MUMBOS)){
                    bitfield_set(this->jiggies, ijig, true);  
                }
                break;
            case JIG_FP_BOGGY_1:
                if(bitfield_get(this->maps,FP_FP)){
                     if(bitfield_get(this->moves, MOVE_BEAK_BUST))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_FP_PIPE:
                if(bitfield_get(this->maps,FP_FP)){
                    if(bitfield_get(this->moves, MOVE_FLIGHT)
                    || bitfield_get(this->moves, MOVE_SHOCK_SPRING))
                    bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_FP_BOGGY_3:
                if(bitfield_get(this->jiggies, JIG_FP_BOGGY_2)){
                    if(bitfield_get(this->moves, MOVE_TURBO_TRAINERS))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_FP_SNOWMAN_BUTTONS:
            case JIG_FP_SIR_SLUSH:
                if(bitfield_get(this->maps,FP_FP)){
                    if(bitfield_get(this->moves, MOVE_FLIGHT)
                    && bitfield_get(this->moves, MOVE_BEAK_BOMB))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_FP_PRESENTS:
                if(bitfield_get(this->maps,FP_FP)
                && bitfield_get(this->maps,FP_XMAS_TREE)){
                    if(bitfield_get(this->moves, MOVE_FLIGHT)
                    || bitfield_get(this->moves, MOVE_SHOCK_SPRING))
                    bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_FP_XMAS_TREE:
                if(bitfield_get(this->maps,FP_FP)
                && bitfield_get(this->maps,FP_XMAS_TREE)){
                    if(bitfield_get(this->moves, MOVE_FLIGHT)
                    && bitfield_get(this->moves, MOVE_BEAK_BUST)
                    && bitfield_get(this->moves, MOVE_EGGS))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_FP_BOGGY_2:
                if(bitfield_get(this->jiggies, JIG_FP_BOGGY_1)){
                    if(bitfield_get(this->tranformations, TRANS_WALRUS))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_FP_WOZZA:
                if(bitfield_get(this->maps,FP_FP)){
                    if(bitfield_get(this->tranformations, TRANS_WALRUS))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_LAIR_1ST:
                if(bitfield_get(this->maps,LAIR_MM_LOBBY)){
                    bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_LAIR_MM_WS:
                if(bitfield_get(this->maps,LAIR_MM_LOBBY)
                && bitfield_get(this->maps,MM_MM)){
                    if(bitfield_get(this->moves, MOVE_BEAK_BUST)){
                        if(bitfield_get(this->tranformations, TRANS_TERMITE)
                        || bitfield_get(this->tranformations, TRANS_CROC)
                        || bitfield_get(this->tranformations, TRANS_WALRUS)
                        || bitfield_get(this->tranformations, TRANS_PUMPKIN)
                        || bitfield_get(this->tranformations, TRANS_BEE))
                            bitfield_set(this->jiggies, ijig, true);
                    }
                }
                break;
            case JIG_LAIR_CC_WS:
                if(bitfield_get(this->maps,LAIR_TTC_CC_PUZ)
                && bitfield_get(this->maps,CC_WITCH_SWITCH)){
                    if(bitfield_get(this->moves, MOVE_BEAK_BUST))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_LAIR_TTC_WS:
                if(bitfield_get(this->maps,LAIR_TTC_LOBBY)
                && bitfield_get(this->maps,TTC_TTC)){
                    if(bitfield_get(this->moves, MOVE_BEAK_BUST)
                    && bitfield_get(this->moves, MOVE_FLIGHT))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_LAIR_BGS_WS:
                if(bitfield_get(this->maps,LAIR_260_ND)
                && bitfield_get(this->maps,BGS_BGS)){
                    if(bitfield_get(this->moves, MOVE_BEAK_BUST)
                    && bitfield_get(this->moves, MOVE_SHOCK_SPRING))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_LAIR_FP_WS:
                if(bitfield_get(this->maps,LAIR_FP_LOBBY)
                && bitfield_get(this->maps,FP_FP)){
                    if(bitfield_get(this->moves, MOVE_BEAK_BUST)
                    && bitfield_get(this->moves, MOVE_FLIGHT)
                    && bitfield_get(this->moves, MOVE_TURBO_TRAINERS)
                    && bitfield_get(this->moves, MOVE_BEAK_BOMB))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_LAIR_MMM_WS:
                if(bitfield_get(this->maps,LAIR_FP_LOBBY)
                && bitfield_get(this->maps,MMM_CHURCH)){
                    if(bitfield_get(this->moves, MOVE_BEAK_BUST)
                    && bitfield_get(this->moves, MOVE_FLIGHT)
                    && bitfield_get(this->moves, MOVE_TURBO_TRAINERS)
                    && bitfield_get(this->moves, MOVE_BEAK_BOMB))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_LAIR_GV_WS:
                if(bitfield_get(this->maps,LAIR_GV_LOBBY)
                && bitfield_get(this->maps,GV_SANDYBUTT)){
                    if(bitfield_get(this->moves, MOVE_BEAK_BUST)
                    && bitfield_get(this->moves, MOVE_SHOCK_SPRING))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_LAIR_RBB_WS:
                if(bitfield_get(this->maps,LAIR_640_ND)
                && bitfield_get(this->maps,RBB_RBB)
                && bitfield_get(this->maps,LAIR_CRYPT)){
                    if(bitfield_get(this->moves, MOVE_BEAK_BUST)
                    && bitfield_get(this->moves, MOVE_SHOCK_SPRING))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_LAIR_CCW_WS:
                if(bitfield_get(this->maps,LAIR_CCW_LOBBY)
                && bitfield_get(this->maps,CCW_WINTER)){
                    if(bitfield_get(this->moves, MOVE_BEAK_BUST)
                    && (bitfield_get(this->moves, MOVE_TALON_TROT)
                       || bitfield_get(this->moves, MOVE_FLIGHT))){
                        if(bitfield_get(this->tranformations, TRANS_BEE))
                            bitfield_set(this->jiggies, ijig, true);
                    }
                }
                break;
            case JIG_GV_JINJO:
                if(bitfield_get(this->maps,GV_GV)
                && bitfield_get(this->maps,GV_JINXY)
                && bitfield_get(this->maps,GV_SANDYBUTT)){
                    if(bitfield_get(this->moves, MOVE_EGGS))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_GV_GRABBA:
                if(bitfield_get(this->maps,GV_GV)){
                    if(bitfield_get(this->moves, MOVE_TURBO_TRAINERS))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_GV_JINXY:
                if(bitfield_get(this->maps,GV_JINXY)){
                    if(bitfield_get(this->moves, MOVE_EGGS))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_GV_MATCHING:
                if(bitfield_get(this->maps,GV_MATCHING)){
                    if(bitfield_get(this->moves, MOVE_BEAK_BUST))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_GV_SANDYBUTT:
                if(bitfield_get(this->maps,GV_GV)
                && bitfield_get(this->maps,GV_SANDYBUTT)){
                    if(bitfield_get(this->moves, MOVE_EGGS))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_GV_WATER:
                if(bitfield_get(this->maps,GV_WATER))
                    bitfield_set(this->jiggies, ijig, true);
                break;
            case JIG_GV_HISTUP:
                if(bitfield_get(this->maps,GV_RUBEE)){
                    if(bitfield_get(this->moves, MOVE_EGGS))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_GV_GOBI_1:
                if(bitfield_get(this->maps,GV_RUBEE)){
                    if(bitfield_get(this->moves, MOVE_EGGS))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_GV_GOBI_2:
                if(bitfield_get(this->maps,GV_GV)){
                    if(bitfield_get(this->moves, MOVE_BEAK_BUST))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_GV_ANCIENT_ONES:
                if(bitfield_get(this->maps,GV_GV)){
                    if(bitfield_get(this->moves, MOVE_FLIGHT))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_CCW_JINJO:
                if(bitfield_get(this->maps,CCW_SPRING)
                && bitfield_get(this->maps,CCW_SUMMER)
                && bitfield_get(this->maps,CCW_AUTUMN)
                && bitfield_get(this->maps,CCW_WINTER)
                && bitfield_get(this->maps,CCW_SP_ZUBBAS)){
                    if(bitfield_get(this->moves, MOVE_TALON_TROT)
                    || (bitfield_get(this->moves, MOVE_FLIGHT)
                       && bitfield_get(this->moves, MOVE_SHOCK_SPRING))){
                        if(bitfield_get(this->tranformations,TRANS_BEE))
                            bitfield_set(this->jiggies, ijig, true);
                    }
                }
                break;
            case JIG_CCW_HOUSE:
                if(bitfield_get(this->maps,CCW_SUMMER)
                || bitfield_get(this->maps,CCW_AUTUMN)){
                    if(bitfield_get(this->moves, MOVE_TALON_TROT)
                    || bitfield_get(this->moves, MOVE_SHOCK_SPRING))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_CCW_Eyrie:
                if(bitfield_get(this->maps,CCW_SPRING)
                && bitfield_get(this->maps,CCW_SUMMER)
                && bitfield_get(this->maps,CCW_AUTUMN)
                && bitfield_get(this->maps,CCW_WINTER)){
                    if((bitfield_get(this->moves, MOVE_TALON_TROT)
                       || (  bitfield_get(this->moves, MOVE_SHOCK_SPRING)
                          && bitfield_get(this->moves, MOVE_FLIGHT)))
                    && bitfield_get(this->moves, MOVE_BEAK_BUST))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_CCW_NABNUT:
                if(bitfield_get(this->maps,CCW_AUTUMN)
                && bitfield_get(this->maps,CCW_AU_NABNUTS)
                && bitfield_get(this->maps,CCW_AU_FLOODED_ROOM)){
                    if( bitfield_get(this->moves, MOVE_TALON_TROT)
                    || bitfield_get(this->moves, MOVE_SHOCK_SPRING)){
                        bitfield_set(this->jiggies, ijig, true);
                    }
                }
                break;
            case JIG_CCW_GNAWTY:
                if(bitfield_get(this->maps,CCW_SUMMER)
                && ( bitfield_get(this->maps,CCW_AUTUMN)
                   ||bitfield_get(this->maps,CCW_WINTER))){
                    if( bitfield_get(this->moves, MOVE_BEAK_BUST)){
                        bitfield_set(this->jiggies, ijig, true);
                    }
                }
                break;
            case JIG_CCW_ZUBBAS:
                if(bitfield_get(this->maps,CCW_SU_ZUBBAS)){
                    bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_CCW_FLOWER:
                if(bitfield_get(this->maps,CCW_SPRING)
                && bitfield_get(this->maps,CCW_SUMMER)
                && bitfield_get(this->maps,CCW_AUTUMN)
                && bitfield_get(this->maps,CCW_WINTER)){
                    if(bitfield_get(this->moves, MOVE_BEAK_BUST)
                    && bitfield_get(this->moves, MOVE_EGGS)){
                        bitfield_set(this->jiggies, ijig, true);
                    }
                }
                break;
            case JIG_CCW_SUM_LEAVES:
                if(bitfield_get(this->maps,CCW_SUMMER)){
                    if(bitfield_get(this->moves, MOVE_TALON_TROT)
                    || bitfield_get(this->moves, MOVE_SHOCK_SPRING)){
                        bitfield_set(this->jiggies, ijig, true);
                    }
                }
                break;
            case JIG_CCW_TREE_TOP:
                if( (bitfield_get(this->maps,CCW_SPRING) 
                    && bitfield_get(this->tranformations, TRANS_BEE))
                || (bitfield_get(this->maps,CCW_WINTER) 
                    && bitfield_get(this->moves, MOVE_FLIGHT))){
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_CCW_TOP_ROOM:
                if(bitfield_get(this->maps,CCW_SPRING)
                || bitfield_get(this->maps,CCW_SUMMER)
                || bitfield_get(this->maps,CCW_AUTUMN)
                || bitfield_get(this->maps,CCW_WINTER)){
                    if( bitfield_get(this->moves, MOVE_TALON_TROT)
                    || bitfield_get(this->moves, MOVE_SHOCK_SPRING)
                    || bitfield_get(this->moves, MOVE_FLIGHT))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_RBB_JINJO:
                if(bitfield_get(this->maps,RBB_RBB)
                && bitfield_get(this->maps,RBB_M_CONTAINER)){
                    if(bitfield_get(this->moves, MOVE_EGGS))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_RBB_WAREHOUSE:
                if(bitfield_get(this->maps,RBB_WAREHOUSE))
                    bitfield_set(this->jiggies, ijig, true);
                break;
            case JIG_RBB_SNORKEL:
                if(bitfield_get(this->maps,RBB_RBB)
                && bitfield_get(this->maps,RBB_ANCHOR)){
                    if(bitfield_get(this->moves, MOVE_BEAK_BUST))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_RBB_WHISTLE:
                if(bitfield_get(this->maps,RBB_RBB)){
                    if(bitfield_get(this->moves, MOVE_BEAK_BUST))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_RBB_FUNNEL:
                if(bitfield_get(this->maps,RBB_RBB))
                    bitfield_set(this->jiggies, ijig, true);
                break;
            case JIG_RBB_BOSS_BOOMBOX:
                if(bitfield_get(this->maps,RBB_BOSS_BOOMBOX))
                    bitfield_set(this->jiggies, ijig, true);
                break;
            case JIG_RBB_PROPELLOR:
                if(bitfield_get(this->maps,RBB_RBB)
                && bitfield_get(this->maps,RBB_ENGINE_BUTTON)
                && bitfield_get(this->maps,RBB_ENGINE_GEARS))
                    if(bitfield_get(this->moves,MOVE_BEAK_BUST))
                        bitfield_set(this->jiggies, ijig, true);
                break;
            case JIG_RBB_CPT_QRTRS:
                if(bitfield_get(this->maps,RBB_CAPT_QRTRS))
                    bitfield_set(this->jiggies, ijig, true);
                break;
            case JIG_RBB_CRANE:
                if(bitfield_get(this->maps,RBB_RBB))
                    if(bitfield_get(this->moves,MOVE_EGGS)
                    && bitfield_get(this->moves,MOVE_SHOCK_SPRING)){
                        bitfield_set(this->jiggies, ijig, true);
                    }
                break;
            case JIG_RBB_ENGINE_ROOM:
                if(bitfield_get(this->maps,RBB_ENGINE_BUTTON)
                && bitfield_get(this->maps,RBB_ENGINE_GEARS))
                    if(bitfield_get(this->moves,MOVE_BEAK_BUST))
                        bitfield_set(this->jiggies, ijig, true);
                break;
            case JIG_MMM_JINJO:
                if(bitfield_get(this->maps,MMM_MMM)
                && bitfield_get(this->maps,MMM_BEDROOM)
                && bitfield_get(this->maps,MMM_CELLAR))
                    if(bitfield_get(this->moves,MOVE_SHOCK_SPRING))
                        bitfield_set(this->jiggies, ijig, true);
                break;
            case JIG_MMM_WELL:
                if(bitfield_get(this->maps,MMM_WELL))
                    bitfield_set(this->jiggies, ijig, true);
                break;
            case JIG_MMM_NAPPER:
                if(bitfield_get(this->maps,MMM_DINING_ROOM))
                    bitfield_set(this->jiggies, ijig, true);
                break;
            case JIG_MMM_CELLAR:
                if(bitfield_get(this->maps,MMM_CELLAR))
                    bitfield_set(this->jiggies, ijig, true);
                break;
            case JIG_MMM_CHURCH_ROOF:
                if(bitfield_get(this->maps,MMM_CELLAR)){
                    if(bitfield_get(this->moves,MOVE_TALON_TROT))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_MMM_MOTZHAND:
                if(bitfield_get(this->maps,MMM_CHURCH)){
                    if(bitfield_get(this->moves,MOVE_BEAK_BUST)
                    && bitfield_get(this->moves,MOVE_SHOCK_SPRING))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_MMM_RAIN_BARREL:
                if(bitfield_get(this->maps,MMM_RAIN_BARREL)){
                    bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_MMM_TUMBLAR:
                if(bitfield_get(this->maps,MMM_TUMBLAR)){
                    bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_MMM_FLOWER_POTS:
                if(bitfield_get(this->maps,MMM_MMM)){
                    if(bitfield_get(this->moves,MOVE_EGGS))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            case JIG_MMM_LOGGO:
                if(bitfield_get(this->maps,MMM_LOGGO)){
                    if(bitfield_get(this->moves,MOVE_EGGS))
                        bitfield_set(this->jiggies, ijig, true);
                }
                break;
            default:
                break;
            }
        }
    }
    this->jiggy_count = bitfield_get_count(this->jiggies);
}

static void _tree_update(tree_t* this, exit_t* warp){
    _tree_update_maps(this, warp);
    _tree_update_moves(this);
    _tree_update_notes(this);
    _tree_update_tokens(this, warp);
    _tree_update_transformations(this);
    _tree_update_jiggies(this);
    _tree_update_warp_flags(this);
}

tree_t* tree_init(exit_t* root_info, warp_pool_t* avail_list, warp_pool_t* unaviail_list){
    tree_t* this = (tree_t*) calloc(1,sizeof(tree_t));
    this->jiggies = bitfield_new(JIG_MAX_INDEX);
    this->maps = bitfield_new(MAP_MAX_INDEX);
    this->moves = bitfield_new(MOVES_MAX_INDEX);
    this->tranformations = bitfield_new(6);
    this->available_exits = avail_list;
    this->unavailable_exits = avail_list;
    
    this->nodes = (tree_node_t *)malloc(sizeof(tree_node_t)*256);
    
    this->root = root_info->map;
    this->nodes[root_info->map];
    this->nodes[this->root].id = *root_info;
    
    _tree_update(this, root_info);

    return this;
}

void tree_attach_exit(tree_t* this, exit_t* parent, exit_t* new_exit){
    u8 iparent = _warp_get_map_index(parent);
    u8 inew = _warp_get_map_index(new_exit);
    this->nodes[inew].id = *new_exit;
    this->nodes[inew].parent = iparent;
    u8* new_depth = &this->nodes[inew].depth;
    *new_depth = this->nodes[iparent].depth + 1;
    if(this->depth < *new_depth)
        this->depth = *new_depth;
    _tree_update(this, new_exit);
}

void tree_free(tree_t* this){
        bitfield_free(this->jiggies);
        bitfield_free(this->maps);
        bitfield_free(this->moves);
        bitfield_free(this->tranformations);
        //TODO: free tree nodes
    return;
}

u8 tree_get_exit_depth(tree_t* this, exit_t* exit){
    return this->nodes[_warp_get_map_index(exit)].depth;
}

u8 _tree_find_children(tree_t* this, u8* child_list, u8 parent){
    u8 child_count = 0; 
    for (u16 i = 0; i < 256; i++)
    {
        tree_node_t* curr_node = &(this->nodes[i]);
        if(curr_node->parent == parent){
            child_list[child_count++] = _warp_get_map_index(&curr_node->id); 
        }
    }
    return child_count;
}

static void _tree_print_node(tree_t* this, tree_node_t* node, char* indent, bool last){
    char elbow[3];
    char c_indent[512];
    strcpy(c_indent,indent);
    if(last){
        strcpy(elbow,"'-");
        strcat(c_indent,"  ");
    } else {
        strcpy(elbow,"+-");
        strcat(c_indent,"| ");
    }
    //printf("%s%s0x%02x\n",indent, elbow, node->id.map);
    u8 children[32];
    u8 child_cnt = _tree_find_children(this, children, _warp_get_map_index(&node->id));
    printf("%s%s0x%02x: %d\n",indent, elbow, node->id.map, child_cnt);
    if(child_cnt == 0){
        return;
    }
    for (u8 i = 0; i < child_cnt; i++)
    {
        _tree_print_node(this, &(this->nodes[children[i]]), c_indent, i == child_cnt-1);
    }
}
void tree_print(tree_t* this){
    #ifdef DEBUG
    printf("  0x%02x\n",this->nodes[this->root].id.map);
    u8 children[32];
    u8 child_cnt = _tree_find_children(this, children, this->root);
    if(child_cnt == 0)
        return;   
    for (u8 i = 0; i < child_cnt; i++)
    {
        _tree_print_node(this, &(this->nodes[children[i]]), "  ", i == child_cnt-1);
    }
    #endif
    return;
}