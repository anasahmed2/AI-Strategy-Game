#include "ResourceSystem.h"

void ResourceSystem::update(float deltaTime) {
    // Process entities with ResourceCollectorComponent
    for (auto& entity : entities) {
        auto collector = entity->getComponent<ResourceCollectorComponent>();
        auto container = entity->getComponent<ResourceContainerComponent>();
        
        if (!collector || !container) continue;
        
        // Simulate gathering resources
        float collected = collector->collectionRate * deltaTime;
        
        // Add to entity's container
        auto& resources = container->resources;
        if (resources.find(collector->resourceType) == resources.end()) {
            resources[collector->resourceType] = collected;
        } else {
            resources[collector->resourceType] += collected;
        }
    }
}

void ResourceSystem::setRequiredComponents() {
    require<ResourceCollectorComponent>();
}

void ResourceSystem::addResource(const std::string& resourceType, float amount) {
    if (globalResources.find(resourceType) == globalResources.end()) {
        globalResources[resourceType] = amount;
    } else {
        globalResources[resourceType] += amount;
    }
}

void ResourceSystem::removeResource(const std::string& resourceType, float amount) {
    if (globalResources.find(resourceType) != globalResources.end()) {
        globalResources[resourceType] = std::max(0.0f, globalResources[resourceType] - amount);
    }
}

float ResourceSystem::getResource(const std::string& resourceType) const {
    auto it = globalResources.find(resourceType);
    if (it != globalResources.end()) {
        return it->second;
    }
    return 0.0f;
}

void ResourceSystem::addResourceToEntity(std::shared_ptr<Entity> entity, 
                                        const std::string& resourceType, float amount) {
    auto container = entity->getComponent<ResourceContainerComponent>();
    if (!container) return;
    
    auto& resources = container->resources;
    if (resources.find(resourceType) == resources.end()) {
        resources[resourceType] = amount;
    } else {
        resources[resourceType] += amount;
    }
}

void ResourceSystem::removeResourceFromEntity(std::shared_ptr<Entity> entity, 
                                            const std::string& resourceType, float amount) {
    auto container = entity->getComponent<ResourceContainerComponent>();
    if (!container) return;
    
    auto& resources = container->resources;
    if (resources.find(resourceType) != resources.end()) {
        resources[resourceType] = std::max(0.0f, resources[resourceType] - amount);
    }
}
