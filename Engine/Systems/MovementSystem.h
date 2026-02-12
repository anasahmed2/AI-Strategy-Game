#pragma once

#include "../ECS/System.h"
#include "../ECS/Component.h"

class MovementSystem : public System {
public:
    void update(float deltaTime) override;
    void setRequiredComponents() override;
};
