#include "PhysicsSystem.h"

void PhysicsSystem::update(float deltaTime) {
    for (auto& entity : entities) {
        if (!entity->isActive()) continue;
        
        auto physics = entity->getComponent<PhysicsComponent>();
        if (!physics) continue;
        
        integrateVelocity(entity, deltaTime);
    }
    
    checkEntityCollisions();
}

void PhysicsSystem::setRequiredComponents() {
    require<TransformComponent>();
    require<PhysicsComponent>();
}

void PhysicsSystem::integrateVelocity(std::shared_ptr<Entity> entity, float deltaTime) {
    auto transform = entity->getComponent<TransformComponent>();
    auto physics = entity->getComponent<PhysicsComponent>();
    
    if (!transform || !physics) return;
    
    // Apply friction
    physics->velocity *= (1.0f - physics->friction);
    
    // Apply gravity
    if (gravity != 0.0f) {
        physics->acceleration.y += gravity;
    }
    
    // Update velocity from acceleration
    physics->velocity += physics->acceleration * deltaTime;
    
    // Update position from velocity
    transform->position += physics->velocity * deltaTime;
    
    // Reset acceleration
    physics->acceleration = Vector2::zero();
}

bool PhysicsSystem::checkCollision(const std::shared_ptr<Entity>& entity1, 
                                   const std::shared_ptr<Entity>& entity2) const {
    auto t1 = entity1->getComponent<TransformComponent>();
    auto c1 = entity1->getComponent<ColliderComponent>();
    auto t2 = entity2->getComponent<TransformComponent>();
    auto c2 = entity2->getComponent<ColliderComponent>();
    
    if (!t1 || !c1 || !t2 || !c2) return false;
    
    float distance = t1->position.distance(t2->position);
    float minDist = c1->radius + c2->radius;
    
    return distance < minDist;
}

void PhysicsSystem::checkEntityCollisions() {
    // Simple O(n²) collision detection
    for (size_t i = 0; i < entities.size(); ++i) {
        for (size_t j = i + 1; j < entities.size(); ++j) {
            if (checkCollision(entities[i], entities[j])) {
                // Trigger collision event
                // This will be connected to EventSystem
            }
        }
    }
}

void PhysicsSystem::setGravity(float g) {
    gravity = g;
}
