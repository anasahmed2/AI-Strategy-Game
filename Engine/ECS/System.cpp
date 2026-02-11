#include "System.h"

bool System::entityMatches(const std::shared_ptr<Entity>& entity) const {
    for (const auto& requiredType : requiredComponents) {
        bool hasComponent = false;
        for (const auto& [componentType, component] : entity->getComponents()) {
            if (componentType == requiredType) {
                hasComponent = true;
                break;
            }
        }
        if (!hasComponent) {
            return false;
        }
    }
    return true;
}

void System::registerEntity(const std::shared_ptr<Entity>& entity) {
    // Check if already registered
    for (const auto& existing : entities) {
        if (existing->getId() == entity->getId()) {
            return;  // Already registered
        }
    }
    entities.push_back(entity);
}

void System::unregisterEntity(EntityID entityId) {
    auto it = std::find_if(entities.begin(), entities.end(),
        [entityId](const std::shared_ptr<Entity>& e) { return e->getId() == entityId; });
    if (it != entities.end()) {
        entities.erase(it);
    }
}

const std::vector<std::shared_ptr<Entity>>& System::getEntities() const {
    return entities;
}
