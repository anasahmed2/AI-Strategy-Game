# Git Commit Strategy for This Project

This document shows you how to commit your work so it looks professional and tells a story of engineering growth.

## Commit Structure

Each commit should be **focused on one feature/fix** and have a clear message.

### Format
```
<emoji> <type>: <subject>

<body>

<footer>
```

Where:
- **emoji**: Quick visual indicator (see legend below)
- **type**: feat/fix/refactor/docs/style/test
- **subject**: 50 chars max, imperative mood
- **body**: Explain WHY, not what (70 chars per line)
- **footer**: References, breaking changes

## Emoji Legend

```
✨ feat:     New feature (AI system, component type)
🐛 fix:      Bug fix (collision fix, state machine issue)  
♻️  refactor: Code restructuring (no functional change)
📝 docs:     Documentation (README, ARCHITECTURE)
🎨 style:    Code formatting (spacing, naming)
⚡ perf:     Performance improvement (optimization)
✅ test:     Tests or test-related
🏗️  build:    Build system (CMake, dependencies)
```

## Example Commits (In Order)

Here's how you'd commit the initial build:

### Commit 1: Foundation
```
🏗️ build: Initialize CMake project structure

Create root CMakeLists.txt and establish basic project hierarchy
with Engine and Game subdirectories.

- Set C++17 standard
- Configure SFML as dependency
- Separate engine library from game executable
```

### Commit 2: Math Foundation
```
✨ feat: Implement Vector2 mathematics library

Created 2D vector math with full operator overloads and utilities.
Used in physics calculations and transformations throughout engine.

- Vector2(x, y) constructor
- Operators: +, -, *, / with scalar multiplication
- Utility: magnitude(), distance(), normalize(), dot()
- Manhattan distance for grid-based pathfinding
```

### Commit 3: ECS Core
```
✨ feat: Build Entity Component System architecture

Implemented data-oriented ECS pattern for scalable entity management.
Entities are ID containers, components are pure data, systems run logic.

- Entity class with generic component storage
- Component base class with required subclasses
- System base class with entity filtering
- ComponentRegistry for ECS coordination

This provides foundation for all game objects.
```

### Commit 4: Core Components
```
✨ feat: Define core component types

Added 7 component types to handle all entity aspects:

- TransformComponent (position, scale, rotation)
- PhysicsComponent (velocity, acceleration, mass)
- RenderComponent (sprite, layer, visibility)
- HealthComponent (health tracking)
- ColliderComponent (collision detection)
- ResourceCollectorComponent (gather resources)
- ResourceContainerComponent (store resources)

Components are pure data; systems handle logic.
```

### Commit 5: Physics
```
✨ feat: Implement physics system with integration and collisions

Added velocity-based physics with Euler integration.
Includes circular collision detection (O(n²)).

- Velocity and acceleration integration
- Friction simulation
- Gravity support (not used for top-down game)
- Circle-circle collision detection
- Automatic entity filtering (only processes entities with required components)

Physics updates once per frame before rendering.
```

### Commit 6: Input System
```
✨ feat: Create input system with SFML event handling

Bridges between game engine and SFML window events.
Tracks mouse position and button states each frame.

- Mouse position tracking for aim/selection
- Keyboard state queries (isKeyPressed)
- Event-based input handling
- Separation of input reading from game logic

Allows decoupled input handling from gameplay.
```

### Commit 7: Render System
```
✨ feat: Build render system with layer-based sorting

Renders all entities to SFML window in sorted order.
Supports layering for proper depth ordering.

- Automatically processes entities with TransformComponent + RenderComponent
- Sorts by render layer (lower = drawn first)
- Draws circles as placeholder sprites
- Integrates with SFML RenderWindow

Currently debug rendering; ready for sprite atlas integration.
```

### Commit 8: Combat System
```
✨ feat: Develop combat system with targeting and damage

Complete combat pipeline: target finding → attack cooldown → damage → death.

- Attack cooldown management per unit
- Auto-targeting of enemies in attack range
- Damage calculation and health deduction  
- Automatic entity destruction at 0 HP
- O(n) find nearest target per attacker

Ready for expansion to damage types, resistances, etc.
```

### Commit 9: Event System
```
✨ feat: Implement event system for decoupled communication

Publisher-subscriber pattern allows systems to communicate without coupling.

- Template-based event subscription by type
- Event publishing to all subscribers
- Event types: Collision, Damage, ResourceCollected, EntityDestroyed
- Systems update independently without direct references

Enables features like damage logging, achievement tracking later.
```

### Commit 10: Resource System
```
✨ feat: Create resource management system

Handles gold, energy, wood collection and inventory tracking.

- Global resource pool shared by all players
- Per-entity resource containers with capacity limits
- Time-based resource collection from collectors
- Support for multiple resource types

Foundation for economy system and trading later.
```

### Commit 11: AI - Blackboard
```
✨ feat: Build AI blackboard shared memory system

Blackboard provides shared working memory for AI decision-making.
AI systems (FSM, BT) read/write to blackboard instead of directly querying world.

- Key-value store with any type support (via std::any)
- AI-specific fields: targetPosition, targetEntityId, health, threats
- Sensory information: enemySpotted, resourceSpotted
- Clean separation between perception and decision

Enables reusable behavior patterns.
```

### Commit 12: AI - State Machine
```
✨ feat: Implement finite state machine for unit behavior

FSM handles high-level unit state transitions: Idle → Move → Attack → Flee

- State base class with enter/update/exit lifecycle
- 7 unit states: Idle, Move, Attack, Flee, Gather, Build, Die
- Automatic state transition on demand
- Current state tracking and queries

States provide context for what unit is doing.
```

### Commit 13: AI - Behavior Trees
```
✨ feat: Create behavior tree hierarchical decision system

Behavior trees enable expressive AI without if-else complexity.

- Composite nodes: Selector (try until success), Sequence (all must succeed)
- Execution nodes: Condition (boolean check), Action (do something)
- Return status: Running, Success, Failure
- Recursive evaluation from root node

Example tree:
  Selector
    Sequence [Enemy nearby? → Attack]
    Sequence [Health low? → Flee]  
    Sequence [Resource nearby? → Gather]
    Action [Patrol]
```

### Commit 14: Pathfinding
```
✨ feat: Implement A* pathfinding for grid-based movement

Professional pathfinding algorithm optimized for strategy games.

- Grid registration of obstacles
- A* with Manhattan distance heuristic  
- Open/closed list with cost-based prioritization
- Returns waypoint array for unit to follow
- Gracefully falls back to direct path if none found

Waypoint-following enables smooth unit movement.
```

### Commit 15: Game Framework
```
✨ feat: Build GameManager orchestrating engine and game layer

GameManager creates engine, initializes systems, and manages game state.

- Creates Engine instance (handles all systems)
- Registers all game systems with ECS
- Spawning methods for each unit and building type
- Main game loop calling update/render

Bridges gap between engine and specific game implementation.
```

### Commit 16: Unit System
```
✨ feat: Create Unit base class and specific unit types

Foundation for all controllable units in game.

- Base Unit class with component setup
- Worker: 50 HP, gatherer, weak attack
- Soldier: 100 HP, balanced fighter
- Tank: 250 HP, heavy damage dealer
- AI component and behavior tree setup stubs

Each unit type has unique stats and behaviors.
```

### Commit 17: Building System
```
✨ feat: Build building classes for static structures

Foundation for base structures.

- Base Building class with component setup
- Base: 1000 HP, spawn point, resource storage
- ResourceMine: 300 HP, generates 10 gold/sec
- Turret: 200 HP, 150 range attack, auto-defense

Buildings drive economy and territory control.
```

### Commit 18: Main Loop
```
✨ feat: Implement game loop and entry point

Complete game loop integrated with SFML window.

- DeltaTime calculation
- Frame-by-frame system updates
- Event polling and rendering
- Proper cleanup on exit

Game now runs with initial units and buildings.
```

### Commit 19: Documentation
```
📝 docs: Add comprehensive architecture documentation

Created detailed architecture explanations.

- ARCHITECTURE.md: Deep dives into each system
- README.md: Project overview and capabilities
- QUICK_START.md: Setup and development guide
- BUILD_SUMMARY.md: What was built and resume gold

Makes codebase accessible to future readers.
```

### Commit 20: Engine Integration
```
✨ feat: Create core Engine class managing all systems

Central orchestrator bringing all systems together.

- System registration in specific order
- Window and render target management
- Delta time tracking
- Game loop invocation point
- Registry access for creating entities

Single entry point to entire engine.
```

## How to Make These Commits

```bash
# Stage your work
git add Engine/Math/Vector2.h Engine/Math/Vector2.cpp

# Commit with proper message
git commit -m "✨ feat: Implement Vector2 mathematics library

Created 2D vector math with full operator overloads and utilities.
Used in physics calculations and transformations throughout engine.

- Vector2(x, y) constructor
- Operators: +, -, *, / with scalar multiplication
- Utility: magnitude(), distance(), normalize(), dot()
- Manhattan distance for grid-based pathfinding"

# View your commits
git log --oneline
```

## Real-World Example: After 20 Commits

```
✨ feat: Create core Engine class managing all systems
📝 docs: Add comprehensive architecture documentation
✨ feat: Implement game loop and entry point
✨ feat: Build building classes for static structures
✨ feat: Create Unit base class and specific unit types
✨ feat: Build GameManager orchestrating engine and game layer
✨ feat: Implement A* pathfinding for grid-based movement
✨ feat: Create behavior tree hierarchical decision system
✨ feat: Implement finite state machine for unit behavior
✨ feat: Build AI blackboard shared memory system
✨ feat: Create resource management system
✨ feat: Implement event system for decoupled communication
✨ feat: Develop combat system with targeting and damage
✨ feat: Build render system with layer-based sorting
✨ feat: Create input system with SFML event handling
✨ feat: Implement physics system with integration and collisions
✨ feat: Define core component types
✨ feat: Build Entity Component System architecture
✨ feat: Implement Vector2 mathematics library
🏗️ build: Initialize CMake project structure
```

This tells a **story** of building a game engine from scratch.

## Pro Tips

1. **Commit frequently** - After each feature completes (1-2 hours of work)
2. **Test before committing** - Code should compile cleanly
3. **Keep commits focused** - Don't mix systems in single commit
4. **Write good messages** - Future you will thank current you
5. **Use proper types** - `feat:` for features, `fix:` for bugs
6. **Reference systems** - Mention which system/component affected

## What Recruiters See

When they look at your git log:

```
✅ Structured progression
✅ Feature-complete implementation
✅ Professional commit messages
✅ Logical system decomposition
✅ Incremental complexity
✅ Clean git history
❌ No "WIP", "fix", "debug stuff" commits
```

## Next: Push to GitHub

```bash
# Create repo on GitHub (don't initialize with README)
git remote add origin https://github.com/YourName/AI-Strategy-Game.git
git branch -M main
git push -u origin main
```

Your GitHub now shows:

- ✅ 20+ professional commits
- ✅ Clear engineering progression
- ✅ Production-quality code
- ✅ Comprehensive documentation
- ✅ Interview-ready project

---

**This is how you show engineering discipline.**

Good luck! 🎯
