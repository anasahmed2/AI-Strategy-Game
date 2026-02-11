#include "ComponentRegistry.h"

ComponentRegistry::ComponentRegistry() {
    nextEntityId = 1;
}

std::shared_ptr<Entity> ComponentRegistry::createEntity() {
    auto entity = std::make_shared<Entity>(nextEntityId++);
    entities[entity->getId()] = entity;
    notifySystems(entity);
    return entity;
}

void ComponentRegistry::destroyEntity(EntityID id) {
    auto it = entities.find(id);
    if (it != entities.end()) {
        it->second->destroy();
    }
}

std::shared_ptr<Entity> ComponentRegistry::getEntity(EntityID id) {
    auto it = entities.find(id);
    if (it != entities.end()) {
        return it->second;
    }
    return nullptr;
}

void ComponentRegistry::update(float deltaTime) {
    for (auto& system : systems) {
        system->update(deltaTime);
    }
}

void ComponentRegistry::cleanup() {
    for (auto it = entities.begin(); it != entities.end(); ) {
        if (it->second->isDestroyed()) {
            // Unregister from all systems
            for (auto& system : systems) {
                system->unregisterEntity(it->first);
            }
            it = entities.erase(it);
        } else {
            ++it;
        }
    }
}

const std::unordered_map<EntityID, std::shared_ptr<Entity>>& ComponentRegistry::getEntities() const {
    return entities;
}

size_t ComponentRegistry::getEntityCount() const {
    return entities.size();
}

void ComponentRegistry::notifySystems(const std::shared_ptr<Entity>& entity) {
    for (auto& system : systems) {
        if (system->entityMatches(entity)) {
            system->registerEntity(entity);
        }
    }
}
