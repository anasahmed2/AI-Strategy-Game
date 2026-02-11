#include "Soldier.h"

void Soldier::setupComponents(Vector2 position) {
    // Call parent setup
    Unit::setupComponents(position);
    
    // Boost combat stats for soldier
    auto combat = entity->getComponent<CombatComponent>();
    if (combat) {
        combat->attackDamage = getAttackDamage();
        combat->attackRange = getAttackRange();
        combat->attackCooldown = 1.5f;
    }
    
    // Set render color
    auto render = entity->getComponent<RenderComponent>();
    if (render) {
        render->spriteId = "soldier";
    }
}

void Soldier::setupAI() {
    Unit::setupAI();
    
    auto ai = entity->getComponent<AIComponent>();
    if (!ai) return;
    
    // Soldier AI: Attack -> Chase -> Patrol
}
