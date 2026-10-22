//! Main entrypoint program.

mod bf;
mod constants;
mod raylib;
mod simulation;

use crate::{
    constants::{HEIGHT, WIDTH},
    raylib::*,
    simulation::Simulation,
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

    let mut sim = Simulation::new();

    while !window.should_close() {
        sim.mutate();
        sim.step();
        let canvas = window.draw(camera);
        canvas.clear_background(BLACK);
        sim.draw(&canvas);
    }
}
