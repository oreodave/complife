/* program_iter.c: Program iteration implementation
 * Created: 2026-03-10
 * Author: Aryadev Chavali
 * License: See end of file
 */

#include <string.h>

#include "program_iter.h"
#include "vec.h"

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

/* Copyright (C) 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License Version 2 for
 * details.

 * You may distribute and modify this code under the terms of the GNU General
 * Public License Version 2, which you should have received a copy of along with
 * this program.  If not, please go to <https://www.gnu.org/licenses/>.

 */
