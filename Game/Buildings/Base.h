#pragma once

#include "Building.h"

class Base : public Building {
public:
    float getMaxHealth() const override { return 1000.0f; }
    EntityRole getRole() const override { return EntityRole::Base; }
    
protected:
    void setupComponents(Vector2 position) override;
};
