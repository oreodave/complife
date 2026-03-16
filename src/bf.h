/* bf.h: Brainfuck implementation
 * Created: 2026-03-10
 * Author: Aryadev Chavali
 * License: See end of file
 */

#ifndef BF_H
#define BF_H

#include "base.h"
#include "sv.h"

typedef i8 bf_token;

// All "programs" are 64 bytes.
#define SIZEOF_PROGRAM 64

struct ProgramConcat
{
  bf_token *a, *b;
  bf_token tape[SIZEOF_PROGRAM * 2];
};

void program_concat(struct ProgramConcat *, bf_token *a, bf_token *b);
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
