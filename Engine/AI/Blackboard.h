#pragma once

#include <any>
#include <string>
#include <unordered_map>
#include "../Math/Vector2.h"

class Blackboard {
public:
    // Set a value
    template<typename T>
    void set(const std::string& key, const T& value) {
        data[key] = std::any(value);
    }
    
    // Get a value
    template<typename T>
    T get(const std::string& key) const {
        auto it = data.find(key);
        if (it != data.end()) {
            return std::any_cast<T>(it->second);
        }
        return T();  // Return default value if not found
    }
    
    // Check if key exists
    bool has(const std::string& key) const {
        return data.find(key) != data.end();
    }
    
    // Clear all data
    void clear() {
        data.clear();
    }
    
    // AI-specific blackboard data
    Vector2 targetPosition;
    uint32_t targetEntityId = 0;
    float health = 100.0f;
    bool resourceSpotted = false;
    bool enemySpotted = false;
    Vector2 enemyPosition;

private:
    std::unordered_map<std::string, std::any> data;
};
