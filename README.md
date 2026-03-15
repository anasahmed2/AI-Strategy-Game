# AI Strategy Game

A C++/SFML top-down RTS prototype with ECS architecture, player/enemy factions, worker economy, production, combat, and AI-driven enemy behavior.

## Tech Stack

### Core Technologies

- Language: C++17
- Architecture: custom ECS (Entity-Component-System)
- Game loop: custom engine loop (input, update, render)

### Libraries Used

- SFML 2.5
	- sfml-graphics: 2D rendering, shapes, text
	- sfml-window: window creation and input events
	- sfml-system: timing/utilities
	- sfml-audio: procedural sound effects

### AI and Gameplay Systems

- Behavior Trees (Selector/Sequence/Condition/Action)
- Finite State Machine (FSM)
- Blackboard memory for AI state sharing
- A* grid pathfinding (Manhattan heuristic)

### Build and Tooling

- Build system: CMake
- Compiler target: C++17
- Primary runtime/dev environment: WSL (Ubuntu on Windows)
- Renderer target: SFML desktop window (1200x800)

## What This Game Has

- Player units: Worker, Soldier, Tank, Scout
- Player buildings: Base, Turret
- Economy loop: Worker gathers gold from mines and returns to base
- Unit production from Base using hotkeys
- Enemy faction with autonomous unit behavior and periodic reinforcements
- A* pathfinding and waypoint movement
- Selection system: click and drag-box
- Fog of war and HUD/debug overlays

## Run In WSL (Recommended)

From WSL at project root:

```bash
cd /mnt/c/Users/Anas/AI-Strategy-Game
cmake -S . -B build
cmake --build build -j$(nproc)
export DISPLAY=:0
./build/Game/AIStrategyGame
```

If already configured once, you can skip configure:

```bash
cmake --build build -j$(nproc)
export DISPLAY=:0
./build/Game/AIStrategyGame
```

## Quick Controls

### Mouse

- Left click: select one unit/building
- Left drag: box-select player units
- Right click on ground: move selected units
- Right click on enemy: attack
- Right click on mine with worker selected: gather
- Shift + right click: defend/hold point

### Keyboard

- B then left click: place Base (costs gold)
- T then left click: place Turret (costs gold)
- Select a player Base, then:
	- 1: produce Worker (75g)
	- 2: produce Soldier (120g)
	- 3: produce Tank (220g)
	- 4: produce Scout (140g)

## Core Gameplay Loop

1. Select a Worker.
2. Right click a Resource Mine to start gathering.
3. Worker collects gold and auto-returns to nearest friendly Base.
4. Gold is added to your faction bank.
5. Select Base and press 2/3/4 to produce combat units.
6. Use army to pressure enemy units/base.

## Start-of-Match Setup

- Top-left: player base + starting units
- Bottom-right: enemy base + starting units
- Center/lanes: resource mines and obstacles

## Troubleshooting

### Worker does not gather or seems stuck

- Make sure a Worker is selected (yellow unit) before right-clicking a mine.
- Try right-clicking directly on the mine body.
- If pathing gets crowded, issue one move command nearby, then right-click mine again.

### Game launches with warning: Setting vertical sync not supported

- In WSL this is expected and harmless.

### No window appears in WSL

- Ensure GUI forwarding is available.
- On Windows 11 with WSLg this usually works automatically.
- Otherwise run `export DISPLAY=:0` before starting.

## Build Notes

- Use WSL + CMake for consistent builds
- Reconfigure with `cmake -S . -B build` after CMake file changes
- Rebuild with `cmake --build build -j$(nproc)`