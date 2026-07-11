# Infinity Creator

A desktop 3D CAD modeling program for designing solid parts and exporting them as STL files ready for 3D printing.

Built in C++ with [Qt 6](https://www.qt.io/) for the user interface and [OpenCASCADE (OCCT)](https://dev.opencascade.org/) as the geometry kernel.

> **Status: early development.** The application currently boots the main window, renders an OpenCASCADE 3D viewport, and runs an end-to-end "tracer bullet" that builds a solid box and exports it to STL. The sketching and part-design workspaces are being scaffolded — most toolbar actions are placeholders for now.

## Features

- **Qt 6 desktop UI** — a main window with File/Edit menus, dockable sketcher toolbars (lines, circles, constraints), and a status bar.
- **Embedded 3D viewport** — an OpenCASCADE `V3d_View` rendered inside a native Qt widget ([`OcctViewport`](src/OcctViewport.h)).
- **Geometry kernel layer** — all OCCT access is isolated behind a single [`Geometry`](src/Geometry.h) namespace, so UI and feature code never touch OpenCASCADE directly.
- **STL export** — tessellates a B-rep solid into triangles and writes a printable `.stl` file, ready to drop into a slicer.
- **Workspace model** — a `Document` tracks the active workspace (Part Design, Sketcher, Assembly), laying the groundwork for a multi-mode CAD workflow.

## Architecture

The codebase is organized to keep the geometry kernel cleanly separated from the UI:

| File | Responsibility |
| --- | --- |
| [`src/main.cpp`](src/main.cpp) | Application entry point; boots Qt, shows the main window, runs the STL tracer bullet |
| [`src/MainWindow.*`](src/MainWindow.h) | Menus, toolbars, and status bar |
| [`src/OcctViewport.*`](src/OcctViewport.h) | OpenCASCADE 3D view hosted in a Qt widget |
| [`src/Geometry.*`](src/Geometry.h) | The **only** module that calls OpenCASCADE — solid modeling and STL export |
| [`src/Document.*`](src/Document.h) | Per-document state and the active workspace |
| [`src/Workspace.*`](src/Workspace.h) | Common interface every workspace implements (`activate` / `deactivate`) |
| [`src/SketcherWorkspace.*`](src/SketcherWorkspace.h) | Sketching mode (in progress) |
| [`src/PartDesignWorkspace.*`](src/PartDesignWorkspace.h) | Part-design / feature mode (in progress) |

## Requirements

The project currently targets **Windows x64** with the following toolchain:

- **Visual Studio 2022** (MSVC, x64)
- **CMake** ≥ 3.21
- **Qt 6.8** (Widgets) — installed via the [Qt online installer](https://www.qt.io/download-qt-installer)
- **OpenCASCADE** — installed via [vcpkg](https://vcpkg.io/)

The included [`CMakePresets.json`](CMakePresets.json) assumes these default install locations:

| Dependency | Expected path |
| --- | --- |
| vcpkg toolchain | `C:/vcpkg/scripts/buildsystems/vcpkg.cmake` |
| Qt 6.8.3 (MSVC 2022, x64) | `C:/Qt/6.8.3/msvc2022_64` |

Adjust the paths in `CMakePresets.json` if your installs live elsewhere.

## Building

Install OpenCASCADE with vcpkg:

```powershell
vcpkg install opencascade:x64-windows
```

Then configure and build with the bundled preset:

```powershell
# Configure (uses the windows-default preset)
cmake --preset windows-default

# Build
cmake --build build --config Debug
```

On Windows the build automatically runs `windeployqt` to copy the required Qt runtime DLLs next to the executable.

## Running

Use the convenience script, which builds and then launches the app:

```powershell
.\run.bat
```

Or run the executable directly after building:

```powershell
.\build\Debug\CADModeler.exe
```

On launch the app opens the main window and writes a 50 × 30 × 10 test box to `output/infinity_test_box.stl` — open it in your slicer to confirm the modeling → export pipeline works end to end.

## Project layout

```
3D-CAD-Modeling-Program/
├─ CMakeLists.txt        # Build definition
├─ CMakePresets.json     # Windows MSVC + vcpkg preset
├─ run.bat               # Build + run helper
├─ src/                  # C++ source
├─ resources/            # App icon (.ico/.png/.svg) and Qt resource files
└─ output/               # Generated STL exports (git-ignored)
```

## License

No license has been specified for this project yet.
