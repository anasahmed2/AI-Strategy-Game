#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <utility>
#include <unordered_map>
#include "../Math/Vector2.h"

class Component {
public:
    virtual ~Component() = default;
    virtual void update(float deltaTime) {}
};

enum class Faction {
    Neutral,
    Player,
    Enemy
};

enum class EntityRole {
    Unknown,
    Worker,
    Soldier,
    Tank,
    Scout,
    Base,
    ResourceMine,
    Turret,
    Obstacle,
    Terrain
};

enum class CommandType {
    None,
    Move,
    Attack,
    Gather,
    Defend,
    ReturnToBase
};

// ===== TRANSFORM COMPONENT ===== 

struct TransformComponent : public Component {
    Vector2 position;
    Vector2 scale = Vector2(1.0f, 1.0f);
    float rotation = 0.0f;
    
    TransformComponent() = default;
    TransformComponent(Vector2 pos) : position(pos) {}
};

// ===== PHYSICS COMPONENT =====
struct PhysicsComponent : public Component {
    Vector2 velocity;
    Vector2 acceleration;
    float mass = 1.0f;
    float friction = 0.1f;
    
    PhysicsComponent() = default;
};

// ===== RENDER COMPONENT =====
struct RenderComponent : public Component {
    std::string spriteId;  // Reference to sprite asset
    int width = 32;
    int height = 32;
    int layer = 0;  // For Z-ordering
    bool visible = true;
    
    RenderComponent() = default;
    RenderComponent(std::string id) : spriteId(id) {}
};

// ===== HEALTH COMPONENT =====
struct HealthComponent : public Component {
    float currentHealth = 100.0f;
    float maxHealth = 100.0f;
    
    HealthComponent() = default;
    HealthComponent(float health) : currentHealth(health), maxHealth(health) {}
};

// ===== GAMEPLAY IDENTITY COMPONENTS =====
struct TeamComponent : public Component {
    Faction faction = Faction::Neutral;
    bool isAIControlled = false;

    TeamComponent() = default;
    TeamComponent(Faction faction, bool aiControlled)
        : faction(faction), isAIControlled(aiControlled) {}
};

struct RoleComponent : public Component {
    EntityRole role = EntityRole::Unknown;

    RoleComponent() = default;
    explicit RoleComponent(EntityRole role) : role(role) {}
};

// ===== COLLIDER COMPONENT =====
struct ColliderComponent : public Component {
    float radius = 16.0f;  // Circular collider for simplicity
    bool isTrigger = false;
    
    ColliderComponent() = default;
    ColliderComponent(float r) : radius(r) {}
};

// ===== RESOURCE COLLECTOR COMPONENT =====
struct ResourceCollectorComponent : public Component {
    float collectionRate = 1.0f;  // Resources per second
    std::string resourceType;  // "Gold", "Energy", "Wood"
    float carryAmount = 0.0f;
    float carryCapacity = 50.0f;
    float gatherRange = 36.0f;
    float dropOffRange = 56.0f;
    
    ResourceCollectorComponent() = default;
};

// ===== RESOURCE CONTAINER COMPONENT =====
struct ResourceContainerComponent : public Component {
    std::unordered_map<std::string, float> resources;
    std::unordered_map<std::string, float> capacity;
    
    ResourceContainerComponent() = default;
};

struct ResourceNodeComponent : public Component {
    std::string resourceType = "Gold";
    float amountRemaining = 1000.0f;

    ResourceNodeComponent() = default;
    ResourceNodeComponent(std::string type, float amount)
        : resourceType(std::move(type)), amountRemaining(amount) {}
};

// ===== SELECTION COMPONENT =====
struct SelectionComponent : public Component {
    bool isSelected = false;
    bool isSelectable = true;
    
    SelectionComponent() = default;
    SelectionComponent(bool selectable) : isSelectable(selectable) {}
};

// ===== MOVEMENT COMPONENT =====
struct MovementComponent : public Component {
    Vector2 targetPosition;
    bool hasTarget = false;
    float moveSpeed = 100.0f;  // pixels per second
    float arrivalRadius = 5.0f;  // Distance to consider "arrived"
    float stuckTimer = 0.0f;
    Vector2 lastPosition;
    bool hasLastPosition = false;
    
    MovementComponent() = default;
    MovementComponent(float speed) : moveSpeed(speed) {}
    
    void setTarget(Vector2 target) {
        targetPosition = target;
        hasTarget = true;
    }
    
    void clearTarget() {
        hasTarget = false;
    }
};

struct PathComponent : public Component {
    std::vector<Vector2> waypoints;
    std::size_t currentIndex = 0;

    void clear() {
        waypoints.clear();
        currentIndex = 0;
    }

    bool hasPath() const {
        return currentIndex < waypoints.size();
    }
};

struct CommandComponent : public Component {
    CommandType type = CommandType::None;
    Vector2 targetPosition;
    std::uint32_t targetEntityId = 0;
    Vector2 defendPosition;
};

struct AIConfigComponent : public Component {
    bool enabled = true;
    Vector2 patrolCenter;
    float patrolRadius = 120.0f;
    float retreatHealthThreshold = 0.3f;
    float engagementRange = 220.0f;
};
