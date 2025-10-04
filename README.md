# MiniRunner

Arcade 2D runner built with C and SDL2.

- Fast procedural level generation
- Multiple enemy types: Patrol, Flying (headbutt kill), Jumper, Shooter (bullets), Spike (non-stompable)
- Crumbling platforms with fade-out and faster demo pacing
- DEMO mode to showcase mechanics quickly

## Build

Requirements:
- SDL2 (macOS: `brew install sdl2`)

Build targets:
```bash
make           # builds main executable: runner
make runner_demo  # builds demo executable: runner_demo
```

## Run
```bash
./runner
# or demo (more enemies, more crumble, higher difficulty):
./runner_demo
```

Environment options:
- `START_DIFFICULTY=<int>`: set initial difficulty (default: 1). Example:
  ```bash
  START_DIFFICULTY=4 ./runner
  ```
- `DEMO_MODE=1`: increase density and guarantee variety. Example:
  ```bash
  DEMO_MODE=1 START_DIFFICULTY=5 ./runner
  ```

## Controls
- Left/Right: A/D or Arrow keys
- Jump: Space
- Quit: ESC

## Gameplay notes
- Flying enemies are killed by headbutt (jumping up into them).
- Jumpers avoid leaping off edges.
- Spike enemies cannot be stomped.
- Crumbling platforms fade out and disappear even if you already left them.

## Project structure
```
include/        # headers
src/            # source files
assets/         # placeholder for art/audio
Makefile        # build rules
```

## License
MIT — see LICENSE.
