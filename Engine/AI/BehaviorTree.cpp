#include "BehaviorTree.h"

// ===== SELECTOR =====
BehaviorStatus Selector::execute(Blackboard& blackboard) {
    for (auto& child : children) {
        BehaviorStatus status = child->execute(blackboard);
        if (status == BehaviorStatus::Success || status == BehaviorStatus::Running) {
            return status;  // Return first successful/running child
        }
    }
    return BehaviorStatus::Failure;  // All children failed
}

void Selector::addChild(std::shared_ptr<BehaviorNode> child) {
    children.push_back(child);
}

// ===== SEQUENCE =====
BehaviorStatus Sequence::execute(Blackboard& blackboard) {
    for (auto& child : children) {
        BehaviorStatus status = child->execute(blackboard);
        if (status != BehaviorStatus::Success) {
            return status;  // Stop on first non-success
        }
    }
    return BehaviorStatus::Success;  // All children succeeded
}

void Sequence::addChild(std::shared_ptr<BehaviorNode> child) {
    children.push_back(child);
}

// ===== CONDITION NODE =====
BehaviorStatus ConditionNode::execute(Blackboard& blackboard) {
    return condition(blackboard) ? BehaviorStatus::Success : BehaviorStatus::Failure;
}

// ===== ACTION NODE =====
BehaviorStatus ActionNode::execute(Blackboard& blackboard) {
    return action(blackboard);
}

// ===== BEHAVIOR TREE =====
void BehaviorTree::setRoot(std::shared_ptr<BehaviorNode> root) {
    this->root = root;
}

BehaviorStatus BehaviorTree::update(Blackboard& blackboard) {
    if (root) {
        return root->execute(blackboard);
    }
    return BehaviorStatus::Failure;
}
