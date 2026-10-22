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
        const POSSIBLE: [char; 10] =
            ['<', '>', '{', '}', '-', '+', '.', ',', '[', ']'];
        let mut tokens = vec![0; SIMULATION_SIZE];
        for token in &mut tokens {
            let choice = rand::random_range(0..11);
            if choice == 10 {
                *token = rand::random();
            } else {
                *token = POSSIBLE[choice] as u8;
            }
        }
        // rand::fill(&mut tokens[..]);
        let data = tokens.into_boxed_slice();

        Self {
            data,
            reserved: [false; NUM_PROGRAMS],
            mutating: false,
        }
    }
}
