pub mod structs;

use std::fs;
use std::io::{BufRead, BufReader};
use std::process::Command;
use tauri::Emitter;
use tauri::{State, Window};

fn cfg() -> structs::Configs {
    use structs::Configs;
    let config = Configs {
        bin_path: String::from("/Users/rohit/rohit-project-work/remucomp/bin/main"),
        file_path: String::from("/Users/rohit/rohit-project-work/remucomp/samples/ide-test.rasm"),
    };

    config
}

#[tauri::command]
fn write_file(contents: String) -> Result<(), String> {
    let config = cfg();
    fs::write(&config.file_path, contents).map_err(|e| e.to_string())
}

#[tauri::command]
fn read_file() -> Result<String, String> {
    let config = cfg();
    fs::read_to_string(&config.file_path).map_err(|e| e.to_string())
}

#[tauri::command]
fn run_program(config: structs::Configs) -> Result<String, String> {
    Command::new(&config.bin_path)
        .output()
        .map_err(|e| e.to_string())
        .and_then(|out| {
            if out.status.success() {
                Ok(String::from_utf8_lossy(&out.stdout).to_string())
            } else {
                Err(String::from_utf8_lossy(&out.stderr).to_string())
            }
        })
}

#[tauri::command]
fn run_binary(window: tauri::Window, contents: String) -> Result<(), String> {
    let config = cfg();

    // Save file before build
    fs::write(&config.file_path, contents).map_err(|e| e.to_string())?;

    // Run build command, pipe output to frontend
    let build_output = Command::new(&config.bin_path)
        .arg("build")
        .arg(&config.file_path)
        .stdout(std::process::Stdio::piped())
        .stderr(std::process::Stdio::piped())
        .spawn()
        .map_err(|e| format!("Build failed to start: {}", e))?;

    // Wait for build to finish and capture output
    let output = build_output
        .wait_with_output()
        .map_err(|e| format!("Failed to wait on build process: {}", e))?;

    if let Ok(stdout) = String::from_utf8(output.stdout.clone()) {
        for line in stdout.lines() {
            if (!(String::from(line).trim() == "")) {
                let _ = window.emit("vm-output", format!("[BUILD] {}", line));
            }
        }
    }

    if let Ok(stderr) = String::from_utf8(output.stderr.clone()) {
        for line in stderr.lines() {
            if (!(String::from(line).trim() == "")) {
                let _ = window.emit("vm-output", format!("[BUILD] {}", line));
            }
        }
    }

    if !output.status.success() {
        return Ok(());
        // return Err("Build command failed".into());
    }

    // Run binary, pipe output to frontend
    let mut child = Command::new(&config.bin_path)
        .arg("run")
        .arg("/Users/rohit/rohit-project-work/remucomp/out/a.bin")
        .stdout(std::process::Stdio::piped())
        .spawn()
        .map_err(|e| format!("Failed to start process: {}", e))?;

    let stdout = child.stdout.take().ok_or("Failed to capture stdout")?;
    std::thread::spawn(move || {
        let reader = BufReader::new(stdout);
        for line in reader.lines().flatten() {
            let _ = window.emit("vm-output", line);
        }
    });

    Ok(())
}

// #[tauri::command]
// fn run_binary(window: tauri::Window) -> Result<(), String> {
//     let config = cfg();
//
//     let build_status = Command::new(&config.bin_path)
//         .arg("build")
//         .arg(&config.file_path)
//         .status()
//         .map_err(|e| format!("Build failed to start: {}", e))?;
//
//     if !build_status.success() {
//         return Err("Build command failed".into());
//     }
//
//     let mut child = Command::new(&config.bin_path)
//         .arg("run")
//         .arg("/Users/rohit/rohit-project-work/remucomp/out/a.bin")
//         .stdout(std::process::Stdio::piped())
//         .spawn()
//         .map_err(|e| format!("Failed to start process: {}", e))?;
//
//     let stdout = child.stdout.take().ok_or("Failed to capture stdout")?;
//     std::thread::spawn(move || {
//         use std::io::{BufRead, BufReader};
//         let reader = BufReader::new(stdout);
//         for line in reader.lines().flatten() {
//             let _ = window.emit("vm-output", line);
//         }
//     });
//
//     Ok(())
// }

#[cfg_attr(mobile, tauri::mobile_entry_point)]
pub fn run() {
    tauri::Builder::default()
        .invoke_handler(tauri::generate_handler![
            write_file,
            run_program,
            read_file,
            run_binary
        ])
        .setup(|app| {
            if cfg!(debug_assertions) {
                app.handle().plugin(
                    tauri_plugin_log::Builder::default()
                        .level(log::LevelFilter::Info)
                        .build(),
                )?;
            }
            Ok(())
        })
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
