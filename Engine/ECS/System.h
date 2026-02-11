#pragma once

#include <vector>
#include <memory>
#include <typeindex>
#include <unordered_set>
#include <algorithm>
#include "Entity.h"

class System {
public:
    virtual ~System() = default;
    
    virtual void update(float deltaTime) = 0;
    
    // Define which components this system needs
    virtual void setRequiredComponents() = 0;
    
    // Check if entity has all required components
    bool entityMatches(const std::shared_ptr<Entity>& entity) const;
    
    // Register entity with this system
    void registerEntity(const std::shared_ptr<Entity>& entity);
    
    // Unregister entity
    void unregisterEntity(EntityID entityId);
    
    // Get all matching entities
    const std::vector<std::shared_ptr<Entity>>& getEntities() const;

protected:
    std::unordered_set<std::type_index> requiredComponents;
    std::vector<std::shared_ptr<Entity>> entities;
    
    // Helper to add required component type
    template<typename T>
    void require() {
        requiredComponents.insert(std::type_index(typeid(T)));
    }
};
