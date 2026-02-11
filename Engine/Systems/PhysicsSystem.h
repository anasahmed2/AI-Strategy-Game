#pragma once

#include "../ECS/System.h"
#include "../ECS/Component.h"

class PhysicsSystem : public System {
public:
    void update(float deltaTime) override;
    void setRequiredComponents() override;
    
    // Collision detection
    bool checkCollision(const std::shared_ptr<Entity>& entity1, 
                       const std::shared_ptr<Entity>& entity2) const;
    
    void setGravity(float g);

private:
    float gravity = 0.0f;  // No gravity by default for top-down game
    
    void integrateVelocity(std::shared_ptr<Entity> entity, float deltaTime);
    void checkEntityCollisions();
};
