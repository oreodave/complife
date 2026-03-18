/* simulation.h: Simulation
 * Created: 2026-03-17
 * Author: Aryadev Chavali
 * License: See end of file
 */

#ifndef SIMULATION_H
#define SIMULATION_H

#include "bf.h"

typedef struct
{
  bf_token memory[SIMULATION_SIZE];
} simulation_t;

void simulation_mutate(simulation_t *sim);
void simulation_iterate(simulation_t *sim);
void simulation_draw(simulation_t *sim);

#endif

/* Copyright (C) 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License Version 2 for
 * details.

 * You may distribute and modify this code under the terms of the GNU General
 * Public License Version 2, which you should have received a copy of along
 with
 * this program.  If not, please go to <https://www.gnu.org/licenses/>.

 */
