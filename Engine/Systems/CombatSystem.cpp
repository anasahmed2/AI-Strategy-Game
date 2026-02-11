#include "CombatSystem.h"

void CombatSystem::update(float deltaTime) {
    updateAttackCooldowns(deltaTime);
    performAttacks();
}

void CombatSystem::setRequiredComponents() {
    require<TransformComponent>();
    require<CombatComponent>();
    require<HealthComponent>();
}

void CombatSystem::updateAttackCooldowns(float deltaTime) {
    for (auto& entity : entities) {
        auto combat = entity->getComponent<CombatComponent>();
        if (combat && combat->attackCooldownTimer > 0.0f) {
            combat->attackCooldownTimer -= deltaTime;
        }
    }
}

void CombatSystem::performAttacks() {
    for (auto& entity : entities) {
        auto combat = entity->getComponent<CombatComponent>();
        if (!combat || combat->attackCooldownTimer > 0.0f) continue;
        
        auto targets = findTargetsInRange(entity);
        if (!targets.empty()) {
            // Attack first target
            dealDamage(targets[0], combat->attackDamage);
            combat->attackCooldownTimer = combat->attackCooldown;
        }
    }
}

void CombatSystem::dealDamage(std::shared_ptr<Entity> target, float damage) {
    auto health = target->getComponent<HealthComponent>();
    if (!health) return;
    
    health->currentHealth -= damage;
    
    if (health->currentHealth <= 0.0f) {
        target->destroy();
    }
}

std::vector<std::shared_ptr<Entity>> CombatSystem::findTargetsInRange(std::shared_ptr<Entity> attacker) {
    std::vector<std::shared_ptr<Entity>> targets;
    
    auto attackerTransform = attacker->getComponent<TransformComponent>();
    auto attackerCombat = attacker->getComponent<CombatComponent>();
    
    if (!attackerTransform || !attackerCombat) return targets;
    
    for (auto& entity : entities) {
        if (entity->getId() == attacker->getId()) continue;  // Don't target self
        
        auto targetTransform = entity->getComponent<TransformComponent>();
        if (!targetTransform) continue;
        
        float distance = attackerTransform->position.distance(targetTransform->position);
        if (distance <= attackerCombat->attackRange) {
            targets.push_back(entity);
        }
    }
    
    return targets;
}
