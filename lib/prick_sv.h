/* sv.h: String Views.
 * Created: 2026-03-01
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:

 To utilise this library, please put:
    #define SV_IMPL
    #include "sv.h"
 in one of your code units.

 This is a simple read-only string view library.  It defines some extremely
 common functions you'd expect for a string view library, excluding any that
 require allocation.
 */

#ifndef SV_H
#define SV_H

#include <stdint.h>

typedef struct
{
  uint64_t size;
  const char *data;
} sv_t;

#define SV(DATA, SIZE) ((sv_t){.data = (DATA), .size = (SIZE)})
#define SV_AUTO(DATA)  ((sv_t){.data = (void *)(DATA), .size = sizeof(DATA) - 1})

// Pretty printers
#define SV_FMT(SV) (int)(SV).size, (SV).data
#define PR_SV      "%.*s"

sv_t sv_chop_left(sv_t, uint64_t size);
sv_t sv_chop_right(sv_t, uint64_t size);
sv_t sv_truncate(sv_t, uint64_t newsize);
sv_t sv_substr(sv_t, uint64_t position, uint64_t size);

sv_t sv_till(sv_t, const char *reject);
sv_t sv_while(sv_t, const char *accept);

#ifdef SV_IMPL
#include <stddef.h>
#include <string.h>

sv_t sv_chop_left(sv_t sv, uint64_t size)
{
  if (sv.size <= size)
    return SV(NULL, 0);
  return SV(sv.data + size, sv.size - size);
}

sv_t sv_chop_right(sv_t sv, uint64_t size)
{
  if (sv.size <= size)
    return SV(NULL, 0);
  return SV(sv.data, sv.size - size);
}

sv_t sv_truncate(sv_t sv, uint64_t newsize)
{
  if (newsize > sv.size)
    return SV(NULL, 0);
  return SV(sv.data, newsize);
}

sv_t sv_substr(sv_t sv, uint64_t position, uint64_t size)
{
  sv_t result = sv_truncate(sv_chop_left(sv, position), size);
  return result;
}

sv_t sv_till(sv_t sv, const char *reject)
{
  if (sv.size == 0 || !sv.data)
    return SV(NULL, 0);

  uint64_t offset;
  for (offset = 0; offset < sv.size && strchr(reject, sv.data[offset]) == NULL;
       ++offset)
    continue;

  return sv_truncate(sv, offset);
}

sv_t sv_while(sv_t sv, const char *accept)
{
  if (sv.size == 0 || !sv.data)
    return SV(NULL, 0);

  uint64_t offset;
  for (offset = 0; offset < sv.size && strchr(accept, sv.data[offset]) != NULL;
       ++offset)
    continue;

  return sv_truncate(sv, offset);
}

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
