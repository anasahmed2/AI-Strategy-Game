#include "MovementSystem.h"
#include <cmath>

void MovementSystem::update(float deltaTime) {
    for (auto& entity : entities) {
        auto transform = entity->getComponent<TransformComponent>();
        auto movement = entity->getComponent<MovementComponent>();
        auto physics = entity->getComponent<PhysicsComponent>();
        
        if (!movement->hasTarget) {
            // No target, stop moving
            if (physics) {
                physics->velocity = Vector2(0, 0);
            }
            continue;
        }
        
        // Calculate direction to target
        Vector2 direction = movement->targetPosition - transform->position;
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        
        // Check if arrived
        if (distance <= movement->arrivalRadius) {
            movement->clearTarget();
            if (physics) {
                physics->velocity = Vector2(0, 0);
            }
            continue;
        }
        
        // Normalize direction and apply speed
        direction.x /= distance;
        direction.y /= distance;
        
        Vector2 targetVelocity(
            direction.x * movement->moveSpeed,
            direction.y * movement->moveSpeed
        );
        
        // Apply velocity to physics component if available
        if (physics) {
            physics->velocity = targetVelocity;
        } else {
            // Directly update position if no physics
            transform->position.x += targetVelocity.x * deltaTime;
            transform->position.y += targetVelocity.y * deltaTime;
        }
    }
}

void MovementSystem::setRequiredComponents() {
    require<TransformComponent>();
    require<MovementComponent>();
}
