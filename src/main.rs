//! Main entrypoint program.

#[allow(dead_code, clippy::pedantic)]
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

pub const MOVEMENT_SPEED: f32 = 5.0;
pub const ZOOM_SPEED: f32 = 0.1;
pub const ZOOM_MAX: f32 = 5.0;

fn main() {
    let window = Window::new(WIDTH, HEIGHT, 60, "Hello, world!")
        .expect("Fix your stuff innit.");

    let mut camera = Camera2D {
        offset: Vector2::default(),
        target: Vector2::default(),
        rotation: 0.0,
        zoom: 1.0,
    };

    // Generalised shared state
    let state: Mutex<State> = Mutex::new(State::new());
    let changed = Condvar::new();
    let paused = AtomicBool::new(false);
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
                    } else if paused.load(Ordering::Relaxed) {
                        continue;
                    }

                    mutate(&state, &changed);
                    next += delta;
                }
            });
        }

        for _ in 0..(WORKER_COUNT - 1) {
            scope.spawn(|| {
                while alive.load(Ordering::Relaxed) {
                    if paused.load(Ordering::Relaxed) {
                        continue;
                    }
                    work_once(&state, &changed);
                }
            });
        }

        while !window.should_close() {
            if window.key_pressed(KEY_SPACE) {
                let invert = !paused.load(Ordering::Relaxed);
                paused.store(invert, Ordering::Relaxed);
            }

            if window.key_down(KEY_W) {
                camera.target.y -= MOVEMENT_SPEED;
            }

            if window.key_down(KEY_S) {
                camera.target.y += MOVEMENT_SPEED;
            }

            if window.key_down(KEY_A) {
                camera.target.x -= MOVEMENT_SPEED;
            }

            if window.key_down(KEY_D) {
                camera.target.x += MOVEMENT_SPEED;
            }

            if window.key_pressed(KEY_Z) {
                camera.zoom += ZOOM_SPEED;
                camera.zoom = camera.zoom.clamp(1.0, ZOOM_MAX);
            }

            if window.key_pressed(KEY_X) {
                camera.zoom -= ZOOM_SPEED;
                camera.zoom = camera.zoom.clamp(1.0, ZOOM_MAX);
            }

            if window.key_pressed(KEY_PERIOD) {
                camera.offset = Vector2::default();
                camera.target = Vector2::default();
                camera.zoom = 1.0;
            }

            {
                let state = state.lock().unwrap();
                display_tokens.copy_from_slice(&state.data);
            }

            let canvas = window.draw(camera);
            canvas.clear_background(BLACK);
            render_tokens(&display_tokens, &canvas);
        }

        paused.store(false, Ordering::Relaxed);
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

        // Draw each cell individually
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
