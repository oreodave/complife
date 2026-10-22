//! Main entrypoint program.

#[allow(dead_code)]
mod raylib;

mod bf;
mod constants;
mod state;
mod workers;

use std::{
    sync::{
        Condvar, Mutex,
        atomic::{AtomicBool, Ordering},
    },
    thread,
    time::{Duration, Instant},
};

use crate::{
    bf::{ProgramId, Token},
    constants::{
        CELL_DIMENSIONS, HEIGHT, MUTATION_CHANCE, NUM_PROGRAMS,
        PROGRAM_ROW_SIZE, SIMULATION_ROW_SIZE, SIMULATION_SIZE, WIDTH,
        WORKER_COUNT,
    },
    raylib::*,
    state::State,
    workers::{mutate, work_once},
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

    // Generalised shared state
    let state: Mutex<State> = Mutex::new(State::new());
    let changed = Condvar::new();
    let alive = AtomicBool::new(true);

    let mut display_tokens = vec![0; SIMULATION_SIZE];

    println!(
        "Spawning {} workers and 1 mutator thread.",
        WORKER_COUNT - 1
    );

    thread::scope(|scope| {
        if MUTATION_CHANCE > 0 {
            scope.spawn(|| {
                let delta = Duration::from_millis(16);
                let mut next = Instant::now() + delta;
                while alive.load(Ordering::Relaxed) {
                    thread::sleep(
                        next.saturating_duration_since(Instant::now()),
                    );

                    if !alive.load(Ordering::Relaxed) {
                        break;
                    }

                    mutate(&state, &changed);
                    next += delta;
                }
            });
        }

        for _ in 0..(WORKER_COUNT - 1) {
            scope.spawn(|| {
                while alive.load(Ordering::Relaxed) {
                    work_once(&state, &changed);
                }
            });
        }

        while !window.should_close() {
            // Make a local copy after locking the mutex to avoid r/w data
            // races.
            {
                let state = state.lock().unwrap();
                display_tokens.copy_from_slice(&state.data);
            }

            let canvas = window.draw(camera);
            canvas.clear_background(BLACK);
            render_tokens(&display_tokens, &canvas);
        }

        alive.store(false, Ordering::Relaxed);
    });
}

fn render_tokens(tokens: &[Token], canvas: &Canvas) {
    let program_row_size =
        Vector2::new(PROGRAM_ROW_SIZE as f32, PROGRAM_ROW_SIZE as f32);
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
        let program_origin = program_origin * program_row_size;

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

        // Draw grid lines to split up programs easily.

        let origin = program_origin * CELL_DIMENSIONS;
        let dimensions = CELL_DIMENSIONS * program_row_size;

        canvas.draw_rectangle_lines_ex(
            Rectangle {
                x: origin.x,
                y: origin.y,
                width: dimensions.x,
                height: dimensions.y,
            },
            CELL_DIMENSIONS.x / (PROGRAM_ROW_SIZE as f32),
            DARKGRAY,
        );
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
