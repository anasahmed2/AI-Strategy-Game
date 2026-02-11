#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include "Entity.h"
#include "System.h"

class ComponentRegistry {
public:
    ComponentRegistry();
    
    // Entity management
    std::shared_ptr<Entity> createEntity();
    void destroyEntity(EntityID id);
    std::shared_ptr<Entity> getEntity(EntityID id);
    
    // System management
    template<typename T>
    std::shared_ptr<T> registerSystem() {
        auto system = std::make_shared<T>();
        system->setRequiredComponents();
        systems.push_back(system);
        
        // Register all existing entities with this system
        for (auto& [id, entity] : entities) {
            if (system->entityMatches(entity)) {
                system->registerEntity(entity);
            }
        }
        
        return system;
    }
    
    // Update all systems
    void update(float deltaTime);
    
    // Clean up destroyed entities
    void cleanup();
    
    // Get all entities
    const std::unordered_map<EntityID, std::shared_ptr<Entity>>& getEntities() const;
    
    size_t getEntityCount() const;
    
    // When adding component to existing entity, notify all systems
    void notifySystems(const std::shared_ptr<Entity>& entity);

private:
    EntityID nextEntityId = 1;
    std::unordered_map<EntityID, std::shared_ptr<Entity>> entities;
    std::vector<std::shared_ptr<System>> systems;
};
