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
    ['<'] = {230, 25, 75, 255},  ['>'] = {60, 180, 75, 255},
    ['{'] = {255, 225, 25, 255}, ['}'] = {0, 130, 200, 255},
    ['-'] = {245, 130, 48, 255}, ['+'] = {145, 30, 180, 255},
    ['.'] = {70, 240, 240, 255}, [','] = {240, 50, 230, 255},
    ['['] = {210, 245, 60, 255}, [']'] = {250, 190, 212, 255},
    ['\0'] = {0, 0, 0, 255},
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
  // Strategy 1: Pick two random cells
  // while (a == b)
  // {
  //   a = rand() % NUM_PROGRAMS;
  //   b = rand() % NUM_PROGRAMS;
  // }

  // Strategy 2: Pick a random cell, then iterate on all neighbours
  a       = rand() % NUM_PROGRAMS;
  u64 a_x = a % SIMULATION_ROW_SIZE;
  u64 a_y = a / SIMULATION_ROW_SIZE;

  bool valid_dir[4] = {
      a_x > 0,
      a_x != (SIMULATION_ROW_SIZE - 1),
      a_y > 0,
      a_y < (SIMULATION_ROW_SIZE - 1),
  };

  u64 neighbours[8] = {0};
  u64 size          = 0;
  if (valid_dir[0])
  {
    neighbours[size++] = a - 1;
    if (valid_dir[2])
    {
      neighbours[size++] = a - SIMULATION_ROW_SIZE - 1;
    }
    if (valid_dir[3])
    {
      neighbours[size++] = a + SIMULATION_ROW_SIZE - 1;
    }
  }
  if (valid_dir[1])
  {
    neighbours[size++] = a + 1;
    if (valid_dir[2])
    {
      neighbours[size++] = a - SIMULATION_ROW_SIZE + 1;
    }
    if (valid_dir[3])
    {
      neighbours[size++] = a + SIMULATION_ROW_SIZE + 1;
    }
  }
  if (valid_dir[2])
  {
    neighbours[size++] = a - SIMULATION_ROW_SIZE;
  }
  if (valid_dir[3])
  {
    neighbours[size++] = a + SIMULATION_ROW_SIZE;
  }

  b = neighbours[rand() % size];

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
