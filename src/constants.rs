//! Constants for simulation.
//!
//! A one-stop shop for all the tweaking you wish to do.

pub const WIDTH: i32 = 1024;
pub const HEIGHT: i32 = 1024;

const PROGRAM_SIZE_POW_2: usize = 8;
pub const PROGRAM_SIZE: usize = 1 << PROGRAM_SIZE_POW_2;

const NUM_PROGRAMS_POW_2: usize = 12;
pub const NUM_PROGRAMS: usize = 1 << NUM_PROGRAMS_POW_2;

pub const PROGRAM_MAX_ITERS: usize = 1 << 13;

pub const SIMULATION_SIZE: usize = NUM_PROGRAMS * PROGRAM_SIZE;
