#pragma once

#include "../ECS/System.h"
#include "../ECS/Component.h"
#include "../Math/Vector2.h"
#include <SFML/Window.hpp>

class SelectionSystem : public System {
public:
    void update(float deltaTime) override;
    void setRequiredComponents() override;
    
    // Handle mouse click for selection
    void handleSelection(Vector2 mousePos, bool leftClick);
    
    // Get currently selected entity
    std::shared_ptr<Entity> getSelectedEntity() const;
    
    // Clear all selections
    void clearSelection();

private:
    std::shared_ptr<Entity> currentSelection;
};
