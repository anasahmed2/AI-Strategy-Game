#pragma once

#include "Unit.h"

class Scout : public Unit {
public:
    float getMaxHealth() const override { return 70.0f; }
    float getSpeed() const override { return 170.0f; }
    float getAttackDamage() const override { return 8.0f; }
    float getAttackRange() const override { return 55.0f; }
    EntityRole getRole() const override { return EntityRole::Scout; }

protected:
    void setupComponents(Vector2 position) override;
    void setupAI() override;
};
