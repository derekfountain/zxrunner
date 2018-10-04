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

/*
 * The player's score for the level starts at a value defined in the level data.
 * Some levels are harder than others and will take longer, so levels starts at
 * different values. The level score reduces down to zero as time passes.
 *
 * When the player completes the level the remaining score is added onto the
 * total score.
 *
 * The level bonus starts at a value defined in the level data and is reduced
 * by a value defined in the level data each time the player uses a slowdown
 * block. This bit's a TODO.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <arch/zx/sp1.h>

#include "utils.h"
#include "scoring.h"

uint32_t total_score = 0;

uint16_t level_score = 5000;
uint16_t last_printed_level_score = 0;

uint16_t level_bonus = 350;
uint16_t last_printed_level_bonus = 0;

void set_level_score( uint16_t score )
{
  level_score = score;
}

uint16_t get_level_score( void )
{
  return level_score;
}

void decrement_level_score( uint16_t decrement )
{
  level_score -= decrement;
}

/***
 *      _____  _           _                _____
 *     |  __ \(_)         | |              / ____|
 *     | |  | |_ ___ _ __ | | __ _ _   _  | (___   ___ ___  _ __ ___ ___
 *     | |  | | / __| '_ \| |/ _` | | | |  \___ \ / __/ _ \| '__/ _ / __|
 *     | |__| | \__ | |_) | | (_| | |_| |  ____) | (_| (_) | | |  __\__ \
 *     |_____/|_|___| .__/|_|\__,_|\__, | |_____/ \___\___/|_|  \___|___/
 *                  | |             __/ |
 *                  |_|            |___/
 */

/*
 * This is defined in main.c. Just share it for now.
 */
extern struct sp1_Rect full_screen;

/*
 * Keep this global so the compiler can preallocate it
 */
struct sp1_pss scoring_print_control = { &full_screen, SP1_PSSFLAG_INVALIDATE,
                                         0, 0,
                                         0x00, 0x03,
                                         0,
                                         0 };
uint8_t scoring_print_string[29];

void show_scores( SCORE_SCREEN_DATA* score_screen_data )
{
  /*
   * Then add some sort of timer to reduce the score. Show the total score
   * between screens?
   */

  /* Only update the screen when one of the values changes. This update is expensive! */
  if( level_score != last_printed_level_score || level_bonus != last_printed_level_bonus )
    {
      /*
       * I spent an age trying to work around this sprintf(), but ultimately
       * it was the best approach:
       *
       * https://github.com/derekfountain/zxwonkyonekey/wiki/Adding-sprintf()
       */
      sprintf(scoring_print_string, "\x14%c\x16%c%cScore:%05u\x16%c%cBonus:%05u",
		score_screen_data->score_screen_attribute,
		score_screen_data->level_score_y,
		score_screen_data->level_score_x,
		level_score,
		score_screen_data->bonus_score_y,
		score_screen_data->bonus_score_x,
		level_bonus);

      sp1_PrintString(&scoring_print_control, (uint8_t*)scoring_print_string);

      last_printed_level_score = level_score;
      last_printed_level_bonus = level_bonus;
    }
  
}

void reset_cached_screen_scores( void )
{
  last_printed_level_score = 0;
  last_printed_level_bonus = 0;
}

void display_total_score( void )
{

}

