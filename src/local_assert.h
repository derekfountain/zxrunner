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

#ifndef __LOCAL_ASSERT_H
#define __LOCAL_ASSERT_H

#ifdef NDEBUG

#define local_assert(exp) ((void)0)

#else

void local_assert_bp(void);

/*
 * This is the local_assert(expression) macro to use in the code.
 * For no debug (-DNDEBUG) builds this boils away to nothing.
 */
#define local_assert(exp) if( !(exp) ) { local_assert_bp(); }

#endif

#endif
