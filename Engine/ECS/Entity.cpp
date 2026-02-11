#include "Entity.h"
#include "Component.h"

Entity::Entity(EntityID id) : id(id) {}

EntityID Entity::getId() const {
    return id;
}

void Entity::addComponent(std::shared_ptr<Component> component) {
    components[std::type_index(typeid(*component))] = component;
}

void Entity::removeComponent(const std::type_index& componentType) {
    components.erase(componentType);
}

const std::unordered_map<std::type_index, std::shared_ptr<Component>>& Entity::getComponents() const {
    return components;
}

bool Entity::isActive() const {
    return active;
}

void Entity::setActive(bool active) {
    this->active = active;
}

void Entity::destroy() {
    destroyed = true;
}

bool Entity::isDestroyed() const {
    return destroyed;
}
