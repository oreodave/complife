use std::path::PathBuf;

fn main() {
    let lib = PathBuf::from(env!("CARGO_MANIFEST_DIR"))
        .join("thirdparty/raylib-6.2_linux_amd64/lib");

    println!("cargo::rustc-link-search=native={}", lib.display());
    println!("cargo::rustc-link-lib=dylib=raylib");
}
