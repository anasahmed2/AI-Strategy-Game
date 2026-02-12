#include "SelectionSystem.h"
#include <iostream>

void SelectionSystem::update(float deltaTime) {
    // SelectionSystem is mostly event-driven, no per-frame updates needed
}

void SelectionSystem::setRequiredComponents() {
    require<TransformComponent>();
    require<SelectionComponent>();
}

void SelectionSystem::handleSelection(Vector2 mousePos, bool leftClick) {
    if (!leftClick) return;
    
    std::cout << "Click at: " << mousePos.x << ", " << mousePos.y << std::endl;
    
    // Clear previous selection
    clearSelection();
    
    // Find entity under mouse
    for (auto& entity : entities) {
        auto transform = entity->getComponent<TransformComponent>();
        auto selection = entity->getComponent<SelectionComponent>();
        auto render = entity->getComponent<RenderComponent>();
        
        if (!selection || !selection->isSelectable) continue;
        
        // Calculate distance from mouse to entity center
        float dx = mousePos.x - transform->position.x;
        float dy = mousePos.y - transform->position.y;
        float distance = std::sqrt(dx * dx + dy * dy);
        
        // Check if click is within entity bounds (use render size or default radius)
        float clickRadius = render ? render->width / 2.0f : 16.0f;
        
        if (distance <= clickRadius) {
            selection->isSelected = true;
            currentSelection = entity;
            std::cout << "Selected entity at: " << transform->position.x << ", " << transform->position.y << std::endl;
            return;  // Select first matching entity
        }
    }
    
    std::cout << "No entity selected" << std::endl;
}

std::shared_ptr<Entity> SelectionSystem::getSelectedEntity() const {
    return currentSelection;
}

void SelectionSystem::clearSelection() {
    for (auto& entity : entities) {
        auto selection = entity->getComponent<SelectionComponent>();
        if (selection) {
            selection->isSelected = false;
        }
    }
    currentSelection = nullptr;
}
