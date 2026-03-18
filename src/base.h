/* base.h: Basic definitions
 * Created: 2026-03-10
 * Author: Aryadev Chavali
 * License: See end of file
 */

#ifndef BASE_H
#define BASE_H

#include <assert.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

static_assert(sizeof(float) == 4, "f32 requires 4 byte floats");
static_assert(sizeof(double) == 8, "f64 requires 8 byte doubles");
typedef float f32;
typedef double f64;

#define MAX(A, B)      ((A) > (B) ? (A) : (B))
#define MIN(A, B)      ((A) < (B) ? (A) : (B))
#define SAFE_SUB(A, B) ((A) < (B) ? 0 : (A) - (B))

// GUI stuff
#define WIDTH  800
#define HEIGHT 800

// Simulation constants
#define SIZEOF_PROGRAM_POW_2 8
#define SIZEOF_PROGRAM       (1 << SIZEOF_PROGRAM_POW_2)
#define NUM_PROGRAMS_POW_2   8
#define NUM_PROGRAMS         (1 << NUM_PROGRAMS_POW_2)

#define MUTATION_CHANCE 10000
#define MUTATION_OFFSET 3

#endif

/* Copyright (C) 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License Version 2 for
 * details.

 * You may distribute and modify this code under the terms of the GNU General
 * Public License Version 2, which you should have received a copy of along with
 * this program.  If not, please go to <https://www.gnu.org/licenses/>.

 */
