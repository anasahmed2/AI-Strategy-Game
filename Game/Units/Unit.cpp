#include "Unit.h"

std::shared_ptr<Entity> Unit::create(std::shared_ptr<Engine> engine, Vector2 position,
                                     Faction faction, bool isAIControlled) {
    this->engine = engine;
    this->faction = faction;
    this->isAIControlled = isAIControlled;
    
    // Create entity
    entity = engine->getRegistry()->createEntity();
    
    // Setup components
    setupComponents(position);
    
    // Setup AI
    setupAI();
    
    // Notify systems that this entity is ready
    engine->getRegistry()->notifySystems(entity);
    
    return entity;
}

void Unit::setupComponents(Vector2 position) {
    // Transform
    auto transform = std::make_shared<TransformComponent>(position);
    entity->addComponent(transform);
    
    // Physics
    auto physics = std::make_shared<PhysicsComponent>();
    entity->addComponent(physics);
    
    // Health
    auto health = std::make_shared<HealthComponent>(getMaxHealth());
    entity->addComponent(health);
    
    // Collider
    auto collider = std::make_shared<ColliderComponent>(16.0f);
    entity->addComponent(collider);
    
    // Render
    auto render = std::make_shared<RenderComponent>("unit");
    render->width = 32;
    render->height = 32;
    render->layer = 2;
    entity->addComponent(render);

    auto team = std::make_shared<TeamComponent>(faction, isAIControlled);
    entity->addComponent(team);

    auto role = std::make_shared<RoleComponent>(getRole());
    entity->addComponent(role);
    
    // Combat
    auto combat = std::make_shared<CombatComponent>();
    combat->attackDamage = getAttackDamage();
    combat->attackRange = getAttackRange();
    entity->addComponent(combat);
    
    // Selection (units are selectable)
    auto selection = std::make_shared<SelectionComponent>(true);
    entity->addComponent(selection);
    
    // Movement
    auto movement = std::make_shared<MovementComponent>(100.0f);  // Default speed
    movement->moveSpeed = getSpeed();
    entity->addComponent(movement);

    auto path = std::make_shared<PathComponent>();
    entity->addComponent(path);

    auto command = std::make_shared<CommandComponent>();
    entity->addComponent(command);
}

void Unit::setupAI() {
    // Add AI component with FSM and BT
    auto ai = std::make_shared<AIComponent>();
    entity->addComponent(ai);

    auto aiConfig = std::make_shared<AIConfigComponent>();
    aiConfig->enabled = isAIControlled;
    auto transform = entity->getComponent<TransformComponent>();
    if (transform) {
        aiConfig->patrolCenter = transform->position;
    }
    entity->addComponent(aiConfig);
    
    // Setup basic behavior tree
    // This is a placeholder - actual implementation in subclasses
}
