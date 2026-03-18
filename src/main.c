/* main.c: Entrypoint
 * Created: 2026-03-09
 * Author: Aryadev Chavali
 * License: See end of file
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <time.h>

#include <raylib.h>
#include <raymath.h>

#include "base.h"
#include "bf.h"
#include "simulation.h"
#include "sv.h"
#include "vec.h"

static char *VALID_OPS = "<>{}-+.,[]";

void simulation_soup(simulation_t *sim)
{
  for (size_t i = 0; i < sizeof(sim->memory); ++i)
  {
    sim->memory[i] = rand() % 255;
  }
}

struct ThreadState
{
  simulation_t *simulation;
  bool paused, done;
};

static const struct timespec THREAD_SLEEP = {.tv_nsec = 5000};
int thread_simulation_iterate(void *ptr)
{
  struct ThreadState *state = ptr;
  while (!state->done)
  {
    while (state->paused && !state->done)
    {
      thrd_sleep(&THREAD_SLEEP, NULL);
    }
    simulation_iterate(state->simulation);
  }
  return 0;
}

int thread_simulation_mutate(void *ptr)
{
  struct ThreadState *state = ptr;
  while (!state->done)
  {
    while (state->paused && !state->done)
    {
      thrd_sleep(&THREAD_SLEEP, NULL);
    }
    simulation_mutate(state->simulation);
  }
  return 0;
}

int main(void)
{
  simulation_t sim = {0};
  simulation_soup(&sim);
  srand(time(NULL));

  auto const MOVEMENT_SPEED = 5.0f;
  auto const ZOOM_SPEED     = 0.5f;
  auto const ZOOM_MAX       = 5.0f;

  thrd_t thread_iterator, thread_mutator;
  struct ThreadState state_iterator = {
      .simulation = &sim,
      .paused     = true,
      .done       = false,
  };
  struct ThreadState state_mutator = {
      .simulation = &sim,
      .paused     = true,
      .done       = false,
  };
  thrd_create(&thread_iterator, thread_simulation_iterate, &state_iterator);
  thrd_create(&thread_mutator, thread_simulation_mutate, &state_mutator);

  InitWindow(WIDTH, HEIGHT, "CompLife");
  SetTargetFPS(60);

  const Vector2 screen_to_program = {CELL_WIDTH * PROGRAM_ROW_SIZE,
                                     CELL_HEIGHT * PROGRAM_ROW_SIZE};

  Camera2D camera = {0};
  camera.target   = (Vector2){0};
  camera.offset   = (Vector2){0};
  camera.rotation = 0.0f;
  camera.zoom     = 1.0f;

  for (size_t ticks = 0; !WindowShouldClose(); ++ticks)
  {
    if (IsKeyPressed(KEY_SPACE))
    {
      state_iterator.paused = !state_iterator.paused;
      state_mutator.paused  = !state_mutator.paused;
    }
    if (IsKeyDown(KEY_W))
    {
      camera.target.y -= MOVEMENT_SPEED;
    }
    if (IsKeyDown(KEY_S))
    {
      camera.target.y += MOVEMENT_SPEED;
    }
    if (IsKeyDown(KEY_A))
    {
      camera.target.x -= MOVEMENT_SPEED;
    }
    if (IsKeyDown(KEY_D))
    {
      camera.target.x += MOVEMENT_SPEED;
    }
    if (IsKeyPressed(KEY_Z))
    {
      camera.zoom += ZOOM_SPEED;
      camera.zoom = Clamp(camera.zoom, 1.0, ZOOM_MAX);
    }
    if (IsKeyPressed(KEY_X))
    {
      camera.zoom -= ZOOM_SPEED;
      camera.zoom = Clamp(camera.zoom, 1.0, ZOOM_MAX);
    }
    if (IsKeyPressed(KEY_PERIOD))
    {
      camera.offset = (Vector2){0};
      camera.target = (Vector2){0};
      camera.zoom   = 1.0f;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && camera.zoom == 1.0f &&
        camera.offset.x == 0 && camera.offset.y == 0)
    {
      Vector2 position = GetMousePosition();
      position         = Vector2Divide(position, screen_to_program);
      position         = (Vector2){floor(position.x), floor(position.y)};

      u64 abs_offset       = (position.x + (position.y * SIMULATION_ROW_SIZE));
      const bf_token *base = sim.memory + abs_offset;
      printf("(%d, %d): ", (i32)position.x, (i32)position.y);
      for (u64 i = 0; i < SIZEOF_PROGRAM; ++i)
      {
        if (base[i] && strchr(VALID_OPS, base[i]))
        {
          printf("%c ", base[i]);
        }
      }
      printf("\n");
    }

    BeginDrawing();
    BeginMode2D(camera);
    ClearBackground(BLACK);
    simulation_draw(&sim);
    EndMode2D();
    EndDrawing();
  }
  state_iterator.done = true;
  state_mutator.done  = true;
  thrd_join(thread_iterator, NULL);
  thrd_join(thread_mutator, NULL);
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
