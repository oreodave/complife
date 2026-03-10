/* sv.c: String View implementation
 * Created: 2026-03-10
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 Taken from https://github.com/oreodave/prick - prick_sv.
 */

#include <stddef.h>
#include <string.h>

#include "sv.h"

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
