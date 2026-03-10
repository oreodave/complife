/* main.c: Entrypoint
 * Created: 2026-03-09
 * Author: Aryadev Chavali
 * License: See end of file
 */

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "base.h"
#include "sv.h"
#include "vec.h"

#include "program_iter.h"
#include "simulation.h"

static const size_t CELL_WIDTH = WIDTH / GRID_WIDTH;

static const char *VALID_OPS = "<>{}-+.,[]";

Color simulation_cell_color(const u8 *program)
{
  // How do we compute a "colour" for a program?  I say we count all the valid
  // opcodes in the program.  These counts are used as weights for 10 distinct
  // colours.
  const Vector4 bases[] = {
      ['<'] = ColorNormalize(ColorFromHSV(0.121, 0.467, 0.706)),
      ['>'] = ColorNormalize(ColorFromHSV(1.000, 0.498, 0.055)),
      ['{'] = ColorNormalize(ColorFromHSV(0.173, 0.627, 0.173)),
      ['}'] = ColorNormalize(ColorFromHSV(0.839, 0.153, 0.157)),
      ['-'] = ColorNormalize(ColorFromHSV(0.580, 0.404, 0.741)),
      ['+'] = ColorNormalize(ColorFromHSV(0.549, 0.337, 0.294)),
      ['.'] = ColorNormalize(ColorFromHSV(0.890, 0.467, 0.761)),
      [','] = ColorNormalize(ColorFromHSV(0.498, 0.498, 0.498)),
      ['['] = ColorNormalize(ColorFromHSV(0.737, 0.741, 0.133)),
      [']'] = ColorNormalize(ColorFromHSV(0.090, 0.745, 0.812)),
  };

  u64 counter[] = {
      ['<'] = 0, ['>'] = 0, ['{'] = 0, ['}'] = 0, ['-'] = 0,
      ['+'] = 0, ['.'] = 0, [','] = 0, ['['] = 0, [']'] = 0,
  };

  u64 total_valid = 0;
  for (u64 i = 0; i < SIZEOF_PROGRAM; ++i)
  {
    if (strchr(VALID_OPS, program[i]))
    {
      counter[(u64)program[i]]++;
      ++total_valid;
    }
  }

  if (total_valid == 0)
    return BLACK;

  f64 colour_cells[3];
  for (const char *ptr = VALID_OPS; *ptr; ++ptr)
  {
    colour_cells[0] += bases[(u64)*ptr].x;
    colour_cells[1] += bases[(u64)*ptr].y;
    colour_cells[2] += bases[(u64)*ptr].z;
  }
  colour_cells[0] /= total_valid;
  colour_cells[1] /= total_valid;
  colour_cells[2] /= total_valid;

  return (Color){.r = 255 * colour_cells[0],
                 .g = 255 * colour_cells[1],
                 .b = 255 * colour_cells[2],
                 .a = 255};
}

void simulation_draw(struct Simulation *sim)
{
  sv_t sv = SV(sim->buffer, SIMULATION_SIZE);

  for (u64 i = 0; i < SIMULATION_SIZE / SIZEOF_PROGRAM; ++i)
  {
    sv_t program = sv_truncate(sv, SIZEOF_PROGRAM);
    Color color  = simulation_cell_color((const u8 *)program.data);

    u64 x = i / GRID_WIDTH;
    u64 y = i % GRID_WIDTH;
    DrawRectangle(x * CELL_WIDTH, y * CELL_WIDTH, CELL_WIDTH, CELL_WIDTH,
                  color);

    sv = sv_chop_left(sv, 64);
  }

  for (u64 i = 0; i < THREAD_POOL; ++i)
  {
    u64 p1 = sim->states[i].p1;
    u64 p2 = sim->states[i].p2;
    DrawRectangleLines((p1 / GRID_WIDTH) * CELL_WIDTH,
                       (p1 % GRID_WIDTH) * CELL_WIDTH, CELL_WIDTH, CELL_WIDTH,
                       BLUE);
    DrawRectangleLines((p2 / GRID_WIDTH) * CELL_WIDTH,
                       (p2 % GRID_WIDTH) * CELL_WIDTH, CELL_WIDTH, CELL_WIDTH,
                       RED);
  }
}

int main(void)
{
  srand(time(NULL));

  struct Simulation sim = {0};
  simulation_start(&sim);

  InitWindow(WIDTH, HEIGHT, "CompLife");
  SetTargetFPS(60);
  for (size_t ticks = 0; !WindowShouldClose(); ++ticks)
  {
    if (IsKeyPressed(KEY_SPACE))
    {
      sim.paused = !sim.paused;
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
      Vector2 pos = GetMousePosition();
      u64 x       = (pos.x / WIDTH) * GRID_WIDTH;
      u64 y       = (pos.y / WIDTH) * GRID_WIDTH;
      printf("(%lu,%lu)=", x, y);
      sv_t sv = sv_truncate(
          sv_chop_left(SV(sim.buffer, SIMULATION_SIZE), (x * GRID_WIDTH) + y),
          64);
      for (u64 i = 0; i < sv.size; ++i)
      {
        if (strchr(VALID_OPS, sv.data[i]))
        {
          printf("%c", sv.data[i]);
        }
      }
      printf("\n");
    }
    BeginDrawing();
    ClearBackground(BLACK);
    simulation_draw(&sim);
    EndDrawing();
  }
  simulation_stop(&sim);
  CloseWindow();
  return 0;
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
