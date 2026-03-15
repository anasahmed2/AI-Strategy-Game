#include "MovementSystem.h"
#include <cmath>

void MovementSystem::update(float deltaTime) {
    for (auto& entity : entities) {
        auto transform = entity->getComponent<TransformComponent>();
        auto movement = entity->getComponent<MovementComponent>();
        auto physics = entity->getComponent<PhysicsComponent>();
        auto path = entity->getComponent<PathComponent>();
        auto command = entity->getComponent<CommandComponent>();

        if (!transform || !movement) {
            continue;
        }

        if (path && path->hasPath()) {
            movement->setTarget(path->waypoints[path->currentIndex]);
        }
        
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
            if (path && path->hasPath()) {
                ++path->currentIndex;
                if (path->hasPath()) {
                    movement->setTarget(path->waypoints[path->currentIndex]);
                    continue;
                }

                path->clear();
            }

            movement->clearTarget();
            if (physics) {
                physics->velocity = Vector2(0, 0);
            }

            if (command && (command->type == CommandType::Move || command->type == CommandType::Defend)) {
                command->type = CommandType::None;
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

        // Basic anti-stuck handling for dynamic maps/crowding.
        if (!movement->hasLastPosition) {
            movement->lastPosition = transform->position;
            movement->hasLastPosition = true;
        } else {
            float movedDistance = transform->position.distance(movement->lastPosition);
            if (movedDistance < 1.0f) {
                movement->stuckTimer += deltaTime;
            } else {
                movement->stuckTimer = 0.0f;
                movement->lastPosition = transform->position;
            }

            if (movement->stuckTimer > 0.8f) {
                movement->stuckTimer = 0.0f;
                if (path) {
                    path->clear();
                }
                movement->clearTarget();
                if (physics) {
                    physics->velocity = Vector2::zero();
                }
            }
        }
    }
}

void MovementSystem::setRequiredComponents() {
    require<TransformComponent>();
    require<MovementComponent>();
}
