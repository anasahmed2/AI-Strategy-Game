#pragma once

#include <string>
#include <unordered_map>
#include "../../Math/Vector2.h"

class Component {
public:
    virtual ~Component() = default;
    virtual void update(float deltaTime) {}
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
    
    ResourceCollectorComponent() = default;
};

// ===== RESOURCE CONTAINER COMPONENT =====
struct ResourceContainerComponent : public Component {
    std::unordered_map<std::string, float> resources;
    std::unordered_map<std::string, float> capacity;
    
    ResourceContainerComponent() = default;
};

