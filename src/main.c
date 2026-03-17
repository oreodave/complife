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

static char *VALID_OPS = "<>{}-+.,[]";

u8 get_op(const u8 cell)
{
  if (strchr(VALID_OPS, cell))
    return cell;
  else
    return '\0';
}

Color simulation_cell_color(const u8 cell)
{
  const Color possible_colors[] = {
      ['<']  = ColorFromHSV(0.121, 0.467, 0.706),
      ['>']  = ColorFromHSV(1.000, 0.498, 0.055),
      ['{']  = ColorFromHSV(0.173, 0.627, 0.173),
      ['}']  = ColorFromHSV(0.839, 0.153, 0.157),
      ['-']  = ColorFromHSV(0.580, 0.404, 0.741),
      ['+']  = ColorFromHSV(0.549, 0.337, 0.294),
      ['.']  = ColorFromHSV(0.890, 0.467, 0.761),
      [',']  = ColorFromHSV(0.498, 0.498, 0.498),
      ['[']  = ColorFromHSV(0.737, 0.741, 0.133),
      [']']  = ColorFromHSV(0.090, 0.745, 0.812),
      ['\0'] = BLACK,
  };

  return possible_colors[get_op(cell)];
}

int main(void)
{
  srand(time(NULL));

  InitWindow(WIDTH, HEIGHT, "CompLife");
  SetTargetFPS(60);
  for (size_t ticks = 0; !WindowShouldClose(); ++ticks)
  {
    BeginDrawing();
    ClearBackground(BLACK);
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
