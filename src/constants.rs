//! Constants for simulation.
//!
//! A one-stop shop for all the tweaking you wish to do.

use crate::raylib::Vector2;

/// Window width.
pub const WIDTH: i32 = 1024;
/// Window height.
pub const HEIGHT: i32 = 1024;

const PROGRAM_SIZE_POW_2: usize = 8;
const NUM_PROGRAMS_POW_2: usize = 8;

/// Size of an individual program.
pub const PROGRAM_SIZE: usize = 1 << PROGRAM_SIZE_POW_2;
/// Number of programs within the simulation.
pub const NUM_PROGRAMS: usize = 1 << NUM_PROGRAMS_POW_2;
/// Total number of cells within the simulation.
pub const SIMULATION_SIZE: usize = NUM_PROGRAMS * PROGRAM_SIZE;

/// Number of cells within a row of a program when the program is treated as a
/// perfect square of cells.
pub const PROGRAM_ROW_SIZE: usize = 1 << (PROGRAM_SIZE_POW_2 / 2);
/// Number of programs within a row of the simulation when the simulation is
/// treated as a perfect square of programs.
pub const SIMULATION_ROW_SIZE: usize = 1 << (NUM_PROGRAMS_POW_2 / 2);
/// Number of cells within a row of the simulation when the simulation is
/// treated as a perfect square of programs.
pub const CELL_ROW_SIZE: usize = PROGRAM_ROW_SIZE * SIMULATION_ROW_SIZE;

/// Dimensions of a single cell when drawn to screen.
pub const CELL_DIMENSIONS: Vector2 = Vector2 {
    x: (WIDTH as f32) / (CELL_ROW_SIZE as f32),
    y: (HEIGHT as f32) / (CELL_ROW_SIZE as f32),
};

/// Maximum bound on iterations during program reaction.
pub const PROGRAM_MAX_ITERS: usize = 1 << 13;

/// Mutation chance (i.e. 1 in MUTATION_CHANCE chance of a given cell mutating).
pub const MUTATION_CHANCE: usize = SIMULATION_SIZE;

/// Number of parallel workers that can perform reactions.
pub const WORKER_COUNT: usize = 7;
