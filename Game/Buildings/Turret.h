#pragma once

#include "Building.h"

class Turret : public Building {
public:
    float getMaxHealth() const override { return 200.0f; }
    EntityRole getRole() const override { return EntityRole::Turret; }
    
protected:
    void setupComponents(Vector2 position) override;
};
