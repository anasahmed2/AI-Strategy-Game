#pragma once

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

enum class AIState {
    Idle,
    Move,
    Attack,
    Flee,
    Gather,
    Build,
    Die
};

class State {
public:
    virtual ~State() = default;
    virtual void enter() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void exit() = 0;
    virtual AIState getStateType() const = 0;
};

class StateMachine {
public:
    void update(float deltaTime);
    
    void addState(AIState stateType, std::shared_ptr<State> state);
    void changeState(AIState newState);
    
    AIState getCurrentState() const;
    std::shared_ptr<State> getCurrentStateObject() const;

private:
    std::unordered_map<int, std::shared_ptr<State>> states;
    std::shared_ptr<State> currentState = nullptr;
    AIState currentStateType = AIState::Idle;
    bool stateChanged = false;
};
