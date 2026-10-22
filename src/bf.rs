//! Simulation "reactions".
//!
//! The computational life model reacts two programs by _concatenating_ the
//! programs together into one tape, _executing_ the concatenated tape as a
//! modified version of Brainfuck, then _splitting_ the concatenated tape back
//! into its subprograms, returning them to the simulation.

use crate::{
    constants::{PROGRAM_MAX_ITERS, PROGRAM_SIZE},
    simulation::Simulation,
};

pub type Token = u8;

#[derive(Debug)]
pub struct Program {
    /// Starting point of the first program.
    first: usize,
    /// Starting point of the second program.
    second: usize,
    /// Contiguous tape for program execution.
    tape: [Token; PROGRAM_SIZE * 2],
}

impl Program {
    /// Construct a new [`Program`] by concatenating the [`Token`] buffers
    /// within the given [`Simulation`] at exactly two points; `first` and
    /// `second`.
    pub fn new(first: usize, second: usize, simulation: &Simulation) -> Self {
        debug_assert!(first + PROGRAM_SIZE < simulation.tokens.len());
        debug_assert!(second + PROGRAM_SIZE < simulation.tokens.len());

        let mut tape: [Token; PROGRAM_SIZE * 2] = [0; PROGRAM_SIZE * 2];
        {
            let (first_half, second_half) = tape.split_at_mut(PROGRAM_SIZE);
            first_half.copy_from_slice(
                &simulation.tokens[first..first + PROGRAM_SIZE],
            );
            second_half.copy_from_slice(
                &simulation.tokens[second..second + PROGRAM_SIZE],
            );
        }

        Self {
            first,
            second,
            tape,
        }
    }

    /// Execute the program constructed till completion.
    pub fn execute(&mut self) {
        let mut ctx = Machine {
            ip: 0,
            head_0: 0,
            head_1: 0,
            total_iters: 0,
            loop_stack: Vec::new(),
            tape: &mut self.tape,
        };

        while ctx.ip < ctx.tape.len() && ctx.total_iters < PROGRAM_MAX_ITERS {
            ctx.exec_once();
            ctx.total_iters += 1;
        }
    }

    /// Following execution, copy over the new contents of each sub-program back
    /// into the [`Simulation`].
    pub fn finish(self, simulation: &mut Simulation) {
        let (first_half, second_half) = self.tape.split_at(PROGRAM_SIZE);
        simulation.tokens[self.first..self.first + PROGRAM_SIZE]
            .copy_from_slice(first_half);
        simulation.tokens[self.second..self.second + PROGRAM_SIZE]
            .copy_from_slice(second_half);
    }
}

#[derive(Debug, Default)]
struct Machine<'a> {
    /// Machine memory from which we read/write/execute.
    tape: &'a mut [Token],
    /// Instruction pointer for this machine.
    ip: usize,
    /// First read/write head.
    head_0: usize,
    /// Second read/write head.
    head_1: usize,
    /// Total iterations performed.
    total_iters: usize,
    /// Stack of positions of loop beginnings i.e. "[".
    loop_stack: Vec<usize>,
}

impl Machine<'_> {
    /// Execute a single operation within the [`Machine::tape`].
    fn exec_once(&mut self) {
        let Some(opcode) = self.tape.get(self.ip) else {
            return;
        };

        // Look at the paper for how I derived these encodings.
        match *opcode as char {
            '<' => {
                self.head_0 -= 1;
                self.head_0 %= self.tape.len();
                self.ip += 1;
            }

            '>' => {
                self.head_0 += 1;
                self.head_0 %= self.tape.len();
                self.ip += 1;
            }

            '{' => {
                self.head_1 -= 1;
                self.head_1 %= self.tape.len();
                self.ip += 1;
            }

            '}' => {
                self.head_1 += 1;
                self.head_1 %= self.tape.len();
                self.ip += 1;
            }

            '-' => {
                self.tape[self.head_0] -= 1;
                self.ip += 1;
            }

            '+' => {
                self.tape[self.head_0] += 1;
                self.ip += 1;
            }

            '.' => {
                self.tape[self.head_1] = self.tape[self.head_0];
                self.ip += 1;
            }

            ',' => {
                self.tape[self.head_0] = self.tape[self.head_1];
                self.ip += 1;
            }

            '[' => {
                self.loop_begin();
            }

            ']' => {
                self.loop_end();
            }

            _ => {
                self.ip += 1;
            }
        }
    }

    fn loop_begin(&mut self) {
        // If this loop IP is not already present in the loop stack, add it.
        if self.loop_stack.iter().find(|&&x| x == self.ip).is_some() {
            self.loop_stack.push(self.ip);
        }

        // Execution check: we only jump if the tape is zero at head_0.
        if self.tape[self.head_0] != 0 {
            self.ip += 1;
            return;
        }

        // Otherwise, attempt to look for the IP of the matching close loop.
        let mut loops = 1;
        let mut close_ip = self.ip + 1;
        for (pos_ip, pos_loop) in self.tape[self.ip + 1..].iter().enumerate() {
            match *pos_loop as char {
                '[' => loops += 1,
                ']' => loops -= 1,
                _ => {}
            }

            if loops == 0 {
                close_ip = pos_ip;
                break;
            }
        }

        if loops == 0 {
            self.ip = close_ip;
        } else {
            // NOTE: as per paper, terminate if we can't find a
            // corresponding close loop.
            self.ip = self.tape.len();
        }
    }

    fn loop_end(&mut self) {
        if self.tape[self.head_0] == 0 {
            self.ip += 1;
        } else if let Some(l_ip) = self.loop_stack.pop() {
            self.ip = l_ip;
        } else {
            // NOTE: as per paper, terminate if we cannot jump to an appropriate
            // point.
            self.ip = self.tape.len();
        }
    }
}
