/* main.c: Entrypoint
 * Created: 2026-03-09
 * Author: Aryadev Chavali
 * License: See end of file
 */

#include <raylib.h>
#include <stdio.h>

#define WIDTH  800
#define HEIGHT 600

int main(void)
{
  InitWindow(WIDTH, HEIGHT, "CompLife");
  SetTargetFPS(60);
  while (!WindowShouldClose())
  {
    BeginDrawing();
    ClearBackground(BLACK);
    DrawFPS(0, 0);
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
