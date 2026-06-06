# Flood Fill vs Boundary Fill Contrast Visualizer

A C++ OpenGL/GLUT application that visually demonstrates the difference between **Flood Fill** and **Boundary Fill** algorithms through interactive grid-based simulations. The program provides multiple test shapes and animated filling behavior, allowing users to compare how each algorithm reacts to boundaries and interior color variations.

## Features

* Interactive visualization of Flood Fill and Boundary Fill algorithms
* Animated fill process for easier observation
* Three predefined test shapes:

  * Simple Closed Room
  * Homogeneous Patch Box
  * Two-Tone Divided Core
* Mouse-based fill seed selection
* Real-time algorithm switching
* Full-screen visualization with responsive coordinate handling

## Controls

| Key                  | Action                       |
| -------------------- | ---------------------------- |
| **F**                | Switch to Flood Fill mode    |
| **B**                | Switch to Boundary Fill mode |
| **C**                | Reset current shape          |
| **1**                | Load Simple Closed Room      |
| **2**                | Load Homogeneous Patch Box   |
| **3**                | Load Two-Tone Divided Core   |
| **Left Mouse Click** | Select fill starting point   |
| **ESC**              | Exit application             |

## Requirements

* C++17 or later
* OpenGL
* GLUT / FreeGLUT

## Build and Run

### Linux

Install FreeGLUT and OpenGL development libraries:

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install freeglut3-dev

g++ flood_boundary_fill.cpp -o visualizer -lGL -lGLU -lglut
./visualizer
```

### macOS

Using Homebrew:

```bash
brew install freeglut

g++ flood_boundary_fill.cpp -o visualizer \
-I/opt/homebrew/include \
-L/opt/homebrew/lib \
-lglut -framework OpenGL

./visualizer
```

### Windows

#### Using MinGW and FreeGLUT

Install FreeGLUT and ensure MinGW is available in your PATH.

```bash
g++ flood_boundary_fill.cpp -o visualizer.exe -lfreeglut -lopengl32 -lglu32
./visualizer.exe
```

#### Using Visual Studio

1. Install FreeGLUT.
2. Add FreeGLUT include and library directories to the project settings.
3. Link the following libraries:

   * `freeglut.lib`
   * `opengl32.lib`
   * `glu32.lib`
4. Build and run the project.

## Educational Purpose

This project is designed for computer graphics courses and demonstrations. It highlights the key difference between the two algorithms:

* **Flood Fill** expands only through connected regions of the same target color.
* **Boundary Fill** expands until it encounters a boundary color, regardless of interior color variations.

By experimenting with the provided shapes, users can observe how each algorithm behaves under different conditions.
