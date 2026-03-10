/* simulation.c: Simulation implementation
 * Created: 2026-03-10
 * Author: Aryadev Chavali
 * License: See end of file
 */

#include <stdlib.h>

#include "simulation.h"

void simulation_init(struct Simulation *sim)
{
  for (u64 i = 0; i < SIMULATION_SIZE / sizeof(u16); ++i)
  {
    ((u16 *)(sim->buffer))[i] = rand() % UINT16_MAX;
  }
}

void simulation_pick(struct Simulation *sim)
{
  sim->p1 = rand() % (SIMULATION_SIZE / SIZEOF_PROGRAM);
  sim->p2 = rand() % (SIMULATION_SIZE / SIZEOF_PROGRAM);
  while (sim->p1 * 8 <= ((sim->p2 * 8) + SIZEOF_PROGRAM) &&
         sim->p2 * 8 <= ((sim->p1 * 8) + SIZEOF_PROGRAM))
  {
    sim->p2 = rand() % (SIMULATION_SIZE / SIZEOF_PROGRAM);
  }
}

void simulation_update(struct Simulation *sim)
{
  simulation_pick(sim);
  sv_t a = SV(sim->buffer + (sim->p1 * SIZEOF_PROGRAM), 64);
  sv_t b = SV(sim->buffer + (sim->p2 * SIZEOF_PROGRAM), 64);

  struct ProgramConcat prog_concat = {0};
  program_concat(&prog_concat, a, b);
  program_execute(&prog_concat);
  program_split(&prog_concat);
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
