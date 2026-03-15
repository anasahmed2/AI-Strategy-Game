#include "Worker.h"

void Worker::setupComponents(Vector2 position) {
    // Call parent setup
    Unit::setupComponents(position);
    
    // Add resource collector
    auto collector = std::make_shared<ResourceCollectorComponent>();
    collector->collectionRate = 5.0f;  // 5 resources per second
    collector->resourceType = "Gold";
    entity->addComponent(collector);
    
    // Add resource container
    auto container = std::make_shared<ResourceContainerComponent>();
    container->resources["Gold"] = 0.0f;
    container->capacity["Gold"] = 50.0f;
    entity->addComponent(container);
    
    // Set render to yellow
    auto render = entity->getComponent<RenderComponent>();
    if (render) {
        render->spriteId = "worker";
    }
}

void Worker::setupAI() {
    Unit::setupAI();
    
    // Worker AI: Gather resources, flee from danger
    auto ai = entity->getComponent<AIComponent>();
    if (!ai) return;

    auto root = std::make_shared<Selector>();

    auto retreatSequence = std::make_shared<Sequence>();
    retreatSequence->addChild(std::make_shared<ConditionNode>([](Blackboard& blackboard) {
        float ratio = blackboard.get<float>("healthRatio");
        return ratio > 0.0f && ratio < 0.35f;
    }));
    retreatSequence->addChild(std::make_shared<ActionNode>([](Blackboard& blackboard) {
        blackboard.set("desiredAction", std::string("retreat"));
        return BehaviorStatus::Success;
    }));

    auto gatherSequence = std::make_shared<Sequence>();
    gatherSequence->addChild(std::make_shared<ConditionNode>([](Blackboard& blackboard) {
        return blackboard.resourceSpotted;
    }));
    gatherSequence->addChild(std::make_shared<ActionNode>([](Blackboard& blackboard) {
        blackboard.set("desiredAction", std::string("gather"));
        return BehaviorStatus::Success;
    }));

    auto patrolAction = std::make_shared<ActionNode>([](Blackboard& blackboard) {
        blackboard.set("desiredAction", std::string("patrol"));
        return BehaviorStatus::Success;
    });

    root->addChild(retreatSequence);
    root->addChild(gatherSequence);
    root->addChild(patrolAction);

    ai->behaviorTree->setRoot(root);
}
