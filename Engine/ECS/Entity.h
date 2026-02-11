#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <typeindex>
#include <unordered_map>

using EntityID = std::uint32_t;

// Forward declarations
class Component;

class Entity {
public:
    explicit Entity(EntityID id);
    
    EntityID getId() const;
    
    // Component management
    void addComponent(std::shared_ptr<Component> component);
    void removeComponent(const std::type_index& componentType);
    
    template<typename T>
    std::shared_ptr<T> getComponent() {
        auto it = components.find(std::type_index(typeid(T)));
        if (it != components.end()) {
            return std::static_pointer_cast<T>(it->second);
        }
        return nullptr;
    }
    
    template<typename T>
    bool hasComponent() const {
        return components.find(std::type_index(typeid(T))) != components.end();
    }
    
    const std::unordered_map<std::type_index, std::shared_ptr<Component>>& getComponents() const;
    
    bool isActive() const;
    void setActive(bool active);
    
    void destroy();
    bool isDestroyed() const;

private:
    EntityID id;
    bool active = true;
    bool destroyed = false;
    std::unordered_map<std::type_index, std::shared_ptr<Component>> components;
};
