//! Generalised state of simulation.

use crate::{
    bf::Token,
    constants::{INIT_RANDOM_SOUP, NUM_PROGRAMS, SIMULATION_SIZE},
};

pub struct State {
    pub data: Box<[Token]>,
    pub reserved: [bool; NUM_PROGRAMS],
    pub mutating: bool,
}

impl State {
    pub fn new() -> Self {
        let mut tokens = vec![0; SIMULATION_SIZE];

        if INIT_RANDOM_SOUP {
            for token in &mut tokens {
                *token = biased_random_token();
            }
            rand::fill(&mut tokens[..]);
        }

        let data = tokens.into_boxed_slice();

        Self {
            data,
            reserved: [false; NUM_PROGRAMS],
            mutating: false,
        }
    }
}

const POSSIBLE: [char; 10] = ['<', '>', '{', '}', '-', '+', '.', ',', '[', ']'];
pub fn biased_random_token() -> Token {
    let choice = rand::random_range(0..(POSSIBLE.len() + 1));
    if choice == POSSIBLE.len() {
        rand::random()
    } else {
        POSSIBLE[choice] as Token
    }
}
