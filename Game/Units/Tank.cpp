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

    auto aiConfig = entity->getComponent<AIConfigComponent>();
    if (aiConfig) {
        aiConfig->patrolRadius = 80.0f;
        aiConfig->retreatHealthThreshold = 0.2f;
        aiConfig->engagementRange = 260.0f;
    }

    auto root = std::make_shared<Selector>();

    auto retreatSequence = std::make_shared<Sequence>();
    retreatSequence->addChild(std::make_shared<ConditionNode>([](Blackboard& blackboard) {
        float ratio = blackboard.get<float>("healthRatio");
        return ratio > 0.0f && ratio < 0.20f;
    }));
    retreatSequence->addChild(std::make_shared<ActionNode>([](Blackboard& blackboard) {
        blackboard.set("desiredAction", std::string("retreat"));
        return BehaviorStatus::Success;
    }));

    auto attackSequence = std::make_shared<Sequence>();
    attackSequence->addChild(std::make_shared<ConditionNode>([](Blackboard& blackboard) {
        return blackboard.enemySpotted;
    }));
    attackSequence->addChild(std::make_shared<ActionNode>([](Blackboard& blackboard) {
        blackboard.set("desiredAction", std::string("attack"));
        return BehaviorStatus::Success;
    }));

    auto defendAction = std::make_shared<ActionNode>([](Blackboard& blackboard) {
        blackboard.set("desiredAction", std::string("patrol"));
        return BehaviorStatus::Success;
    });

    root->addChild(retreatSequence);
    root->addChild(attackSequence);
    root->addChild(defendAction);

    ai->behaviorTree->setRoot(root);
}
