//! Raylib integration.
//!
//! Instead of using some other guys crate, just write bindings to Raylib for
//! the things we need.  Please note that the raylib source we're building
//! against is located at thirdparty/.

use std::{
    ffi::CString,
    ops::{Add, Mul},
};

#[repr(C)]
#[derive(Clone, Copy, Debug, Default)]
pub struct Vector2 {
    pub x: f32,
    pub y: f32,
}

impl Vector2 {
    pub const fn new(x: f32, y: f32) -> Self {
        Self { x, y }
    }
}

impl Add for Vector2 {
    type Output = Self;

    fn add(self, other: Self) -> Self::Output {
        Self::new(self.x + other.x, self.y + other.y)
    }
}

impl Mul for Vector2 {
    type Output = Self;

    fn mul(self, other: Self) -> Self::Output {
        Self::new(self.x * other.x, self.y * other.y)
    }
}

#[repr(C)]
#[derive(Clone, Copy, Debug)]
pub struct Color {
    pub r: u8,
    pub g: u8,
    pub b: u8,
    pub a: u8,
}

pub const fn color(r: u8, g: u8, b: u8) -> Color {
    Color { r, g, b, a: 255 }
}

pub const BLACK: Color = color(0, 0, 0);
pub const WHITE: Color = color(255, 255, 255);
pub const DARKGRAY: Color = color(80, 80, 80);

#[repr(C)]
#[derive(Clone, Copy)]
pub struct Rectangle {
    pub x: f32,
    pub y: f32,
    pub width: f32,
    pub height: f32,
}

#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct Camera2D {
    pub offset: Vector2,
    pub target: Vector2,
    pub rotation: f32,
    pub zoom: f32,
}

pub const KEY_SPACE: i32 = 32;
pub const KEY_W: i32 = 87;
pub const KEY_S: i32 = 83;
pub const KEY_A: i32 = 65;
pub const KEY_D: i32 = 68;
pub const KEY_Z: i32 = 90;
pub const KEY_X: i32 = 88;
pub const KEY_PERIOD: i32 = 46;
pub const KEY_COMMA: i32 = 44;

pub struct Window;
pub struct Canvas<'a>(&'a Window, Camera2D);

impl Window {
    pub fn new(
        width: i32,
        height: i32,
        fps: i32,
        title: &str,
    ) -> Result<Self, String> {
        let title =
            CString::new(title).expect("window title cannot contain NUL");
        unsafe {
            InitWindow(width, height, title.as_ptr());
            if !IsWindowReady() {
                return Err("raylib could not create a window".into());
            }
            SetTargetFPS(fps);
        }
        Ok(Self)
    }

    pub fn should_close(&self) -> bool {
        unsafe { WindowShouldClose() }
    }

    pub fn frame_time(&self) -> f32 {
        unsafe { GetFrameTime() }
    }

    pub fn key_pressed(&self, key: i32) -> bool {
        unsafe { IsKeyPressed(key) }
    }

    pub fn key_down(&self, key: i32) -> bool {
        unsafe { IsKeyDown(key) }
    }

    pub fn mouse_button_pressed(&self, mouse_button: i32) -> bool {
        unsafe { IsMouseButtonPressed(mouse_button) }
    }

    pub fn mouse_position(&self) -> Vector2 {
        unsafe { GetMousePosition() }
    }

    pub const fn draw(&self, camera: Camera2D) -> Canvas<'_> {
        Canvas(self, camera)
    }
}

impl<'a> Canvas<'a> {
    fn new(win: &'a Window, camera: Camera2D) -> Self {
        unsafe { BeginDrawing() };
        unsafe { BeginMode2D(camera) };
        Self(win, camera)
    }

    pub fn clear_background(&self, color: Color) {
        unsafe { ClearBackground(color) }
    }

    pub fn draw_rectangle_v(
        &self,
        pos: Vector2,
        dimensions: Vector2,
        color: Color,
    ) {
        unsafe { DrawRectangleV(pos, dimensions, color) }
    }

    pub fn draw_rectangle_lines_ex(
        &self,
        rect: Rectangle,
        thickness: f32,
        color: Color,
    ) {
        unsafe { DrawRectangleLinesEx(rect, thickness, color) }
    }

    pub fn draw_text(
        &self,
        text: &str,
        x: i32,
        y: i32,
        font_size: i32,
        color: Color,
    ) {
        let s = CString::new(text).expect("No null bytes in my string please");
        unsafe {
            DrawText(s.as_ptr(), x, y, font_size, color);
        }
    }
}

impl Drop for Window {
    fn drop(&mut self) {
        unsafe { CloseWindow() };
    }
}

impl Drop for Canvas<'_> {
    fn drop(&mut self) {
        unsafe { EndMode2D() };
        unsafe { EndDrawing() };
    }
}

unsafe extern "C" {
    fn InitWindow(width: i32, height: i32, title: *const std::ffi::c_char);
    fn CloseWindow();
    fn IsWindowReady() -> bool;
    fn WindowShouldClose() -> bool;
    fn SetTargetFPS(fps: i32);
    fn GetFrameTime() -> f32;
    fn IsKeyPressed(key: i32) -> bool;
    fn IsKeyDown(key: i32) -> bool;
    fn IsMouseButtonPressed(button: i32) -> bool;
    fn GetMousePosition() -> Vector2;
    fn BeginDrawing();
    fn BeginMode2D(camera: Camera2D);
    fn EndMode2D();
    fn EndDrawing();
    fn ClearBackground(color: Color);
    fn DrawRectangleV(pos: Vector2, dim: Vector2, color: Color);
    fn DrawRectangleLinesEx(rect: Rectangle, thickness: f32, color: Color);
    fn DrawText(
        text: *const std::ffi::c_char,
        x: i32,
        y: i32,
        size: i32,
        color: Color,
    );
}
