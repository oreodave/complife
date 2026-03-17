/* simulation.c: Simulation implementation
 * Created: 2026-03-17
 * Author: Aryadev Chavali
 * License: See end of file
 */

#include <stdlib.h>
#include <string.h>

#include <raylib.h>

#include "simulation.h"

static char *VALID_OPS = "<>{}-+.,[]";

static const Color possible_colors[] = {
    {0xb4, 0x60, 0x5f, 0xff}, {0xe, 0x7, 0x7, 0xff},
    {0x2c, 0x10, 0x10, 0xff}, {0x28, 0x21, 0x21, 0xff},
    {0xbc, 0x71, 0x70, 0xff}, {0x4a, 0x31, 0x31, 0xff},
    {0xc2, 0x68, 0x67, 0xff}, {0x7e, 0x40, 0x3f, 0xff},
    {0x21, 0x9, 0x8, 0xff},   {0xcf, 0x35, 0x34, 0xff},
    {0x0, 0x0, 0x0, 0xff},    {0},
};

void simulation_pick(u64 *a, u64 *b)
{
  // Pick two indices that don't intersect
  while (*a == *b)
  {
    *a = rand() % NUM_PROGRAMS;
    *b = rand() % NUM_PROGRAMS;
  }
}

void simulation_iterate(simulation_t *sim)
{
  u64 a = 0, b = 0;
  simulation_pick(&a, &b);

  // Change of basis from program indices to bf_token indices.
  a *= SIZEOF_PROGRAM;
  b *= SIZEOF_PROGRAM;

  // Perform the catalytic reaction
  struct ProgramConcat a_b_concat = {0};
  program_concat(&a_b_concat, sim->memory + a, sim->memory + b);
  program_execute(&a_b_concat);
  program_split(&a_b_concat);
}

u8 get_op(const u8 cell)
{
  if (strchr(VALID_OPS, cell))
    return cell;
  else
    return '\0';
}

Color simulation_cell_color(const u8 cell)
{
  return possible_colors[get_op(cell)];
}

void simulation_draw(simulation_t *sim)
{
  assert(0 && "TODO: implement simulation_draw");
}

/* Copyright (C) 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License Version 2 for
 * details.

 * You may distribute and modify this code under the terms of the GNU General
 * Public License Version 2, which you should have received a copy of along with
 * this program.  If not, please go to <https://www.gnu.org/licenses/>.

 */
