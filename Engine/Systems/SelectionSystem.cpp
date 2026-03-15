#include "SelectionSystem.h"
#include <iostream>
#include <algorithm>
#include <cmath>

void SelectionSystem::update(float deltaTime) {
    // SelectionSystem is mostly event-driven, no per-frame updates needed
}

void SelectionSystem::setRequiredComponents() {
    require<TransformComponent>();
    require<SelectionComponent>();
}

void SelectionSystem::handleSelection(Vector2 mousePos, bool leftClick, bool additive) {
    if (!leftClick) return;

    if (!additive) {
        clearSelection();
    }
    
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
            bool alreadySelected = std::any_of(selectedEntities.begin(), selectedEntities.end(),
                [&entity](const std::shared_ptr<Entity>& existing) {
                    return existing && existing->getId() == entity->getId();
                });
            if (!alreadySelected) {
                selectedEntities.push_back(entity);
            }
            return;  // Select first matching entity
        }
    }
}

void SelectionSystem::handleBoxSelection(Vector2 start, Vector2 end, bool additive) {
    float minX = std::min(start.x, end.x);
    float maxX = std::max(start.x, end.x);
    float minY = std::min(start.y, end.y);
    float maxY = std::max(start.y, end.y);

    if (!additive) {
        clearSelection();
    }

    for (auto& entity : entities) {
        auto transform = entity->getComponent<TransformComponent>();
        auto selection = entity->getComponent<SelectionComponent>();
        auto role = entity->getComponent<RoleComponent>();
        auto team = entity->getComponent<TeamComponent>();
        if (!transform || !selection || !selection->isSelectable || !team || !role) {
            continue;
        }

        if (team->faction != Faction::Player) {
            continue;
        }

        if (role->role == EntityRole::Base || role->role == EntityRole::Turret || role->role == EntityRole::ResourceMine) {
            continue;
        }

        const Vector2& pos = transform->position;
        if (pos.x >= minX && pos.x <= maxX && pos.y >= minY && pos.y <= maxY) {
            selection->isSelected = true;
            bool alreadySelected = std::any_of(selectedEntities.begin(), selectedEntities.end(),
                [&entity](const std::shared_ptr<Entity>& existing) {
                    return existing && existing->getId() == entity->getId();
                });
            if (!alreadySelected) {
                selectedEntities.push_back(entity);
            }
        }
    }

    if (!selectedEntities.empty()) {
        currentSelection = selectedEntities.front();
    }
}

std::shared_ptr<Entity> SelectionSystem::getSelectedEntity() const {
    return currentSelection;
}

const std::vector<std::shared_ptr<Entity>>& SelectionSystem::getSelectedEntities() const {
    return selectedEntities;
}

void SelectionSystem::clearSelection() {
    for (auto& entity : entities) {
        auto selection = entity->getComponent<SelectionComponent>();
        if (selection) {
            selection->isSelected = false;
        }
    }
    currentSelection = nullptr;
    selectedEntities.clear();
}
