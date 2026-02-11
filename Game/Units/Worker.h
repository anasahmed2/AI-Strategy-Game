#pragma once

#include "Unit.h"

class Worker : public Unit {
public:
    float getMaxHealth() const override { return 50.0f; }
    float getSpeed() const override { return 80.0f; }
    float getAttackDamage() const override { return 5.0f; }
    float getAttackRange() const override { return 30.0f; }
    
protected:
    void setupComponents(Vector2 position) override;
    void setupAI() override;
};
