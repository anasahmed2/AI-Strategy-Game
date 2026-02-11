#include "Turret.h"

void Turret::setupComponents(Vector2 position) {
    Building::setupComponents(position);
    
    // Smaller size
    auto render = entity->getComponent<RenderComponent>();
    if (render) {
        render->width = 48;
        render->height = 48;
        render->spriteId = "turret";
    }
    
    // Combat component
    auto combat = std::make_shared<CombatComponent>();
    combat->attackDamage = 20.0f;
    combat->attackRange = 150.0f;
    combat->attackCooldown = 1.0f;
    entity->addComponent(combat);
}
