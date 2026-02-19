# OscarTutorials-GameTank

Educational tutorials for the [GameTank](https://gametank.zone) 8-bit game console, written in C for the [Oscar64](https://github.com/drmortalwombat/oscar64) compiler.

These are ports of [OscarTutorials](https://github.com/drmortalwombat/OscarTutorials) — a set of C64-targeted tutorials by [@drmortalwombat](https://github.com/drmortalwombat) — adapted for the GameTank's framebuffer-based hardware (no text mode, no VIC/SID chips, hardware blitter instead of sprites).

> **Note:** Building these tutorials requires the [`gametank-target`](https://github.com/sdwfrost/oscar64/tree/gametank-target) branch of Oscar64, which adds GameTank as a first-class compiler target (`-tm=gametank`). Pre-built `.gtr` ROM files are included in each tutorial directory for convenience.

## Tutorials

| # | Directory | Original | Concept |
|---|-----------|----------|---------|
| 1 | `0010_HelloColors` | 0010_HelloWorld | Fill screen with colored stripes |
| 2 | `0200_GamepadMove` | 0200_CursorMove | Move a box with gamepad d-pad |
| 3 | `0300_Labyrinth` | 0300_Labyrinth | Maze generation via recursive backtracking |
| 4 | `1000_ColorCycle` | 1000_BorderColor | Cycle background color each frame |
| 5 | `1310_MovingBox` | 1310_MovingSprite | Boxes moving downward with wrapping |
| 6 | `1320_BouncingBoxes` | 1320_ReflectingSprite | Boxes bouncing off screen edges |
| 7 | `1330_CollidingBoxes` | 1330_CollidingSprite | AABB collision detection between boxes |
| 8 | `1350_GravityBoxes` | 1350_GravitySprite | Gravity physics with floor bounce and damping |
| 9 | `1500_PixelCurve` | 1500_BitmapPixels | Parametric curve drawn pixel-by-pixel |
| 10 | `4010_FixPointCircle` | 4010_FixPointNumbers | Fixed-point vector rotation drawing a circle |
| 11 | `4250_SineTable` | 4250_CosinTable | Precomputed sine lookup table for circular motion |
| 12 | `4260_CordicCircle` | 4260_CosinCordic | CORDIC algorithm computing sin/cos with shifts and adds |

## Project Structure

```
OscarTutorials-GameTank/
├── build.sh                 # Build script
├── lib/
│   ├── gt.h                 # Shared GameTank helper library (header)
│   └── gt.c                 # Shared GameTank helper library (implementation)
├── 0010_HelloColors/
│   ├── hello.c              # Tutorial source
│   └── hello.gtr            # Pre-built 2MB ROM image
├── 0200_GamepadMove/
│   └── ...
└── ...
```

Each tutorial is a single `.c` file in its own directory. All tutorials share the `lib/gt.h` helper library, which provides:

- `gt_init()` — Initialize hardware (banking, audio, blitter, double-buffered framebuffer)
- `gt_flip()` — Swap display and draw framebuffer pages
- `gt_sync()` — Wait for vblank then flip (tear-free page swap)
- `gt_clear(color)` — Clear the screen with a solid color
- `gt_draw_box(x, y, w, h, color)` — Draw a filled rectangle via the hardware blitter
- `gt_wait_vblank()` — Wait for the next vertical blank
- `gt_read_gamepad()` — Read gamepad state as a 16-bit bitmask

## Prerequisites

- **Oscar64 compiler** with GameTank target support (`-tm=gametank`). Build from the [`gametank-target`](https://github.com/sdwfrost/oscar64/tree/gametank-target) branch:
  ```bash
  git clone -b gametank-target https://github.com/sdwfrost/oscar64.git
  cd oscar64
  cmake -B build -DCMAKE_BUILD_TYPE=Release
  cmake --build build
  ```

- **GameTank Emulator** (optional, for testing): [GameTankEmulator](https://github.com/clydeshaffer/GameTankEmulator)

The build script expects the following directory layout:
```
parent/
├── oscar64/          # Oscar64 compiler (with gametank-target branch)
├── GameTankEmulator/ # Emulator (optional)
└── OscarTutorials-GameTank/   # This repository
```

## Building

Build a single tutorial:
```bash
./build.sh 0010_HelloColors
```

Build all tutorials:
```bash
for d in [0-9]*/; do ./build.sh "${d%/}"; done
```

Run with no arguments to see available tutorials:
```bash
./build.sh
```

Each build produces a 2MB `.gtr` ROM file in the tutorial's directory.

## Running

Load the `.gtr` file in the GameTank emulator:
```bash
../GameTankEmulator/bin/GameTankEmulator 0010_HelloColors/hello.gtr
```

Default emulator key bindings:
| Key | GameTank Button |
|-----|----------------|
| Arrow keys | D-pad |
| Z / J | A |
| X / K | B |
| C / L | C |
| Enter | Start |

## GameTank Hardware Overview

The GameTank is an 8-bit console based on the WDC 65C02:

- **Display:** 128x128 pixel framebuffer, double-buffered
- **Graphics:** Hardware blitter for fast rectangle fills and sprite copies
- **Input:** SEGA Genesis-compatible gamepad
- **Memory:** 8KB RAM, 32KB VRAM (two 16KB pages), 2MB banked ROM
- **Audio:** Dedicated 6502 audio coprocessor

Unlike the C64, the GameTank has no text/character mode — all graphics are drawn to the framebuffer via the blitter or CPU writes.

## Credits

- Original [OscarTutorials](https://github.com/drmortalwombat/OscarTutorials) by [@drmortalwombat](https://github.com/drmortalwombat)
- [Oscar64 compiler](https://github.com/drmortalwombat/oscar64) by [@drmortalwombat](https://github.com/drmortalwombat)
- [GameTank](https://gametank.zone) console by Clyde Shaffer

## License

These tutorials follow the same licensing as the original [OscarTutorials](https://github.com/drmortalwombat/OscarTutorials).
