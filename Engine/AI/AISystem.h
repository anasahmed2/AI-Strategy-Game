#pragma once

#include "../ECS/System.h"
#include "../ECS/Component.h"
#include "StateMachine.h"
#include "BehaviorTree.h"
#include "Blackboard.h"

struct AIComponent : public Component {
    std::shared_ptr<StateMachine> stateMachine;
    std::shared_ptr<BehaviorTree> behaviorTree;
    Blackboard blackboard;
    
    AIComponent() : stateMachine(std::make_shared<StateMachine>()),
                    behaviorTree(std::make_shared<BehaviorTree>()) {}
};

class AISystem : public System {
public:
    void update(float deltaTime) override;
    void setRequiredComponents() override;
    
    // Decision making
    void updateAIDecisions();
    void updateBlackboards();

private:
    void updateSensory(std::shared_ptr<Entity> entity);  // Check what AI sees
};
