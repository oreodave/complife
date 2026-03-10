/* sv.h: String Views
 * Created: 2026-03-10
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 Taken from https://github.com/oreodave/prick - prick_sv.
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

#endif

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
