//! Complete program simulation.

use crate::{
    bf::{Reaction, Token},
    constants::{
        CELL_DIMENSIONS, MUTATION_CHANCE, NUM_PROGRAMS, PROGRAM_ROW_SIZE,
        PROGRAM_SIZE, SIMULATION_ROW_SIZE, SIMULATION_SIZE,
    },
    raylib::{Canvas, Color, Rectangle, Vector2, WHITE, color},
};

/// Index into [`Simulation`] of some Program.  These must be in the range
/// (0..NUM_PROGRAMS).
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

/// Generalised simulation data.
#[derive(Debug)]
pub struct Simulation {
    pub tokens: Box<[Token]>,
}

impl Simulation {
    /// Construct a new [`Simulation`].
    pub fn new() -> Self {
        let mut tokens = vec![0; SIMULATION_SIZE];
        rand::fill(&mut tokens);
        let tokens = tokens.into_boxed_slice();
        Self { tokens }
    }

    pub fn mutate(&mut self) {
        if MUTATION_CHANCE == 0 {
            return;
        }
        for cell in &mut self.tokens {
            if rand::random_range(1..=MUTATION_CHANCE) == 1 {
                *cell = rand::random();
            }
        }
    }

    pub fn step(&mut self) {
        let a = rand::random_range(0..NUM_PROGRAMS);
        let b = random_neighbour(a);

        // Concatenate.
        let mut program = Program::new(a, b, self);
        // Execute.
        program.execute();
        // Split.
        program.finish(self);
    }

    pub fn draw(&self, canvas: &Canvas) {
        for i in 0..NUM_PROGRAMS {
            let slice = &self.tokens[i * PROGRAM_SIZE..];
            let s_x = (i % SIMULATION_ROW_SIZE) * PROGRAM_ROW_SIZE;
            let s_y = (i / SIMULATION_ROW_SIZE) * PROGRAM_ROW_SIZE;
            for j in 0..PROGRAM_SIZE {
                let p_x = s_x + (j % PROGRAM_ROW_SIZE);
                let p_y = s_y + (j % PROGRAM_ROW_SIZE);
                canvas.draw_rectangle_v(
                    CELL_DIMENSIONS * Vector2::new(p_x as f32, p_y as f32),
                    CELL_DIMENSIONS,
                    cell_color(slice[j]),
                );
            }

            let origin = Vector2::new(s_x as f32, s_y as f32) * CELL_DIMENSIONS;
            let dimensions = CELL_DIMENSIONS
                * Vector2::new(
                    PROGRAM_ROW_SIZE as f32,
                    PROGRAM_ROW_SIZE as f32,
                );

            canvas.draw_rectangle_lines_ex(
                Rectangle {
                    x: origin.x,
                    y: origin.y,
                    width: dimensions.x,
                    height: dimensions.y,
                },
                CELL_DIMENSIONS.x / (PROGRAM_ROW_SIZE as f32),
                WHITE,
            );
        }
    }
}

const fn cell_color(cell: Token) -> Color {
    match cell as char {
        '<' => color(230, 25, 75),
        '>' => color(60, 180, 75),
        '{' => color(255, 225, 25),
        '}' => color(0, 130, 200),
        '-' => color(245, 130, 48),
        '+' => color(145, 30, 180),
        '.' => color(70, 240, 240),
        ',' => color(240, 50, 230),
        '[' => color(210, 245, 60),
        ']' => color(250, 190, 212),
        _ => color(0, 0, 0),
    }
}

fn random_other_cell(a: usize) -> usize {
    let mut b = rand::random_range(0..NUM_PROGRAMS);
    while b == a {
        b = rand::random_range(0..NUM_PROGRAMS);
    }
    b
}

fn random_neighbour(a: usize) -> usize {
    let x = a % SIMULATION_ROW_SIZE;
    let y = a / SIMULATION_ROW_SIZE;

    let mut neighbours = [0; 8];
    let mut count = 0;

    for (dx, dy) in [
        (-1, -1),
        (0, -1),
        (1, -1),
        (-1, 0),
        (1, 0),
        (-1, 1),
        (0, 1),
        (1, 1),
    ] {
        if let (Some(nx), Some(ny)) =
            (x.checked_add_signed(dx), y.checked_add_signed(dy))
        {
            if nx < SIMULATION_ROW_SIZE && ny < SIMULATION_ROW_SIZE {
                neighbours[count] = nx + (ny * SIMULATION_ROW_SIZE);
                count += 1;
            }
        }
    }

    neighbours[rand::random_range(0..count)]
}
