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

#include "bf.h"
#include "simulation.h"

void simulation_soup(simulation_t *sim)
{
  for (size_t i = 0; i < sizeof(sim->memory); ++i)
  {
    sim->memory[i] = (rand() % 82) + 43;
  }
}

int main(void)
{
  simulation_t sim = {0};
  simulation_soup(&sim);
  srand(time(NULL));

  InitWindow(WIDTH, HEIGHT, "CompLife");
  SetTargetFPS(60);
  for (size_t ticks = 0; !WindowShouldClose(); ++ticks)
  {
    simulation_iterate(&sim);
    BeginDrawing();
    ClearBackground(BLACK);
    simulation_draw(&sim);
    EndDrawing();
  }
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
