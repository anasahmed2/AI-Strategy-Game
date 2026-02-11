#include "Tank.h"

void Tank::setupComponents(Vector2 position) {
    // Call parent setup
    Unit::setupComponents(position);
    
    // Increase size
    auto render = entity->getComponent<RenderComponent>();
    if (render) {
        render->width = 48;
        render->height = 48;
        render->spriteId = "tank";
    }
    
    // Boost combat stats for tank
    auto combat = entity->getComponent<CombatComponent>();
    if (combat) {
        combat->attackDamage = getAttackDamage();
        combat->attackRange = getAttackRange();
        combat->attackCooldown = 2.0f;
    }
    
    // Tank size collider
    auto collider = entity->getComponent<ColliderComponent>();
    if (collider) {
        collider->radius = 24.0f;
    }
}

void Tank::setupAI() {
    Unit::setupAI();
    
    auto ai = entity->getComponent<AIComponent>();
    if (!ai) return;
    
    // Tank AI: Defend position -> Attack -> Hold ground
}
