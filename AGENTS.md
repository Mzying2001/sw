# AGENTS.md

This file provides guidance to AI coding agents when working with code in this repository.

## Project Overview

**SimpleWindow (sw)** is a C++14 Windows desktop GUI framework providing OOP encapsulation of Windows standard controls. Inspired by WPF. Zero external dependencies — only Windows SDK (user32, gdi32, comctl32).

All code is in `namespace sw`. Source files are UTF-8 encoded. Wide-character strings (`L"..."`) are used throughout (UNICODE defined).

## Directory Layout

- `sw/inc/` — Header files (public API)
- `sw/src/` — Implementation files
- `examples/` — Example apps demonstrating features
- `single_header/` — Auto-generated single-file distribution
- `vs/` — Visual Studio project files
- `docs/` — Doxygen HTML output

## Build Commands

### Build the library (CMake)

```bash
cmake -S sw -B build
cmake --build build
```

### Build examples

```bash
cmake -S examples -B examples/build
cmake --build examples/build
```

### Generate single-header distribution

```bash
python single_header/build.py
```

GitHub Action regenerates `single_header/` automatically; unless explicitly requested, **do not** run this script or edit generated files manually.

### Visual Studio

The `vs/sw.vcxproj` project has 8 configurations: Debug/Release × MT/default × Win32/x64 (toolset v143).

## Code Conventions

- **Naming**: PascalCase for classes, methods, properties, enums. `_camelCase` for private fields.
- **Style**: 4-space indent, clang-format (Microsoft style), `#pragma once` include guards
- **Docs**: Doxygen comments in Chinese

## Git Commit Conventions

- Use Conventional Commits: `<type>(<scope>): <summary>`
- Common types: `feat`, `fix`, `refactor`, `build`, `docs`, `test`, `chore`
- Use an optional scope for the affected class or file, such as `Button`, `CMakeLists.txt`, or no scope
- Write concise Chinese summaries, matching the existing commit history
- For non-trivial changes, include a Chinese body explaining the reason and implementation/fix approach
