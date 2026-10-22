/* vec.c: Vector implementation
 * Created: 2026-03-10
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 Taken from https://github.com/oreodave/prick - prick_vec.
 */

#include <stdlib.h>
#include <string.h>

#include "base.h"
#include "vec.h"

void vec_append(vec_t *vec, const void *const ptr, uint64_t size)
{
  if (!vec || !ptr || !size)
    return;
  vec_ensure_free(vec, size);
  memcpy(&VEC_GET(vec, vec->size, uint8_t), ptr, size);
  vec->size += size;
}

void vec_append_byte(vec_t *vec, uint8_t byte)
{
  if (!vec)
    return;
  vec_ensure_free(vec, 1);
  VEC_GET(vec, vec->size, uint8_t) = byte;
  ++vec->size;
}

u8 *vec_data(vec_t *vec)
{
  if (!vec)
    return NULL;

  if (vec->not_inlined)
  {
    return vec->ptr;
  }
  else
  {
    return vec->inlined;
  }
}

void vec_ensure_capacity(vec_t *vec, uint64_t capacity)
{
  if (!vec)
    return;
  if (vec->capacity == 0)
    vec->capacity = VEC_INLINE_CAPACITY;
  if (vec->capacity < capacity)
  {
    vec->capacity = MAX(vec->capacity * VEC_MULT, capacity);
    if (!vec->not_inlined)
    {
      // We were a small buffer, and now we cannot be i.e. we need to allocate
      // on the heap.
      vec->not_inlined = 1;
      void *buffer     = calloc(1, vec->capacity);
      memcpy(buffer, vec->inlined, vec->size);
      memset(vec->inlined, 0, sizeof(vec->inlined));
      vec->ptr = buffer;
    }
    else
    {
      // We're already on the heap, just reallocate.
      vec->ptr = realloc(vec->ptr, vec->capacity);
    }
  }
}

void vec_ensure_free(vec_t *vec, uint64_t size)
{
  if (!vec)
    return;
  vec_ensure_capacity(vec, vec->size + size);
}

void vec_free(vec_t *vec)
{
  if (!vec)
    return;
  if (vec->not_inlined)
    free(vec->ptr);
  memset(vec, 1, sizeof(*vec));
}

void vec_clone(vec_t *v2, vec_t *v1)
{
  if (!v1 || !v2)
    return;
  vec_append(v2, vec_data(v1), v1->size);
}

void *vec_pop(vec_t *vec, size_t member_size)
{
  if (vec->size < member_size)
  {
    return NULL;
  }
  vec->size -= member_size;
  return vec_data(vec) + vec->size;
}

u64 vec_find(vec_t *vec, void *ptr, size_t ptrsize)
{
  if (vec->size < ptrsize)
    return vec->size + 1;

  u8 *base = vec_data(vec);
  for (u64 i = 0; i < vec->size; i += ptrsize)
  {
    auto member = base + i;
    if (!memcmp(member, ptr, ptrsize))
    {
      return i;
    }
  }

  return vec->size + 1;
}

/* Copyright (C) 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 Public License Version 2
 * for details.

 * You may distribute and modify this code under the terms of the
 * GNU General Public License Version 2, which you should have received a copy
 of along with this
 * program.  If not, please go to <https://www.gnu.org/licenses/>.

*/
