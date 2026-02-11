#pragma once

#include "Unit.h"

class Soldier : public Unit {
public:
    float getMaxHealth() const override { return 100.0f; }
    float getSpeed() const override { return 120.0f; }
    float getAttackDamage() const override { return 15.0f; }
    float getAttackRange() const override { return 60.0f; }
    
protected:
    void setupComponents(Vector2 position) override;
    void setupAI() override;
};
