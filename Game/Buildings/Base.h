#pragma once

#include "Building.h"

class Base : public Building {
public:
    float getMaxHealth() const override { return 1000.0f; }
    
protected:
    void setupComponents(Vector2 position) override;
};
