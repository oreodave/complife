//! Simulation "reactions".
//!
//! The computational life model reacts two programs by _concatenating_ the
//! programs together into one tape, _executing_ the concatenated tape as a
//! modified version of Brainfuck, then _splitting_ the concatenated tape back
//! into its subprograms, returning them to the simulation.

use crate::constants::{NUM_PROGRAMS, PROGRAM_MAX_ITERS, PROGRAM_SIZE};

pub type Token = u8;

/// Identifier for a program.  These must be in the range (0..NUM_PROGRAMS).
#[derive(Debug, Copy, Clone, PartialEq, Eq)]
pub struct ProgramId(usize);

impl ProgramId {
    pub fn new(n: usize) -> Self {
        debug_assert!(n < NUM_PROGRAMS);
        Self(n)
    }

    pub fn cell_range(self) -> std::ops::Range<usize> {
        let start = self.0 * PROGRAM_SIZE;
        start..start + PROGRAM_SIZE
    }
}

/// The state of some Reaction in progress.
#[derive(Debug)]
pub struct Reaction {
    /// First Program's ID.
    first: ProgramId,
    /// Second Program's ID.
    second: ProgramId,
    /// Contiguous tape for program execution.
    tape: [Token; PROGRAM_SIZE * 2],
}

impl Reaction {
    /// Construct a new [`Reaction`] by concatenating the [`Token`] buffers
    /// within the given [`Simulation`] at exactly two points; `first` and
    /// `second`.
    pub fn new(
        first: ProgramId,
        second: ProgramId,
        simulation: &[Token],
    ) -> Self {
        let mut tape: [Token; PROGRAM_SIZE * 2] = [0; PROGRAM_SIZE * 2];
        {
            let (first_half, second_half) = tape.split_at_mut(PROGRAM_SIZE);
            first_half.copy_from_slice(&simulation[first.cell_range()]);
            second_half.copy_from_slice(&simulation[second.cell_range()]);
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
    pub fn finish(self, simulation: &mut [Token]) {
        let (first_half, second_half) = self.tape.split_at(PROGRAM_SIZE);
        simulation[self.first.cell_range()].copy_from_slice(first_half);
        simulation[self.second.cell_range()].copy_from_slice(second_half);
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
                self.head_0 = self.head_0.wrapping_sub_signed(1);
                self.head_0 %= self.tape.len();
                self.ip += 1;
            }

            '>' => {
                self.head_0 = self.head_0.wrapping_add(1);
                self.head_0 %= self.tape.len();
                self.ip += 1;
            }

            '{' => {
                self.head_1 = self.head_1.wrapping_sub_signed(1);
                self.head_1 %= self.tape.len();
                self.ip += 1;
            }

            '}' => {
                self.head_1 = self.head_1.wrapping_add(1);
                self.head_1 %= self.tape.len();
                self.ip += 1;
            }

            '-' => {
                self.tape[self.head_0] =
                    self.tape[self.head_0].wrapping_sub_signed(1);
                self.ip += 1;
            }

            '+' => {
                self.tape[self.head_0] = self.tape[self.head_0].wrapping_add(1);
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
        if self.loop_stack.iter().find(|&&x| x == self.ip).is_none() {
            self.loop_stack.push(self.ip);
        }

        // Execution check: we only jump if the tape is zero at head_0.
        if self.tape[self.head_0] != 0 {
            self.ip += 1;
            return;
        }

        // Otherwise, attempt to look for the IP of the matching close loop.
        let mut loops = 1;
        let mut close_ip = 0;
        for (pos_ip, pos_loop) in self.tape[self.ip + 1..].iter().enumerate() {
            match *pos_loop as char {
                '[' => loops += 1,
                ']' => loops -= 1,
                _ => {}
            }

            if loops == 0 {
                close_ip = pos_ip + self.ip + 2;
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
