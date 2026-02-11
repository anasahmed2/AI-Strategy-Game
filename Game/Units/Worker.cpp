#include "Worker.h"

void Worker::setupComponents(Vector2 position) {
    // Call parent setup
    Unit::setupComponents(position);
    
    // Add resource collector
    auto collector = std::make_shared<ResourceCollectorComponent>();
    collector->collectionRate = 5.0f;  // 5 resources per second
    collector->resourceType = "Gold";
    entity->addComponent(collector);
    
    // Add resource container
    auto container = std::make_shared<ResourceContainerComponent>();
    container->resources["Gold"] = 0.0f;
    container->capacity["Gold"] = 50.0f;
    entity->addComponent(container);
    
    // Set render to yellow
    auto render = entity->getComponent<RenderComponent>();
    if (render) {
        render->spriteId = "worker";
    }
}

void Worker::setupAI() {
    Unit::setupAI();
    
    // Worker AI: Gather resources, flee from danger
    auto ai = entity->getComponent<AIComponent>();
    if (!ai) return;
    
    // Behavior tree setup for worker
    // Will be configured when needed
}
