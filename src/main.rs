//! Main entrypoint program.

#[allow(dead_code)]
mod raylib;

mod bf;
mod constants;
mod state;
mod workers;

use std::sync::Mutex;

use crate::{
    bf::{ProgramId, Token},
    constants::{
        CELL_DIMENSIONS, HEIGHT, NUM_PROGRAMS, PROGRAM_ROW_SIZE,
        SIMULATION_ROW_SIZE, SIMULATION_SIZE, WIDTH,
    },
    raylib::*,
    state::State,
};

fn main() {
    let window = Window::new(WIDTH, HEIGHT, 60, "Hello, world!")
        .expect("Fix your stuff innit.");

    let camera = Camera2D {
        offset: Vector2::default(),
        target: Vector2::default(),
        rotation: 0.0,
        zoom: 1.0,
    };

    let mut state: Mutex<State> = Mutex::new(State::new());
    let mut display_tokens = [0; SIMULATION_SIZE];

    while !window.should_close() {
        workers::work_once(&mut state);

        // Make a local copy after locking the mutex to avoid r/w data races.
        {
            let mut state = state.lock().unwrap();
            display_tokens.copy_from_slice(&state.data);
        }

        let canvas = window.draw(camera);
        canvas.clear_background(BLACK);
        render_tokens(&display_tokens, &canvas);
    }
}

fn render_tokens(tokens: &[Token], canvas: &Canvas) {
    for i in 0..NUM_PROGRAMS {
        // A program is simply a contiguous block within the token stream; get
        // it.
        let program = &tokens[ProgramId::new(i).cell_range()];

        // Compute the coordinates for the top left of this program within the
        // simulation grid.
        let program_origin = Vector2::new(
            (i % SIMULATION_ROW_SIZE) as f32,
            (i / SIMULATION_ROW_SIZE) as f32,
        );

        // Translate to cells.
        let program_origin = program_origin
            * Vector2::new(PROGRAM_ROW_SIZE as f32, PROGRAM_ROW_SIZE as f32);

        for (j, cell) in program.iter().enumerate() {
            // Compute the coordinates of this cell within the program square.
            let cell_origin = Vector2::new(
                (j % PROGRAM_ROW_SIZE) as f32,
                (j / PROGRAM_ROW_SIZE) as f32,
            );

            // Translate by program origin to get location of this cell within
            // the simulation grid.
            let cell_origin = cell_origin + program_origin;

            // Scale to fit within the actual window.
            let cell_origin = cell_origin * CELL_DIMENSIONS;

            canvas.draw_rectangle_v(
                cell_origin,
                CELL_DIMENSIONS,
                cell_color(*cell),
            );
        }

        // // Draw grid lines to split up programs easily.

        // let origin = Vector2::new(s_x as f32, s_y as f32) * CELL_DIMENSIONS;
        // let dimensions = CELL_DIMENSIONS
        //     * Vector2::new(PROGRAM_ROW_SIZE as f32, PROGRAM_ROW_SIZE as f32);

        // canvas.draw_rectangle_lines_ex(
        //     Rectangle {
        //         x: origin.x,
        //         y: origin.y,
        //         width: dimensions.x,
        //         height: dimensions.y,
        //     },
        //     CELL_DIMENSIONS.x / (PROGRAM_ROW_SIZE as f32),
        //     DARKGRAY,
        // );
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
