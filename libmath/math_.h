/* MathLib: Pilot shared library of IEEE-754 double math functions
 *
 * This version of math.h is designed for inclusion by the GCC math
 * functions, not the calling application program.  Its purpose is to
 * allow proper compilation of the GCC source code with as few 
 * changes as possible.
 *
 * Copyright (C) 1997 Rick Huebner
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see file COPYING.LIB.  If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA
 *
 * Version 1.0, 15 August 1997, Rick Huebner
 */

// Typedefs used in math routines
typedef long int32_t;
typedef unsigned long u_int32_t;

// Prototypes for primary math functions, copied verbatim from GCC source
double __ieee754_log(double x);
double __ieee754_log10(double x);
double __floor(double x);
int    __isnan(double x);

// Prototypes for our "jump island" stubs to avoid 32KB pc-rel jsr limit
double jumpto__ieee754_sqrt(double x);
double jumpto__fabs(double x);