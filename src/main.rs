//! Main entrypoint program.

mod constants;
mod raylib;
mod sim;

use crate::{
    constants::{HEIGHT, WIDTH},
    raylib::*,
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

    while !window.should_close() {
        let canvas = window.draw(camera);
        canvas.clear_background(BLACK);
        canvas.draw_text("Hello!", 20, 20, 100, WHITE);
    }
}
