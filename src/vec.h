/* vec.h: Vectors
 * Created: 2026-03-10
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 Taken from https://github.com/oreodave/prick - prick_vec.
 */

#ifndef VEC_H
#define VEC_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#define VEC_INLINE_CAPACITY 32
#define VEC_MULT            2

typedef struct
{
  uint64_t size, capacity;
  uint8_t not_inlined;
  union
  {
    void *ptr;
    alignas(max_align_t) uint8_t inlined[VEC_INLINE_CAPACITY];
  };
} vec_t;

static_assert(sizeof(vec_t) == 64, "Expected sizeof(vec_t) to be 64");

void vec_append(vec_t *vec, const void *const ptr, uint64_t size);
void vec_append_byte(vec_t *vec, uint8_t byte);
u8 *vec_data(vec_t *vec);
void vec_ensure_capacity(vec_t *vec, uint64_t capacity);
void vec_ensure_free(vec_t *vec, uint64_t size);
void vec_free(vec_t *vec);
void vec_clone(vec_t *v2, vec_t *v1);
void *vec_pop(vec_t *vec, size_t member_size);
u64 vec_find(vec_t *vec, void *ptr, size_t ptrsize);

#define VEC_GET(VEC, INDEX, TYPE) (((TYPE *)vec_data(VEC))[INDEX])

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
