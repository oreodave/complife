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

void simulation_mutate(simulation_t *sim)
{
#if MUTATION_CHANCE
  for (u64 i = 0; i < SIMULATION_SIZE; ++i)
  {
    // Has probability of 1 / MUTATION_CHANCE.
    if ((rand() % MUTATION_CHANCE) == (MUTATION_CHANCE - 1))
    {
      sim->memory[i] = rand() % 255;
    }
  }
#else
  (void)sim;
#endif
}

// Strategy 1: Pick two random cells
void simulation_pick_rng_pair(u64 *a, u64 *b);
// Strategy 2: Pick a random cell, then iterate on all neighbours
void simulation_pick_rng_neighbour(u64 *a, u64 *b);

static struct ProgramConcat *a_b_concat = NULL;
void simulation_iterate(simulation_t *sim)
{
  // Pick a pair to react
  u64 a = 0, b = 0;
  simulation_pick_rng_neighbour(&a, &b);

  // Construct our concatenation object if not done already
  if (!a_b_concat)
  {
    a_b_concat = calloc(1, sizeof(*a_b_concat));
  }
  // Ensure a_b_concat is completely zeroed
  memset(a_b_concat, 0, sizeof(*a_b_concat));

  // Reaction: Concat, execute, split.
  program_concat(a_b_concat, sim->memory + (a * SIZEOF_PROGRAM),
                 sim->memory + (b * SIZEOF_PROGRAM));
  program_execute(a_b_concat);
  program_split(a_b_concat);
}

Color simulation_cell_color(const bf_token cell);
void simulation_draw(simulation_t *sim)
{
  for (size_t i = 0; i < NUM_PROGRAMS; ++i)
  {
    // Each program is a contiguous block of SIZEOF_PROGRAM bytes in the
    // simulation memory.
    const bf_token *base = sim->memory + (i * SIZEOF_PROGRAM);

    // Compute the coordinates in the simulation square for this program, in
    // cells.
    u64 s_x = (i % SIMULATION_ROW_SIZE) * PROGRAM_ROW_SIZE;
    u64 s_y = (i / SIMULATION_ROW_SIZE) * PROGRAM_ROW_SIZE;

    for (u64 j = 0; j < SIZEOF_PROGRAM; ++j)
    {
      // Compute the position of this cell in the simulation sphere.
      u64 p_x = j % PROGRAM_ROW_SIZE;
      u64 p_y = j / PROGRAM_ROW_SIZE;
      p_x += s_x;
      p_y += s_y;

      Color color = simulation_cell_color(base[j]);

      DrawRectangleV((Vector2){CELL_WIDTH * p_x, CELL_HEIGHT * p_y},
                     (Vector2){CELL_WIDTH, CELL_HEIGHT}, color);
    }

#if DRAW_PROGRAM_OUTLINE
    DrawRectangleLinesEx(
        (Rectangle){
            .x      = s_x * CELL_WIDTH,
            .y      = s_y * CELL_HEIGHT,
            .width  = CELL_WIDTH * PROGRAM_ROW_SIZE,
            .height = CELL_HEIGHT * PROGRAM_ROW_SIZE,
        },
        CELL_WIDTH / PROGRAM_ROW_SIZE, DARKGRAY);
#endif
  }
}

void simulation_pick_rng_pair(u64 *a, u64 *b)
{
  while (*a == *b)
  {
    *a = rand() % NUM_PROGRAMS;
    *b = rand() % NUM_PROGRAMS;
  }
}

void simulation_pick_rng_neighbour(u64 *a, u64 *b)
{
  *a      = rand() % NUM_PROGRAMS;
  u64 a_x = *a % SIMULATION_ROW_SIZE;
  u64 a_y = *a / SIMULATION_ROW_SIZE;

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
    neighbours[size++] = *a - 1;
    if (valid_dir[2])
    {
      neighbours[size++] = *a - SIMULATION_ROW_SIZE - 1;
    }
    if (valid_dir[3])
    {
      neighbours[size++] = *a + SIMULATION_ROW_SIZE - 1;
    }
  }
  if (valid_dir[1])
  {
    neighbours[size++] = *a + 1;
    if (valid_dir[2])
    {
      neighbours[size++] = *a - SIMULATION_ROW_SIZE + 1;
    }
    if (valid_dir[3])
    {
      neighbours[size++] = *a + SIMULATION_ROW_SIZE + 1;
    }
  }
  if (valid_dir[2])
  {
    neighbours[size++] = *a - SIMULATION_ROW_SIZE;
  }
  if (valid_dir[3])
  {
    neighbours[size++] = *a + SIMULATION_ROW_SIZE;
  }

  *b = neighbours[rand() % size];
}

static const char *VALID_OPS         = "<>{}-+.,[]";
static const Color possible_colors[] = {
    ['<'] = {230, 25, 75, 255},  ['>'] = {60, 180, 75, 255},
    ['{'] = {255, 225, 25, 255}, ['}'] = {0, 130, 200, 255},
    ['-'] = {245, 130, 48, 255}, ['+'] = {145, 30, 180, 255},
    ['.'] = {70, 240, 240, 255}, [','] = {240, 50, 230, 255},
    ['['] = {210, 245, 60, 255}, [']'] = {250, 190, 212, 255},
    ['\0'] = {0, 0, 0, 255},
};

Color simulation_cell_color(const bf_token cell)
{
  return possible_colors[(!!strchr(VALID_OPS, (u8)cell)) * cell];
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
