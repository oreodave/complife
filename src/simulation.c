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

void simulation_mutate(simulation_t *sim)
{
#if MUTATION_CHANCE
  for (u64 i = 0; i < SIMULATION_SIZE; ++i)
  {
    if ((rand() % MUTATION_CHANCE) == (MUTATION_CHANCE - 1))
    {
      sim->memory[i] = rand() % 255;
    }
  }
#else
  (void)sim;
#endif
}

static struct ProgramConcat *a_b_concat = NULL;
void simulation_iterate(simulation_t *sim)
{
  u64 a = 0, b = 0;
  while (a == b)
  {
    a = rand() % NUM_PROGRAMS;
    b = rand() % NUM_PROGRAMS;
  }

  // Perform the reaction
  if (!a_b_concat)
  {
    a_b_concat = calloc(1, sizeof(*a_b_concat));
  }
  memset(a_b_concat, 0, sizeof(*a_b_concat));
  program_concat(a_b_concat, sim->memory + (a * SIZEOF_PROGRAM),
                 sim->memory + (b * SIZEOF_PROGRAM));
  program_execute(a_b_concat);
  program_split(a_b_concat);
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
  for (size_t i = 0; i < NUM_PROGRAMS; ++i)
  {
    const bf_token *base = sim->memory + (i * SIZEOF_PROGRAM);
    u64 s_x              = (i % SIMULATION_ROW_SIZE) * PROGRAM_ROW_SIZE;
    u64 s_y              = (i / SIMULATION_ROW_SIZE) * PROGRAM_ROW_SIZE;

    for (u64 j = 0; j < SIZEOF_PROGRAM; ++j)
    {
      u64 p_x = j % PROGRAM_ROW_SIZE;
      u64 p_y = j / PROGRAM_ROW_SIZE;
      p_x += s_x;
      p_y += s_y;

      Color color = simulation_cell_color(base[j]);

      // DrawRectangle(p_x * CELL_WIDTH, p_y * CELL_HEIGHT, CELL_WIDTH,
      //               CELL_HEIGHT, color);
      DrawRectangleV((Vector2){CELL_WIDTH * p_x, CELL_HEIGHT * p_y},
                     (Vector2){CELL_WIDTH, CELL_HEIGHT}, color);
    }

#if DRAW_PROGRAM_OUTLINE
    DrawRectangleLinesEx((Rectangle){.x      = s_x * CELL_WIDTH,
                                     .y      = s_y * CELL_HEIGHT,
                                     .width  = CELL_WIDTH * PROGRAM_ROW_SIZE,
                                     .height = CELL_HEIGHT * PROGRAM_ROW_SIZE},
                         CELL_WIDTH / PROGRAM_ROW_SIZE, DARKGRAY);
#endif
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
