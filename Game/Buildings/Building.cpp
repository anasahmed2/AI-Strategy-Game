#include "Building.h"

std::shared_ptr<Entity> Building::create(std::shared_ptr<Engine> engine, Vector2 position,
                                         Faction faction, bool isAIControlled) {
    this->engine = engine;
    this->faction = faction;
    this->isAIControlled = isAIControlled;
    
    // Create entity
    entity = engine->getRegistry()->createEntity();
    
    // Setup components
    setupComponents(position);
    
    // Notify systems that this entity is ready
    engine->getRegistry()->notifySystems(entity);
    
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
    render->layer = 1;
    entity->addComponent(render);

    auto team = std::make_shared<TeamComponent>(faction, isAIControlled);
    entity->addComponent(team);

    auto role = std::make_shared<RoleComponent>(getRole());
    entity->addComponent(role);
    
    // Selection (buildings are selectable but not movable)
    auto selection = std::make_shared<SelectionComponent>(true);
    entity->addComponent(selection);

    auto command = std::make_shared<CommandComponent>();
    entity->addComponent(command);
}
