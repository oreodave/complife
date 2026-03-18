/* simulation.c: Simulation implementation
 * Created: 2026-03-17
 * Author: Aryadev Chavali
 * License: See end of file
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <raylib.h>

#include "simulation.h"

static char *VALID_OPS = "<>{}-+.,[]";

static const Color possible_colors[] = {
    ['<'] = {0x2f, 0x4f, 0x4f, 0xff}, ['>'] = {0x22, 0x8b, 0x22, 0xff},
    ['{'] = {0xb0, 0x30, 0x60, 0xff}, ['}'] = {0xff, 0x00, 0x00, 0xff},
    ['-'] = {0xff, 0xff, 0x00, 0xff}, ['+'] = {0x00, 0xff, 0x00, 0xff},
    ['.'] = {0x00, 0xff, 0xff, 0xff}, [','] = {0xa0, 0x20, 0xf0, 0xff},
    ['['] = {0x1e, 0x90, 0xff, 0xff}, [']'] = {0xff, 0xde, 0xad, 0xff},
    ['\0'] = {0x0, 0x0, 0x0, 0xff},
};

void simulation_iterate(simulation_t *sim)
{
  sim->a = 0;
  sim->b = 0;
  while (sim->a == sim->b)
  {
    sim->a = rand() % NUM_PROGRAMS;
    sim->b = rand() % NUM_PROGRAMS;
  }

  // Perform the catalytic reaction
  struct ProgramConcat a_b_concat = {0};
  program_concat(&a_b_concat, sim->memory + (sim->a * SIZEOF_PROGRAM),
                 sim->memory + (sim->b * SIZEOF_PROGRAM));
  program_execute(&a_b_concat);
  program_split(&a_b_concat);
}

bf_token get_op(const bf_token cell)
{
  if (strchr(VALID_OPS, cell))
    return cell;
  else
    return '\0';
}

Color simulation_cell_color(const bf_token cell)
{
  return possible_colors[get_op(cell)];
}

void simulation_draw(simulation_t *sim)
{
  auto const PROGRAM_ROW    = (1 << (SIZEOF_PROGRAM_POW_2 / 2));
  auto const SIMULATION_ROW = (1 << (NUM_PROGRAMS_POW_2 / 2));
  auto const CELL_WIDTH     = WIDTH / ((f64)(PROGRAM_ROW * SIMULATION_ROW));
  auto const CELL_HEIGHT    = HEIGHT / ((f64)(PROGRAM_ROW * SIMULATION_ROW));

  for (size_t i = 0; i < NUM_PROGRAMS; ++i)
  {
    auto s_x = (i / SIMULATION_ROW) * PROGRAM_ROW;
    auto s_y = (i % SIMULATION_ROW) * PROGRAM_ROW;
    // printf("%lu => (%lu, %lu)\n", i, s_x, s_y);

    const bf_token *base = sim->memory + (i * SIZEOF_PROGRAM);

    for (u64 j = 0; j < SIZEOF_PROGRAM; ++j)
    {
      auto p_x = j / PROGRAM_ROW;
      auto p_y = j % PROGRAM_ROW;
      p_x += s_x;
      p_y += s_y;

      // printf("\t%lu => (%lu, %lu)\n", j, p_x, p_y);

      auto color = simulation_cell_color(base[j]);

      DrawRectangleV((Vector2){CELL_WIDTH * p_x, CELL_HEIGHT * p_y},
                     (Vector2){CELL_WIDTH, CELL_HEIGHT}, color);
    }

    if (sim->a == i || sim->b == i)
    {
      DrawRectangleLinesEx((Rectangle){.x      = s_x * CELL_WIDTH,
                                       .y      = s_y * CELL_HEIGHT,
                                       .width  = PROGRAM_ROW * CELL_WIDTH,
                                       .height = PROGRAM_ROW * CELL_HEIGHT},
                           1, BLUE);
    }
  }
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
