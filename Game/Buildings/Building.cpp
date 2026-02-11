#include "Building.h"

std::shared_ptr<Entity> Building::create(std::shared_ptr<Engine> engine, Vector2 position) {
    this->engine = engine;
    
    // Create entity
    entity = engine->getRegistry()->createEntity();
    
    // Setup components
    setupComponents(position);
    
    return entity;
}

void Building::setupComponents(Vector2 position) {
    // Transform
    auto transform = std::make_shared<TransformComponent>(position);
    entity->addComponent(transform);
    
    // Health
    auto health = std::make_shared<HealthComponent>(getMaxHealth());
    entity->addComponent(health);
    
    // Collider
    auto collider = std::make_shared<ColliderComponent>(32.0f);
    entity->addComponent(collider);
    
    // Render
    auto render = std::make_shared<RenderComponent>("building");
    render->width = 64;
    render->height = 64;
    render->layer = 0;
    entity->addComponent(render);
}
