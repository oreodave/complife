/* prick_vec.h: A dynamically sized array with SBO.
 * Created: 2026-01-22
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:

 To utilise this library, please put:
    #define PRICK_VEC_IMPL
    #include "prick_vec.h"
 in one of your code units.

 This library defines another form of dynamically sized array as opposed to
 prick_darr.h.  This one is closer to the one classically implemented by most; a
 structure with some metadata and a pointer to the raw buffer.  This way,
 pointers to the dynamic array are stable (as the structure itself is never
 reallocated) and the array can still grow as required.

 We use a trick, called Small Buffer Optimisation (SBO), to inline elements
 directly into the structure when there are a small number of them (see
 PRICK_VEC_INLINE_CAPACITY).  This makes lookup _even faster_ (no derefence and
 possibility of the entire vector existing in the CPU cache) and allows us to
 avoid allocation for smaller use cases.  If the number of elements exceeds
 PRICK_VEC_INLINE_CAPACITY, we utilise the allocator.
 */

#ifndef PRICK_VEC_H
#define PRICK_VEC_H

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
void *vec_data(vec_t *vec);
void vec_ensure_capacity(vec_t *vec, uint64_t capacity);
void vec_ensure_free(vec_t *vec, uint64_t size);
void vec_free(vec_t *vec);
void vec_clone(vec_t *v2, vec_t *v1);

#define VEC_GET(VEC, INDEX, TYPE) (((TYPE *)vec_data(VEC))[INDEX])

#ifdef VEC_IMPL
#define MAX(A, B) ((A) > (B) ? (A) : (B))

#include <stdlib.h>
#include <string.h>

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

void *vec_data(vec_t *vec)
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

#undef MAX
#endif

#endif

/* Copyright (C) 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the Unlicense for details.

 * You may distribute and modify this code under the terms of the Unlicense,
 * which you should have received a copy of along with this program.  If not,
 * please go to <https://unlicense.org/>.

 */
