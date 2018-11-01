/*
 * Wonky One Key, a ZX Spectrum game featuring a single control key
 * Copyright (C) 2018 Derek Fountain
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <stdint.h>
#include <arch/zx.h>
#include <arch/zx/sp1.h>

#include "door.h"
#include "utils.h"

/* This is in the assembly language file */
extern uint8_t door_f1[];

/*
 * This is defined in main.c. Just share it for now.
 */
extern struct sp1_Rect full_screen;

/*
 * This is in the levels code. It can only be used after the levels code
 * has initialised it.
 */
extern struct sp1_pss level_print_control;

/*
 * The door sits closer to the viewer and is OR'ed into the display.
 * The runner is behind, LOAD'ed in so that's fast, the door is OR'ed
 * over the top.
 */
#define DOOR_PLANE    (uint8_t)(1)

static uint8_t key_sp1_string[9] = "\x16" "yx" "\x10" "i" "\x11" "p" "t";
void display_key( DOOR* door, uint8_t visible )
{
  /* Build and print the string to display the key */
  key_sp1_string[1] = door->collectable.y;
  key_sp1_string[2] = door->collectable.x;
  key_sp1_string[4] = door->key_ink;
  key_sp1_string[6] = door->key_paper;

  if( visible )
    key_sp1_string[7] = door->key_tile_num;
  else
    key_sp1_string[7] = 255; /* TODO Macro needed */

  sp1_PrintString(&level_print_control, key_sp1_string);
}

static uint8_t ink_param;
static void initialise_colour(unsigned int count, struct sp1_cs *c)
{
  (void)count;    /* Suppress compiler warning about unused parameter */

  c->attr_mask = SP1_AMASK_INK;
  c->attr      = ink_param;
}

void create_door( DOOR* door )
{
  display_key( door, TRUE );

  door->moving_direction = DOOR_STATIONARY;
  door->y_offset         = 0;

  /*
   * This code will need a timer, so I need to break that out and share it
   * with the teleporter code.
   *
   * The door needs a 60ms timer. Check github issue, I worked it out.
   */
  door->sprite = sp1_CreateSpr(SP1_DRAW_LOAD1LB, SP1_TYPE_1BYTE, 2, 0, DOOR_PLANE);
  sp1_AddColSpr(door->sprite, SP1_DRAW_LOAD1RB, SP1_TYPE_1BYTE, 0, DOOR_PLANE);

  /*
   * I use the _callee version specifically here because sp1_MoveSprPix()
   * is itself a macro and putting the DOOR_SCREEN_LOCATION macro
   * in the arguments breaks the preprocessor.
   */
  sp1_MoveSprPix_callee(door->sprite, &full_screen,
                        (void*)door_f1,
                        DOOR_SCREEN_LOCATION(door));

  /* Colour the cells the sprite occupies */
  ink_param = door->door_ink_colour;
  sp1_IterateSprChar(door->sprite, initialise_colour);
}

void destroy_door( DOOR* door )
{
  (void)door;
#if 0
  /* Move sprite offscreen before calling delete function */
  sp1_MoveSprPix(slowdown->sprite, &full_screen, (void*)0, 255, 255);
  sp1_DeleteSpr(slowdown->sprite);
#endif
}

/*
 * Invalidator, called at screen update time to ensure the door
 * sprite tiles are invalidated and hence redrawn
 */
static void invalidateDoorSprite(unsigned int count, struct sp1_update *u)
{
  (void)count;

  sp1_InvUpdateStruct(u);
}

void animate_door( DOOR* door )
{
  (void)door;

#if 0
  /*
   * Crude little state machine to pulse the slowdown pills.
   * I tried to do this with a table/data approach but it always
   * came out bigger and more complicated. :)
   */
  uint8_t* next_frame;

  if( slowdown->available == PILL_NOT_AVAILABLE )
  {
    /* Move it off screen so it disappears */
    next_frame = (uint8_t*)slowdown_pill_f1;
    sp1_MoveSprPix(slowdown->sprite, &full_screen, next_frame, 255, 255);
  }
  else
  {
    if( slowdown->expanding )
    {
      if( slowdown->frame == 0 )
      {
        slowdown->frame = 1;
        next_frame = (uint8_t*)slowdown_pill_f2;
      }
      else if( slowdown->frame == 1 )
      {
        slowdown->frame = 2;
        next_frame = (uint8_t*)slowdown_pill_f3;
      }
      else /* slowdown->frame == 2 */
      {
        slowdown->expanding = 0;
        next_frame = (uint8_t*)slowdown_pill_f3;
      }
    }
    else
    {
      if( slowdown->frame == 0 )
      {
        slowdown->expanding = 1;
        next_frame = (uint8_t*)slowdown_pill_f1;
      }
      else if( slowdown->frame == 1 )
      {
        slowdown->frame = 0;
        next_frame = (uint8_t*)slowdown_pill_f1;
      }
      else /* slowdown->frame == 2 */
      {
        slowdown->frame = 1;
        next_frame = (uint8_t*)slowdown_pill_f2;
      }
    }

    /* Set the correct graphic on the pill sprite */
    sp1_MoveSprPix_callee(slowdown->sprite, &full_screen,
                          next_frame,
                          SLOWDOWN_SCREEN_LOCATION(slowdown));
  }

  /* Finally, invalidate the pill sprite so it redraws */
  sp1_IterateUpdateSpr(slowdown->sprite, invalidatePillSprite);
#endif
}

void animate_door_key( DOOR* door )
{
  /* Call key display key using availabily as visibility */
  display_key( door, IS_COLLECTABLE_AVAILABLE( &(door->collectable) ) );
}

/*
 * Collectable collection handler, called when the the runner
 * collects the door key. Set the key unavailable and
 * update the screen accordingly, then start the timer to count
 * down the time the door is open.
 */
void door_key_collected(COLLECTABLE* collectable, void* data)
{
  DOOR* door = (DOOR*)data;
  (void)collectable;

  SET_COLLECTABLE_AVAILABLE(door->collectable,COLLECTABLE_NOT_AVAILABLE);

  /* Remove key from screen */
  animate_door_key( door );

  /* Open the door */
//  start_door_animation( DOOR_OPENING );

  START_COLLECTABLE_TIMER(door->collectable, door->open_secs);

  return;
}

/*
 * Door open time up handler, called by the collectable timer timeout
 * code when the user has collected the key and the door has been open
 * for the allotted period. Close the door and reinstate the key.
 */
uint8_t door_open_timeup(COLLECTABLE* collectable, void* data)
{
  DOOR* door = (DOOR*)data;
  (void)collectable;

  SET_COLLECTABLE_AVAILABLE(door->collectable,COLLECTABLE_AVAILABLE);

  /* Redraw the key */
  animate_door_key( door );

  /* Close the door */
//  start_door_animation( DOOR_CLOSING );

  return 0;
}

void door_passed_through( DOOR* door )
{
  /* Not sure if this is useful yet */
  SET_COLLECTABLE_AVAILABLE( door->collectable, COLLECTABLE_DISARMED );

  /*
   * Cancel the timer with the door open and the key off screen.
   * The door stays open.
   */
  CANCEL_COLLECTABLE_TIMER( door->collectable );
}

