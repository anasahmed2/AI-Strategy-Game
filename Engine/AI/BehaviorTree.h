#pragma once

#include <memory>
#include <vector>
#include <functional>
#include "Blackboard.h"

enum class BehaviorStatus {
    Running,
    Success,
    Failure
};

class BehaviorNode {
public:
    virtual ~BehaviorNode() = default;
    virtual BehaviorStatus execute(Blackboard& blackboard) = 0;
};

// ===== NODE TYPES =====

class Selector : public BehaviorNode {
public:
    BehaviorStatus execute(Blackboard& blackboard) override;
    void addChild(std::shared_ptr<BehaviorNode> child);

private:
    std::vector<std::shared_ptr<BehaviorNode>> children;
};

class Sequence : public BehaviorNode {
public:
    BehaviorStatus execute(Blackboard& blackboard) override;
    void addChild(std::shared_ptr<BehaviorNode> child);

private:
    std::vector<std::shared_ptr<BehaviorNode>> children;
};

// Condition nodes
class ConditionNode : public BehaviorNode {
public:
    explicit ConditionNode(std::function<bool(Blackboard&)> condition)
        : condition(condition) {}
    
    BehaviorStatus execute(Blackboard& blackboard) override;

private:
    std::function<bool(Blackboard&)> condition;
};

// Action nodes
class ActionNode : public BehaviorNode {
public:
    explicit ActionNode(std::function<BehaviorStatus(Blackboard&)> action)
        : action(action) {}
    
    BehaviorStatus execute(Blackboard& blackboard) override;

private:
    std::function<BehaviorStatus(Blackboard&)> action;
};

// ===== BEHAVIOR TREE =====

class BehaviorTree {
public:
    void setRoot(std::shared_ptr<BehaviorNode> root);
    BehaviorStatus update(Blackboard& blackboard);

private:
    std::shared_ptr<BehaviorNode> root;
};
