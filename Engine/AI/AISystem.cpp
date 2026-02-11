#include "AISystem.h"

void AISystem::update(float deltaTime) {
    updateBlackboards();
    updateAIDecisions();
    
    // Update state machines
    for (auto& entity : entities) {
        auto ai = entity->getComponent<AIComponent>();
        if (!ai) continue;
        
        // Update behavior tree
        ai->behaviorTree->update(ai->blackboard);
        
        // Update state machine
        ai->stateMachine->update(deltaTime);
    }
}

void AISystem::setRequiredComponents() {
    require<AIComponent>();
    require<TransformComponent>();
}

void AISystem::updateBlackboards() {
    for (auto& entity : entities) {
        auto ai = entity->getComponent<AIComponent>();
        auto transform = entity->getComponent<TransformComponent>();
        auto health = entity->getComponent<HealthComponent>();
        
        if (!ai || !transform) continue;
        
        // Update basic info
        ai->blackboard.targetPosition = transform->position;
        if (health) {
            ai->blackboard.health = health->currentHealth;
        }
        
        // Update sensory information
        updateSensory(entity);
    }
}

void AISystem::updateAIDecisions() {
    // This is where behavior tree decisions get executed
    // Actual decision-making will be implemented in game layer
}

void AISystem::updateSensory(std::shared_ptr<Entity> entity) {
    auto ai = entity->getComponent<AIComponent>();
    auto transform = entity->getComponent<TransformComponent>();
    
    if (!ai || !transform) return;
    
    // Reset sensory data
    ai->blackboard.enemySpotted = false;
    ai->blackboard.resourceSpotted = false;
    
    // Simple vision system - detect nearby entities within vision radius
    float visionRange = 200.0f;
    
    // Check all visible entities (detailed implementation in game layer)
}
