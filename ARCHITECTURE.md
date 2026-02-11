
# AI Strategy Game - Architecture Deep Dive

## Project Philosophy

This project is built with **engineering discipline**, not beginner tutorials. Every architectural decision prioritizes:

1. **Scalability** - Can handle hundreds of entities with complex AI
2. **Reusability** - Engine code works for any game built on it
3. **Maintainability** - Clear separation of concerns
4. **Performance** - Data-oriented design for cache efficiency
5. **Extensibility** - Easy to add new systems and behaviors

---

## ECS (Entity Component System) Deep Dive

### Why ECS Instead of OOP Inheritance?

**Traditional OOP (Bad for Games):**
```cpp
class Character { virtual void update(); };
class Unit : public Character { };
class Enemy : public Unit { };
class Soldier : public Enemy { };
// Result: Deep hierarchy, vtable overhead, cache misses, tight coupling
```

**ECS (Good for Games):**
```cpp
Entity 1: [TransformComponent][PhysicsComponent][AIComponent][CombatComponent]
Entity 2: [TransformComponent][ResourceCollectorComponent]
Entity 3: [TransformComponent][HealthComponent]

// Systems process only entities that have required components
AISystem processes only Entity 1
ResourceSystem processes only Entity 2
PhysicsSystem processes both Entity 1 and Entity 3
```

### Memory Layout
```
Component Storage (Contiguous Memory = Cache Efficient)

TransformComponent Array:
[Entity1 Transform][Entity3 Transform][Entity2 Transform]...

PhysicsComponent Array:
[Entity1 Physics][Entity3 Physics]...

RenderComponent Array:
[Entity1 Render][Entity3 Render][Entity2 Render]...
```

When **PhysicsSystem** updates, it iterates Entity 1 and Entity 3 in order → perfect cache locality.

### Component Types

```cpp
struct TransformComponent {
    Vector2 position;
    Vector2 scale;
    float rotation;
};
```
**Why not Transform* in Entity?** Because it creates pointer indirection (cache miss).

---

## AI Architecture

### 1. Blackboard (Shared Memory)

Units need to share information without tight coupling:

```cpp
AI Unit's Blackboard:
├── targetPosition (Vector2)
├── targetEntityId (uint32)
├── health (float)
├── enemySpotted (bool)
├── resourceSpotted (bool)
└── Custom key-value pairs
```

**Why this pattern?**
- Behavior Tree reads blackboard for decisions
- State Machine can check blackboard to transition
- Multiple behaviors share the same data
- Easy to debug (inspect blackboard at any time)

### 2. Behavior Tree Execution

```cpp
Selector {
    Sequence {
        Condition: EnemyNearby()
        Action: Attack()
    }
    Sequence {
        Condition: HealthLow()
        Action: Flee()
    }
    Sequence {
        Condition: ResourceNearby()
        Action: Gather()
    }
    Action: Patrol()
}
```

**Execution Flow:**
1. Try first sequence: If enemy nearby AND can attack → SUCCESS, stop
2. If first failed, try second: If health low AND can flee → SUCCESS, stop
3. If second failed, try third: If resource nearby AND can gather → SUCCESS, stop
4. If all failed, patrol

**BehaviorStatus Return Values:**
- `Success` - Action completed successfully
- `Running` - Action still executing (will continue next frame)
- `Failure` - Action failed, try next

### 3. State Machine for Unit Behavior

```
FSM States:

Idle State
    - Entry: Stop moving, stop attacking
    - Update: Check for threats/targets
    - Exit: N/A
    - Transitions: → Gather, → Attack, → Flee

Move State
    - Entry: Start pathfinding
    - Update: Follow path, check for interruptions
    - Exit: Clear path
    - Transitions: → Attack (enemy found), → Idle (reached goal)

Attack State
    - Entry: Face target, start cooldown
    - Update: Check if target in range, deal damage
    - Exit: N/A
    - Transitions: → Flee (health low), → Move (target moved away)

Flee State
    - Entry: Pathfind away from threat
    - Update: Follow escape path
    - Exit: N/A
    - Transitions: → Idle (threat gone), → Move (safe distance)
```

### Decision-Making Pipeline

```
Frame 1:
├─ Sensory Update (AISystem)
│  └─ Check what unit sees within vision range
│     └─ Update blackboard.enemySpotted, blackboard.resourceSpotted
│
├─ Behavior Tree Evaluation
│  └─ Selector checks: Enemy? → YES
│     └─ Set: blackboard.actionToDo = "Attack"
│
├─ State Machine Transition
│  └─ Current state: Move
│     └─ New state: Attack (based on actionToDo)
│
└─ State Execution
   └─ AttackState::update()
      └─ Find targets, deal damage
```

---

## Pathfinding (A* Algorithm)

### Grid System
```
Map: 1200x800 pixels
Cell size: 32 pixels
Grid: 37.5 × 25 cells ≈ 38 × 25

Each cell either:
  - Walkable (0)
  - Obstacle (1)
```

### A* Algorithm Steps

**Open List**: Priority queue of nodes to evaluate
**Closed List**: Nodes we've already processed

```
1. Add start node to open list
2. While open list not empty:
   a. Get node with lowest f_cost
   b. If this is goal → reconstruct path, return
   c. For each neighbor of current node:
      - If walkable and not in closed list:
        - Calculate g_cost (cost from start)
        - Calculate h_cost (heuristic to goal)
        - Calculate f_cost = g_cost + h_cost
        - Add to open list
   d. Add current to closed list
3. No path found → return straight line to goal
```

### Heuristic (Manhattan Distance)
```cpp
float heuristic(Vector2 from, Vector2 to) {
    return abs(from.x - to.x) + abs(from.y - to.y);
}
```

**Why Manhattan?** Grid cells can only move in 4 directions (up, down, left, right), so Manhattan is admissible.

### Pathfinding Flow
```
Unit wants to move to position X
    ↓
Check if pathfinding is needed (distance > threshold)
    ↓
Call Pathfinder::findPath(currentPos, targetPos)
    ↓
A* returns waypoint array: [pos1, pos2, pos3, ..., targetPos]
    ↓
Unit follows waypoints one by one
    ↓
Each frame: Move from current waypoint toward next waypoint
```

---

## Physics System

### Velocity Integration (Euler Method)

```cpp
void update(deltaTime) {
    for each entity:
        // Apply friction
        velocity *= (1.0 - friction)
        
        // Apply gravity
        acceleration.y += gravity
        
        // Update velocity and position
        velocity += acceleration * deltaTime
        position += velocity * deltaTime
        
        // Reset acceleration
        acceleration = Vector2.zero()
}
```

### Collision Detection

Simple circle-based collisions:
```cpp
bool collides(Entity a, Entity b) {
    auto ta = a->getComponent<TransformComponent>();
    auto ca = a->getComponent<ColliderComponent>();
    auto tb = b->getComponent<TransformComponent>();
    auto cb = b->getComponent<ColliderComponent>();
    
    float distance = ta->position.distance(tb->position);
    float minDistance = ca->radius + cb->radius;
    
    return distance < minDistance;
}
```

**Optimization Idea**: Use spatial hashing grid instead of O(n²) checks.

---

## Combat System

### Attack Targeting
```cpp
FindTargetsInRange(attacker, range):
    targets = []
    for each entity:
        if entity == attacker: skip
        if entity.combatComponent == null: skip
        if distance(attacker, entity) <= range:
            targets.push(entity)
    return targets
```

### Damage Flow
```
CombatSystem::update()
    ├─ Update all attack cooldowns
    └─ For each unit with CombatComponent:
       ├─ Find targets in range
       └─ If targets found AND cooldown ready:
          ├─ Deal damage to first target
          ├─ Reset cooldown
          └─ Publish DamageEvent
                ↓
            HealthComponent::currentHealth -= damage
            If health <= 0: entity->destroy()
```

---

## Event System

### Publisher-Subscriber Pattern

**Why events instead of direct calls?**
- Systems don't need to know about each other
- Easy to add new event handlers
- Events can be deferred/queued
- Logging and debugging made easy

```cpp
// Subscribe
eventSystem->subscribe<DamageEvent>([](auto event) {
    std::cout << "Entity " << event->targetEntityId 
              << " took " << event->damage << " damage" << std::endl;
});

// Publish
auto damageEvent = std::make_shared<DamageEvent>(targetId, 25.0f);
eventSystem->publish(damageEvent);

// Triggers all subscribed callbacks
```

---

## Game Loop Execution Order

```cpp
while (running) {┌────────────────────────────────────────────┐
                 │ FRAME START (60 FPS = 16.67ms)            │
                 └────────────────────────────────────────────┘
                                    ↓
                 ┌────────────────────────────────────────────┐
                 │ 1. HANDLE INPUT                             │
                 │   InputSystem::handleEvents()               │
                 └────────────────────────────────────────────┘
                                    ↓
                 ┌────────────────────────────────────────────┐
                 │ 2. AI DECISIONS                             │
                 │   AISystem::updateBlackboards()             │
                 │   - Check sensory input                     │
                 │   - Update threat levels                    │
                 │   BehaviorTree::evaluate()                  │
                 │   - Make decisions                          │
                 │   StateMachine::update()                    │
                 │   - Execute current state                   │
                 └────────────────────────────────────────────┘
                                    ↓
                 ┌────────────────────────────────────────────┐
                 │ 3. PHYSICS                                  │
                 │   PhysicsSystem::update(deltaTime)          │
                 │   - Integrate velocity                      │
                 │   - Check collisions                        │
                 │   - Update positions                        │
                 └────────────────────────────────────────────┘
                                    ↓
                 ┌────────────────────────────────────────────┐
                 │ 4. COMBAT                                   │
                 │   CombatSystem::update()                    │
                 │   - Update cooldowns                        │
                 │   - Find targets                            │
                 │   - Deal damage                             │
                 └────────────────────────────────────────────┘
                                    ↓
                 ┌────────────────────────────────────────────┐
                 │ 5. RESOURCES                                │
                 │   ResourceSystem::update(deltaTime)         │
                 │   - Collect resources                       │
                 │   - Update inventories                      │
                 └────────────────────────────────────────────┘
                                    ↓
                 ┌────────────────────────────────────────────┐
                 │ 6. RENDER                                   │
                 │   window.clear()                            │
                 │   RenderSystem::render()                    │
                 │   window.display()                          │
                 └────────────────────────────────────────────┘
                                    ↓
                 ┌────────────────────────────────────────────┐
                 │ 7. CLEANUP                                  │
                 │   Registry::cleanup()                       │
                 │   - Remove destroyed entities               │
                 │   - Free memory                             │
                 └────────────────────────────────────────────┘
                                    ↓
                    FRAME COMPLETE (~16.67ms)
}
```

---

## Data Flow Example: Unit Attacks Another Unit

```
Frame N:
┌────────────────────────────────────────────────┐
│ AISystem::updateBlackboards()                   │
│                                                 │
│ Soldier unit checks position (100, 200)         │
│ Searches for enemy within vision range          │
│ Finds Enemy at (130, 220) - distance 41.2      │
│ Updates blackboard:                             │
│   soldierBlacckboard.enemySpotted = true       │
│   soldierBlacckboard.enemyPosition = (130,220) │
│   soldierBlacckboard.targetEntityId = enemy.id │
└────────────────────────────────────────────────┘
                        ↓
┌────────────────────────────────────────────────┐
│ BehaviorTree::evaluate()                        │
│ Root: Selector {                                │
│   Sequence {                                    │
│     Condition: blackboard.enemySpotted == true │
│     → Returns SUCCESS                           │
│   }                                             │
│ }                                               │
│ Tree returns: SUCCESS                           │
│ Action decided: ATTACK                          │
└────────────────────────────────────────────────┘
                        ↓
┌────────────────────────────────────────────────┐
│ StateMachine::changeState(ATTACK)               │
│                                                 │
│ Current state: MOVE → exits                     │
│ New state: ATTACK → enters                      │
│ AttackState checks target distance              │
│ Distance: 41.2 < attackRange: 60 ✓ IN RANGE    │
└────────────────────────────────────────────────┘
                        ↓
┌────────────────────────────────────────────────┐
│ CombatSystem::update()                          │
│                                                 │
│ For Soldier unit:                               │
│   - Check cooldown: 0.3s remaining ✗️ NOT READY │
│   - Decrement: cooldown -= deltaTime            │
│   - No attack this frame                        │
└────────────────────────────────────────────────┘

Frame N+1 (0.016s later):
┌────────────────────────────────────────────────┐
│ CombatSystem::update()                          │
│                                                 │
│ For Soldier unit:                               │
│   - Check cooldown: 0.284s remaining ✗️ NOT READY│
│   - Decrement: cooldown -= deltaTime            │
│   - Still waiting...                            │
└────────────────────────────────────────────────┘

Frame N+20 (0.32s later, cooldown expired):
┌────────────────────────────────────────────────┐
│ CombatSystem::update()                          │
│                                                 │
│ For Soldier unit:                               │
│   - Check cooldown: 0 <= 0 ✓ READY              │
│   - Find targets in range: [Enemy]              │
│   - dealDamage(Enemy, 15.0)                     │
│       └─ Update target health: 100 - 15 = 85   │
│   - Reset cooldown: 1.5s                        │
│   - Publish DamageEvent(enemyId, 15)            │
└────────────────────────────────────────────────┘
                        ↓
        Event subscribers notified (logging, etc)
        Enemy's HealthComponent updated
        Follow-up: If health drops to 0, enemy destroyed
```

---

## Code Quality & Resume Impact

### What Recruiters See

✅ **Custom ECS** - Shows you understand data-oriented design
✅ **Behavior Trees** - Modern AI, not if-else chains
✅ **A* Pathfinding** - Classic algorithm, properly implemented
✅ **State Machines** - Professional behavior management
✅ **Event System** - Loose coupling, scalable design
✅ **Component Architecture** - Clean, testable code
✅ **Memory Management** - Smart pointers, no leaks
✅ **Separation of Concerns** - Engine vs Game layer

### Interview Talking Points

*"I built a custom ECS from scratch because it provides better cache locality than traditional OOP. Components are stored contiguously, so when a system processes entities, it gets perfect memory access patterns."*

*"My AI uses behavior trees for hierarchical decision-making - selectors for 'try this OR that', sequences for 'do this AND that'. The blackboard allows units to share information without hard coupling."*

*" A* pathfinding uses Manhattan distance heuristic because we're on a grid with 4-directional movement. Each unit caches waypoints to avoid recalculating every frame."*

---

## Next Steps for Enhancement

### Immediate (1-2 weeks)
1. Complete Worker/Soldier/Tank AI behaviors
2. Add ImGui debug visualization
3. Implement proper rendering with sprites
4. Add sound effects

### Short-term (2-4 weeks)
1. Spatial hashing optimization for collisions
2. Squad-based tactics (units work together)
3. Fog of war implementation
4. Resource decay (units get hungry)

### Long-term (4+ weeks)
1. Multiplayer networking
2. Neural network-based learning AI
3. Procedural map generation
4. Advanced pathfinding (jump flooding, flow fields)

---

This architecture is **production-ready foundation** for a real strategy game. Build it out, and you'll have portfolio gold.
