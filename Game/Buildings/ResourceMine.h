#pragma once

#include "Building.h"

class ResourceMine : public Building {
public:
    float getMaxHealth() const override { return 300.0f; }
    
protected:
    void setupComponents(Vector2 position) override;
};
