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

#include "./lib/prick_aliases.h"

#define SV_IMPL
#include "./lib/prick_sv.h"

#define VEC_IMPL
#include "./lib/prick_vec.h"

#define SAFE_SUB(A, B) ((A) < (B) ? 0 : (A) - (B))

#define SIZEOF_PROGRAM (1LU << 6)
struct ProgramConcat
{
  sv_t A, B;
  u8 tape[SIZEOF_PROGRAM * 2];
};

void program_concat(struct ProgramConcat *ret, sv_t a, sv_t b)
{
  assert(a.size == SIZEOF_PROGRAM && b.size == SIZEOF_PROGRAM);
  memset(ret, 0, sizeof(*ret));
  ret->A = a;
  ret->B = b;
  memcpy(ret->tape, a.data, SIZEOF_PROGRAM);
  memcpy(ret->tape + SIZEOF_PROGRAM, b.data, SIZEOF_PROGRAM);
}

u64 vec_pop(vec_t *vec)
{
  u64 ret = 0;
  if (vec->size < sizeof(ret))
    return ret;
  vec->size -= sizeof(ret);
  memcpy(&ret, (typeof(ret) *)(((u8 *)vec_data(vec)) + vec->size), sizeof(ret));
  return ret;
}

bool vec_in(vec_t *vec, u64 n)
{
  for (u64 i = 0; i < vec->size / sizeof(n); ++i)
  {
    if (VEC_GET(vec, i, typeof(n)) == n)
    {
      return true;
    }
  }
  return false;
}

void program_execute(struct ProgramConcat *prg)
{
  vec_t cond_stack = {0};
  vec_ensure_capacity(&cond_stack, sizeof(prg->tape) * sizeof(u64));

  for (u64 ip = 0, head0 = 0, head1 = 0, total_iters = 0;
       ip < sizeof(prg->tape) && total_iters < (1LU << 13); ++total_iters)
  {
    u8 opcode = prg->tape[ip];
    switch (opcode)
    {
    case '<':
      head0 = SAFE_SUB(head0, 1);
      ++ip;
      break;
    case '>':
      head0++;
      ++ip;
      break;
    case '{':
      head1 = SAFE_SUB(head1, 1);
      ++ip;
      break;
    case '}':
      head1++;
      ++ip;
      break;
    case '-':
      prg->tape[head0]--;
      ++ip;
      break;
    case '+':
      prg->tape[head0]++;
      ++ip;
      break;
    case '.':
      prg->tape[head1] = prg->tape[head0];
      ++ip;
      break;
    case ',':
      prg->tape[head0] = prg->tape[head1];
      ++ip;
      break;
    case '[':
    {
      if (!vec_in(&cond_stack, ip))
      {
        vec_append(&cond_stack, &ip, sizeof(ip));
      }
      if (!prg->tape[head0])
      {
        // Iterate forward, trying to find a matching closed bracket
        u64 square_brackets = 0;
        u64 close_ip;
        for (close_ip = ip + 1; close_ip < sizeof(prg->tape); ++close_ip)
        {
          if (prg->tape[close_ip] == '[')
          {
            ++square_brackets;
          }
          else if (prg->tape[close_ip] == ']')
          {
            if (square_brackets == 0)
            {
              break;
            }
            --square_brackets;
          }
        }
        if (square_brackets != 0)
        {
          // NOTE: as per paper, terminate.
          ip = sizeof(prg->tape);
        }
        else
        {
          ip = close_ip;
        }
      }
      break;
    }
    case ']':
    {
      if (prg->tape[head0])
      {
        if (cond_stack.size < sizeof(u64))
        {
          // NOTE: as per paper, terminate.
          ip = sizeof(prg->tape);
        }
        else
        {
          ip = vec_pop(&cond_stack);
        }
      }
      else
      {
        ++ip;
      }
      break;
    }
    default:
      ++ip;
      break;
    }
  }

  vec_free(&cond_stack);
}

void program_split(struct ProgramConcat *prg)
{
  assert(prg->A.data && prg->B.data);
  memcpy((char *)prg->A.data, prg->tape, SIZEOF_PROGRAM);
  memcpy((char *)prg->B.data, prg->tape + SIZEOF_PROGRAM, SIZEOF_PROGRAM);
}

#define WIDTH  800
#define HEIGHT 600

#define NUM_PROGRAMS_POW_2 10
#define NUM_PROGRAMS       (1LU << NUM_PROGRAMS_POW_2)
#define SIMULATION_SIZE    (SIZEOF_PROGRAM * NUM_PROGRAMS)
struct Simulation
{
  char buffer[SIMULATION_SIZE];
  u64 p1, p2;
};

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
  sv_t a = SV(sim->buffer + (sim->p1 * SIZEOF_PROGRAM), 64);
  sv_t b = SV(sim->buffer + (sim->p2 * SIZEOF_PROGRAM), 64);

  struct ProgramConcat prog_concat = {0};
  program_concat(&prog_concat, a, b);
  program_execute(&prog_concat);
  program_split(&prog_concat);
}

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

  static const char *VALID_OPS = "<>{}-+.,[]";

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
  // Our grid will be of lengths sqrt(NUM_PROGRAMS) == 1 <<
  // (NUM_PROGRAMS_POW_2/2).
  const size_t GRID_WIDTH = 1LU << (NUM_PROGRAMS_POW_2 / 2);
  const size_t CELL_WIDTH = WIDTH / GRID_WIDTH;

  sv_t sv = SV(sim->buffer, SIMULATION_SIZE);

  for (u64 i = 0; i < SIMULATION_SIZE / SIZEOF_PROGRAM; ++i)
  {
    sv_t program = sv_truncate(sv, SIZEOF_PROGRAM);

    Color color = simulation_cell_color((const u8 *)program.data);

    u64 x = i / GRID_WIDTH;
    u64 y = i % GRID_WIDTH;
    DrawRectangle(x * CELL_WIDTH, y * CELL_WIDTH, CELL_WIDTH, CELL_WIDTH,
                  color);

    if (i == sim->p1 || i == sim->p2)
    {
      DrawRectangleLines(x * CELL_WIDTH, y * CELL_WIDTH, CELL_WIDTH, CELL_WIDTH,
                         BLUE);
    }

    sv = sv_chop_left(sv, 64);
  }
}

int main(void)
{
  srand(time(NULL));

  struct Simulation sim = {0};
  simulation_init(&sim);

  InitWindow(WIDTH, HEIGHT, "CompLife");
  SetTargetFPS(60);
  for (size_t ticks = 0; !WindowShouldClose(); ++ticks)
  {
    simulation_pick(&sim);
    simulation_update(&sim);
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
