/* program_iter.h: Program iteration procedure
 * Created: 2026-03-10
 * Author: Aryadev Chavali
 * License: See end of file
 */

#ifndef PROGRAM_ITER_H
#define PROGRAM_ITER_H

#include "base.h"
#include "sv.h"

// Result of concatenating two programs.  Maintains a concatenated tape of the
// two programs fed for ease of use.
struct ProgramConcat
{
  sv_t A, B;
  u8 tape[SIZEOF_PROGRAM * 2];
};

void program_concat(struct ProgramConcat *, sv_t, sv_t);
void program_execute(struct ProgramConcat *);
void program_split(struct ProgramConcat *);

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
