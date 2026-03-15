#include "CombatSystem.h"

void CombatSystem::update(float deltaTime) {
    updateAttackCooldowns(deltaTime);
    performAttacks();
}

void CombatSystem::setRequiredComponents() {
    require<TransformComponent>();
    require<HealthComponent>();
    require<TeamComponent>();
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
        if (!entity || !entity->isActive() || entity->isDestroyed()) {
            continue;
        }

        auto combat = entity->getComponent<CombatComponent>();
        if (!combat || combat->attackCooldownTimer > 0.0f) continue;

        auto commandTarget = resolveCommandTarget(entity);
        if (commandTarget) {
            auto attackerTransform = entity->getComponent<TransformComponent>();
            auto targetTransform = commandTarget->getComponent<TransformComponent>();
            if (attackerTransform && targetTransform &&
                attackerTransform->position.distance(targetTransform->position) <= combat->attackRange) {
                dealDamage(commandTarget, combat->attackDamage);
                combat->attackCooldownTimer = combat->attackCooldown;
                continue;
            }
        }

        auto targets = findTargetsInRange(entity);
        if (!targets.empty()) {
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

std::vector<std::shared_ptr<Entity>> CombatSystem::findTargetsInRange(std::shared_ptr<Entity> attacker, float overrideRange) {
    std::vector<std::shared_ptr<Entity>> targets;
    
    auto attackerTransform = attacker->getComponent<TransformComponent>();
    auto attackerCombat = attacker->getComponent<CombatComponent>();
    auto attackerTeam = attacker->getComponent<TeamComponent>();
    
    if (!attackerTransform || !attackerCombat || !attackerTeam) return targets;

    float range = overrideRange > 0.0f ? overrideRange : attackerCombat->attackRange;
    
    for (auto& entity : entities) {
        if (!entity || !entity->isActive() || entity->isDestroyed()) {
            continue;
        }
        if (entity->getId() == attacker->getId()) continue;
        
        auto targetTransform = entity->getComponent<TransformComponent>();
        auto targetTeam = entity->getComponent<TeamComponent>();
        if (!targetTransform || !targetTeam) continue;
        if (targetTeam->faction == attackerTeam->faction) continue;
        if (targetTeam->faction == Faction::Neutral) continue;
        
        float distance = attackerTransform->position.distance(targetTransform->position);
        if (distance <= range) {
            targets.push_back(entity);
        }
    }
    
    return targets;
}

std::shared_ptr<Entity> CombatSystem::resolveCommandTarget(const std::shared_ptr<Entity>& attacker) {
    auto command = attacker->getComponent<CommandComponent>();
    if (!command || command->type != CommandType::Attack || command->targetEntityId == 0) {
        return nullptr;
    }

    for (auto& entity : entities) {
        if (entity && entity->getId() == command->targetEntityId && entity->isActive() && !entity->isDestroyed()) {
            return entity;
        }
    }

    return nullptr;
}
