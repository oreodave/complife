/* simulation.c: Simulation implementation
 * Created: 2026-03-10
 * Author: Aryadev Chavali
 * License: See end of file
 */

#include <stdlib.h>
#include <string.h>

#include "simulation.h"

bool any_threads_using(const struct ThreadState *const states, u64 n)
{
  for (u64 i = 0; i < THREAD_POOL; ++i)
  {
    if (states[i].p1 == n || states[i].p2 == n)
    {
      return true;
    }
  }
  return false;
}

void thread_pick(struct ThreadState *state)
{
  struct Simulation *sim = state->sim;
  while (true)
  {
    u64 p1 = rand() % (SIMULATION_SIZE / SIZEOF_PROGRAM);
    if (any_threads_using(sim->states, p1))
    {
      continue;
    }
    u64 p2 = rand() % (SIMULATION_SIZE / SIZEOF_PROGRAM);
    while (p1 * 8 <= ((p2 * 8) + SIZEOF_PROGRAM) &&
           p2 * 8 <= ((p1 * 8) + SIZEOF_PROGRAM))
    {
      p2 = rand() % (SIMULATION_SIZE / SIZEOF_PROGRAM);
    }
    if (any_threads_using(sim->states, p2))
    {
      continue;
    }
    state->p1 = p1;
    state->p2 = p2;
    break;
  }
}

const struct timespec THREAD_DEFAULT_SLEEP = {.tv_sec = 1};

int thread_update(void *rawptr)
{
  struct ThreadState *state     = rawptr;
  struct Simulation *simulation = state->sim;

  while (!simulation->stopped)
  {
    while (simulation->paused)
    {
      thrd_sleep(&THREAD_DEFAULT_SLEEP, NULL);
    }

    // We only need to lock when picking the programs
    mtx_lock(&simulation->mutex);
    thread_pick(state);
    mtx_unlock(&simulation->mutex);

    sv_t a = SV(simulation->buffer + (state->p1 * SIZEOF_PROGRAM), 64);
    sv_t b = SV(simulation->buffer + (state->p2 * SIZEOF_PROGRAM), 64);

    struct ProgramConcat prog_concat = {0};
    program_concat(&prog_concat, a, b);
    program_execute(&prog_concat);
    program_split(&prog_concat);
  }
  return 0;
}

void simulation_start(struct Simulation *sim)
{
  for (u64 i = 0; i < SIMULATION_SIZE / sizeof(u16); ++i)
  {
    ((u16 *)(sim->buffer))[i] = rand() % UINT16_MAX;
  }

  sim->stopped = false;
  sim->paused  = true;
  mtx_init(&sim->mutex, mtx_plain);
  for (u64 i = 0; i < THREAD_POOL; ++i)
  {
    memset(&sim->states[i], 0, sizeof(sim->states[i]));
    sim->states[i].sim = sim;
    thrd_create(&sim->threads[i], thread_update, &sim->states[i]);
  }
}

void simulation_stop(struct Simulation *sim)
{
  sim->stopped = true;
  for (u64 i = 0; i < THREAD_POOL; ++i)
  {
    thrd_join(sim->threads[i], NULL);
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
