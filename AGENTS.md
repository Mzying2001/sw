# AGENTS.md

This file provides guidance to AI coding agents when working with code in this repository.

## Project Overview

**SimpleWindow (sw)** is a C++14 Windows desktop GUI framework providing OOP encapsulation of Windows standard controls. Inspired by WPF. Zero external dependencies — only Windows SDK (user32, gdi32, comctl32).

All code is in `namespace sw`. Source files are UTF-8 encoded. Wide-character strings (`L"..."`) are used throughout (UNICODE defined).

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

### Visual Studio

The `vs/sw.vcxproj` project has 8 configurations: Debug/Release × MT/default × Win32/x64 (toolset v143).

## Architecture

### Class Hierarchy

```
WndBase              — Win32 HWND wrapper
  └─ UIElement       — Base UI class (layout, events, properties)
      ├─ Control     — 30+ standard controls (Button, TextBox, ComboBox, etc.)
      ├─ PanelBase   — Container base (Grid, StackPanel, DockPanel, Canvas, etc.)
      ├─ MenuBase    — Menu system
      ├─ HwndHost    — Hosts external HWNDs
      └─ Layer
          └─ Window  — Top-level window
```

### Core Systems

- **Property System** (`Property.h`, `Macros.h`): SFINAE-based C#-like properties declared via macros (`SW_DEFINE_PROPERTY`, `SW_DEFINE_READONLY_PROPERTY`, `SW_DEFINE_NOTIFY_PROPERTY`, `SW_DEFINE_EXPR_PROPERTY`)
- **Delegates** (`Delegate.h`): Type-erased function wrappers supporting lambdas, std::function, member functions
- **Routed Events** (`Event.h`): WPF-style event bubbling through the control hierarchy
- **Data Binding** (`Binding.h`, `DataBinding.h`): Two-way property binding with value converters (`Converters.h`)
- **Reflection** (`Reflection.h`): Runtime type information, `Variant` type, dynamic objects
- **Layouts** (`ILayout.h`): GridLayout, StackLayout, DockLayout, CanvasLayout, WrapLayout, UniformGridLayout, FillLayout

### Directory Layout

- `sw/inc/` — Header files (public API). `SimpleWindow.h` is the master include.
- `sw/src/` — Implementation files
- `examples/` — 10 example apps demonstrating features
- `single_header/` — Auto-generated `sw_all.h` + `sw_all.cpp` (via `build.py` topological sort)
- `vs/` — Visual Studio project files
- `docs/` — Doxygen HTML output

## Code Conventions

- **Naming**: PascalCase for classes, methods, properties, enums. `_camelCase` for private fields.
- **Style**: 4-space indent, clang-format (Microsoft style), `#pragma once` include guards
- **Docs**: Doxygen comments, bilingual (Chinese + English)

## Compiler Notes

- MSVC: `/utf-8` or `/source-charset:utf-8` required
- GCC/Clang: `-finput-charset=UTF-8` required
- CMake minimum: 3.10
- Targets Windows Vista+, SDK 10.0
