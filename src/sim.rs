//! Complete program simulation.

use crate::{bf::Token, constants::SIMULATION_SIZE};

#[derive(Debug)]
pub struct Simulation {
    pub tokens: [Token; SIMULATION_SIZE],
}
