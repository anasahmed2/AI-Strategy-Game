#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <typeindex>
#include <unordered_map>

class Event {
public:
    virtual ~Event() = default;
};

using EventCallback = std::function<void(const std::shared_ptr<Event>&)>;

class EventSystem {
public:
    // Subscribe to an event type
    template<typename T>
    void subscribe(EventCallback callback) {
        auto typeIndex = std::type_index(typeid(T));
        subscribers[typeIndex].push_back(callback);
    }
    
    // Publish an event
    template<typename T>
    void publish(const std::shared_ptr<T>& event) {
        auto typeIndex = std::type_index(typeid(T));
        auto it = subscribers.find(typeIndex);
        if (it != subscribers.end()) {
            for (auto& callback : it->second) {
                callback(event);
            }
        }
    }
    
    void clear();

private:
    std::unordered_map<std::type_index, std::vector<EventCallback>> subscribers;
};

// ===== EVENT TYPES =====

struct EntityDestroyedEvent : public Event {
    uint32_t entityId;
    EntityDestroyedEvent(uint32_t id) : entityId(id) {}
};

struct CollisionEvent : public Event {
    uint32_t entity1Id;
    uint32_t entity2Id;
    CollisionEvent(uint32_t id1, uint32_t id2) : entity1Id(id1), entity2Id(id2) {}
};

struct DamageEvent : public Event {
    uint32_t targetEntityId;
    float damage;
    DamageEvent(uint32_t target, float dmg) : targetEntityId(target), damage(dmg) {}
};

struct ResourceCollectedEvent : public Event {
    uint32_t collecterId;
    std::string resourceType;
    float amount;
    ResourceCollectedEvent(uint32_t id, const std::string& type, float amt) 
        : collecterId(id), resourceType(type), amount(amt) {}
};
