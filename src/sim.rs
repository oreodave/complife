//! Complete program simulation.

use crate::{
    bf::Token,
    constants::{
        CELL_DIMENSIONS, MUTATION_CHANCE, NUM_PROGRAMS, PROGRAM_ROW_SIZE,
        PROGRAM_SIZE, SIMULATION_ROW_SIZE, SIMULATION_SIZE,
    },
    raylib::{Canvas, Color, Rectangle, Vector2, WHITE, color},
};

/// Generalised simulation data.
#[derive(Debug)]
pub struct Simulation {
    pub tokens: Box<[Token]>,
}

impl Simulation {
    /// Construct a new [`Simulation`].
    pub fn new() -> Self {
        let mut tokens = vec![0; SIMULATION_SIZE].into_boxed_slice();
        rand::fill(&mut tokens[..]);
        Self { tokens }
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
