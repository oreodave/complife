/* simulation.h: Simulation handlers and helpers
 * Created: 2026-03-10
 * Author: Aryadev Chavali
 * License: See end of file
 * Commentary:
 */

#ifndef SIMULATION_H
#define SIMULATION_H

#include "program_iter.h"

#define NUM_PROGRAMS_POW_2 10
#define NUM_PROGRAMS       (1LU << NUM_PROGRAMS_POW_2)
#define SIMULATION_SIZE    (SIZEOF_PROGRAM * NUM_PROGRAMS)

// Simulation is simply a massive tape.  We pick two 64 byte portions of this
// buffer for our update procedure, which is based on program_iter.
struct Simulation
{
  char buffer[SIMULATION_SIZE];
  u64 p1, p2;
};

void simulation_init(struct Simulation *sim);
void simulation_update(struct Simulation *sim);

#endif

/* Copyright (C) 2026 Aryadev Chavali

 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License Version 2 for
 * details.

 * You may distribute and modify this code under the terms of the GNU General
 * Public License Version 2, which you should have received a copy of along with
 * this program.  If not, please go to <https://www.gnu.org/licenses/>.

 */
