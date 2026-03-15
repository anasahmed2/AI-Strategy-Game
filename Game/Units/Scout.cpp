#include "Scout.h"

void Scout::setupComponents(Vector2 position) {
    Unit::setupComponents(position);

    auto render = entity->getComponent<RenderComponent>();
    if (render) {
        render->spriteId = "scout";
        render->width = 28;
        render->height = 28;
    }

    auto combat = entity->getComponent<CombatComponent>();
    if (combat) {
        combat->attackCooldown = 0.9f;
        combat->attackDamage = getAttackDamage();
        combat->attackRange = getAttackRange();
    }
}

void Scout::setupAI() {
    Unit::setupAI();

    auto ai = entity->getComponent<AIComponent>();
    auto aiConfig = entity->getComponent<AIConfigComponent>();
    if (aiConfig) {
        aiConfig->patrolRadius = 220.0f;
        aiConfig->engagementRange = 260.0f;
    }

    if (!ai) {
        return;
    }

    auto root = std::make_shared<Selector>();

    auto attackSequence = std::make_shared<Sequence>();
    attackSequence->addChild(std::make_shared<ConditionNode>([](Blackboard& blackboard) {
        return blackboard.enemySpotted;
    }));
    attackSequence->addChild(std::make_shared<ActionNode>([](Blackboard& blackboard) {
        blackboard.set("desiredAction", std::string("attack"));
        return BehaviorStatus::Success;
    }));

    auto patrolAction = std::make_shared<ActionNode>([](Blackboard& blackboard) {
        blackboard.set("desiredAction", std::string("patrol"));
        return BehaviorStatus::Success;
    });

    root->addChild(attackSequence);
    root->addChild(patrolAction);
    ai->behaviorTree->setRoot(root);
}
