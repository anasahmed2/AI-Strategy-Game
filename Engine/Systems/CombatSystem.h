#pragma once

#include "../ECS/System.h"
#include "../ECS/Component.h"

struct CombatComponent : public Component {
    float attackDamage = 10.0f;
    float attackRange = 50.0f;
    float attackCooldown = 1.0f;
    float attackCooldownTimer = 0.0f;
    std::string damageType = "Physical";
    
    CombatComponent() = default;
};

class CombatSystem : public System {
public:
    void update(float deltaTime) override;
    void setRequiredComponents() override;
    
    // Deal damage to entity
    void dealDamage(std::shared_ptr<Entity> target, float damage);
    
    // Find nearby enemies
    std::vector<std::shared_ptr<Entity>> findTargetsInRange(std::shared_ptr<Entity> attacker);

private:
    void updateAttackCooldowns(float deltaTime);
    void performAttacks();
};
