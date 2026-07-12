# Infinity Creator

A parametric 3D CAD application modeled on CATIA V5's Part Design + Sketcher workflow, for individual part modeling with 3D-printable output. Single-part focus - no assembly functionality.

Built with C++17, Qt 6.8.3 (MSVC 2022 64-bit), OpenCASCADE 7.9.3, and CMake + vcpkg.

## Status: Phase 3 - files & settings

Phase 3 adds the file lifecycle: save/open .infinity files (versioned JSON of the feature history - see docs/FILE_FORMAT.md), Save As / Save As Copy / Save All, dirty tracking with save prompts on close and exit, and File > Document History (recent files). Exports now include 3MF (self-contained OPC writer, no external zip library) alongside binary STL, both honoring the mesh-quality setting. Settings arrive on both scopes: Edit > Options (units mm/inch, export mesh quality, author name - application-wide) and File > Document Properties (per-file overrides with inherit semantics, stored in the .infinity). A first-run setup wizard captures initial defaults. All definition dialogs display and accept values in the effective unit; storage stays millimeters.

Phase 2 recap: the specification tree (left dock): double-click a feature to edit its definition, right-click for Edit / Suppress / Delete. Failed features stay in the tree marked red (CATIA behavior) with the error as tooltip. Full undo/redo (Ctrl+Z / Ctrl+Y) covers add, delete, edit, and suppress, per document. Fillet and Chamfer (all edges) are live. The full 7-tab menu system is in place - File, Edit, View, Insert, Workspace, Window, Help - with future items visible but disabled, each tagged with the phase it arrives in.

Phase 1 recap: feature-history engine. In the Part Design workbench you can insert five primitives (Box, Cylinder, Sphere, Cone, Torus), each opening a CATIA-style definition dialog with dimensions, placement (position + rotation), and a boolean operation (new body / add / subtract / intersect). Features form an ordered history that is replayed on every recompute - the foundation the feature tree, sketcher, and file formats build on.

Coming in later phases: feature tree dock (edit/suppress/delete), fillet/chamfer, .infinity save/open, 3MF export, undo/redo, then the Sketcher workbench (constraint solver, Pad/Pocket/Revolve).

## Prerequisites

- Visual Studio 2022 with the C++ workload
- Qt 6.8.3 MSVC 2022 64-bit (at C:\Qt\6.8.3\msvc2022_64)
- vcpkg at C:\vcpkg with: .\vcpkg install opencascade:x64-windows

If your paths differ, edit CMakePresets.json.

## Build & run

```
run.bat
```

GUI-subsystem exe; diagnostics append to debug.log next to the exe (Get-Content build\debug.log -Wait to tail).

## Try it

File > New Part, then in the Part Design toolbar: insert a Box (50x50x50, New body). Insert a Cylinder, set Operation to Subtract, position it at x=25 y=25 - you've cut a hole. Left-drag rotates, middle-drag pans, scroll zooms. File > Export STL writes a printable file.

## Architecture

MainWindow owns a QMdiArea and one shared instance of each Workspace (workbench). Each open part is a Document in a QMdiSubWindow, owning its own OcctViewport and its PartModel - an ordered list of Feature objects. PartModel::recompute() replays the history: each feature's apply() takes the part built so far and returns it with the feature applied (primitives transform themselves into place and fuse/cut/intersect via BRepAlgoAPI). Recompute failures are caught per feature (Standard_Failure) and reported. Every feature carries toJson()/fromJson() from birth - the .infinity format (JSON history replay) plugs straight into this in the I/O phase.
