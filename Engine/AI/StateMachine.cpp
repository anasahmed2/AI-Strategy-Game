#include "StateMachine.h"

void StateMachine::update(float deltaTime) {
    if (currentState) {
        currentState->update(deltaTime);
    }
}

void StateMachine::addState(AIState stateType, std::shared_ptr<State> state) {
    states[static_cast<int>(stateType)] = state;
    
    // Set first state as current
    if (!currentState) {
        currentState = state;
        currentStateType = stateType;
        currentState->enter();
    }
}

void StateMachine::changeState(AIState newState) {
    if (currentStateType == newState) return;  // Already in this state
    
    // Exit current state
    if (currentState) {
        currentState->exit();
    }
    
    // Find and enter new state
    auto it = states.find(static_cast<int>(newState));
    if (it != states.end()) {
        currentState = it->second;
        currentStateType = newState;
        currentState->enter();
        stateChanged = true;
    }
}

AIState StateMachine::getCurrentState() const {
    return currentStateType;
}

std::shared_ptr<State> StateMachine::getCurrentStateObject() const {
    return currentState;
}
