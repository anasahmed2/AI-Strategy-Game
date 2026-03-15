#pragma once

#include <memory>
#include "../../Engine/Math/Vector2.h"
#include "../../Engine/ECS/Entity.h"
#include "../../Engine/Core/Engine.h"

class Building {
public:
    virtual ~Building() = default;
    
    // Create entity with all components
    virtual std::shared_ptr<Entity> create(std::shared_ptr<Engine> engine, Vector2 position,
                                           Faction faction, bool isAIControlled = false);
    
    // Building properties
    virtual float getMaxHealth() const { return 500.0f; }
    virtual EntityRole getRole() const { return EntityRole::Unknown; }
    
protected:
    std::shared_ptr<Engine> engine;
    std::shared_ptr<Entity> entity;
    Faction faction = Faction::Neutral;
    bool isAIControlled = false;
    
    // Setup components
    virtual void setupComponents(Vector2 position);
};
