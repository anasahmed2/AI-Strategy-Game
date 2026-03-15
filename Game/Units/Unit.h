#pragma once

#include <memory>
#include "../../Engine/Math/Vector2.h"
#include "../../Engine/ECS/Entity.h"
#include "../../Engine/Core/Engine.h"

class Unit {
public:
    virtual ~Unit() = default;
    
    // Create entity with all components
    virtual std::shared_ptr<Entity> create(std::shared_ptr<Engine> engine, Vector2 position,
                                           Faction faction, bool isAIControlled);
    
    // Unit properties
    virtual float getMaxHealth() const { return 100.0f; }
    virtual float getSpeed() const { return 100.0f; }
    virtual float getAttackDamage() const { return 10.0f; }
    virtual float getAttackRange() const { return 50.0f; }
    virtual EntityRole getRole() const { return EntityRole::Unknown; }
    
protected:
    std::shared_ptr<Engine> engine;
    std::shared_ptr<Entity> entity;
    Faction faction = Faction::Neutral;
    bool isAIControlled = false;
    
    // Setup components
    virtual void setupComponents(Vector2 position);
    virtual void setupAI();
};
