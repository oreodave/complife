//! Generalised state of simulation.

use crate::{
    bf::Token,
    constants::{NUM_PROGRAMS, SIMULATION_SIZE},
};

pub struct State {
    pub data: Box<[Token]>,
    pub reserved: [bool; NUM_PROGRAMS],
    pub mutating: bool,
}

impl State {
    pub fn new() -> Self {
        let mut tokens = vec![0; SIMULATION_SIZE];
        rand::fill(&mut tokens[..]);
        let data = tokens.into_boxed_slice();

        Self {
            data,
            reserved: [false; NUM_PROGRAMS],
            mutating: false,
        }
    }
}
