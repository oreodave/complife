/* bf.c: Program iteration implementation
 * Created: 2026-03-10
 * Author: Aryadev Chavali
 * License: See end of file
 */

#include <string.h>

#include "bf.h"
#include "vec.h"

void program_concat(struct ProgramConcat *ret, bf_token *a, bf_token *b)
{
  assert(a && b);
  memset(ret, 0, sizeof(*ret));
  ret->a = a;
  ret->b = b;
  memcpy(ret->tape, a, SIZEOF_PROGRAM);
  memcpy(ret->tape + SIZEOF_PROGRAM, b, SIZEOF_PROGRAM);
}

struct MachineContext
{
  u64 ip, head0, head1;
  u64 total_iters;
};

// This function implements the jump-forward, given the current state of the
// machine.  The main thing to concern us here is setting up the cond_stack
// correctly.
void loop_begin(struct ProgramConcat *prg, struct MachineContext *ctx,
                vec_t *cond_stack)
{
  // Add to cond_stack if not already present.
  if (vec_find(cond_stack, &ctx->ip, sizeof(ctx->ip)) > cond_stack->size)
  {
    vec_append(cond_stack, &ctx->ip, sizeof(ctx->ip));
  }

  // If tape at head0 is nonzero, then go to the next instruction.
  if (prg->tape[ctx->head0])
  {
    ++ctx->ip;
    return;
  }

  // Otherwise, we need to find the corresponding closed bracket and jump to it.
  u64 square_brackets = 1;
  u64 close_ip;
  for (close_ip = ctx->ip + 1;
       square_brackets != 0 && close_ip < sizeof(prg->tape); ++close_ip)
  {
    switch (prg->tape[close_ip])
    {
    case '[':
      ++square_brackets;
      break;
    case ']':
      --square_brackets;
      break;
    }
  }

  if (!square_brackets)
  {
    ctx->ip = close_ip;
  }
  else
  {
    // NOTE: as per paper, terminate.
    ctx->ip = sizeof(prg->tape);
  }
}

void program_execute(struct ProgramConcat *prg)
{
  vec_t cond_stack = {0};
  vec_ensure_capacity(&cond_stack, sizeof(prg->tape) * sizeof(u64));

  for (struct MachineContext ctx = {0};
       ctx.ip < sizeof(prg->tape) && ctx.total_iters < (1LU << 13);
       ++ctx.total_iters)
  {
    u8 opcode = prg->tape[ctx.ip];
    switch (opcode)
    {
    case '<':
      ctx.head0 = SAFE_SUB(ctx.head0, 1);
      ++ctx.ip;
      break;
    case '>':
      ctx.head0++;
      ++ctx.ip;
      break;
    case '{':
      ctx.head1 = SAFE_SUB(ctx.head1, 1);
      ++ctx.ip;
      break;
    case '}':
      ctx.head1++;
      ++ctx.ip;
      break;
    case '-':
      prg->tape[ctx.head0]--;
      ++ctx.ip;
      break;
    case '+':
      prg->tape[ctx.head0]++;
      ++ctx.ip;
      break;
    case '.':
      prg->tape[ctx.head1] = prg->tape[ctx.head0];
      ++ctx.ip;
      break;
    case ',':
      prg->tape[ctx.head0] = prg->tape[ctx.head1];
      ++ctx.ip;
      break;
    case '[':
    {
      loop_begin(prg, &ctx, &cond_stack);
      break;
    }
    case ']':
    {
      if (!prg->tape[ctx.head0])
      {
        ++ctx.ip;
        continue;
      }
      else if (cond_stack.size < sizeof(u64))
      {
        // NOTE: as per paper, terminate.
        ctx.ip = sizeof(prg->tape);
      }
      else
      {
        ctx.ip = *(u64 *)vec_pop(&cond_stack, sizeof(ctx.ip));
      }
      break;
    }
    default:
      ++ctx.ip;
      break;
    }
  }

  vec_free(&cond_stack);
}

void program_split(struct ProgramConcat *prg)
{
  assert(prg->a && prg->b);
  memcpy(prg->a, prg->tape, SIZEOF_PROGRAM);
  memcpy(prg->b, prg->tape + SIZEOF_PROGRAM, SIZEOF_PROGRAM);
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
