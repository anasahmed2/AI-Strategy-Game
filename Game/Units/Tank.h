#pragma once

#include "Unit.h"

class Tank : public Unit {
public:
    float getMaxHealth() const override { return 250.0f; }
    float getSpeed() const override { return 60.0f; }
    float getAttackDamage() const override { return 25.0f; }
    float getAttackRange() const override { return 70.0f; }
    
protected:
    void setupComponents(Vector2 position) override;
    void setupAI() override;
};
