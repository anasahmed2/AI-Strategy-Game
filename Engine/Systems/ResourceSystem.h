#pragma once

#include "../ECS/System.h"
#include "../ECS/Component.h"
#include <unordered_map>

class ResourceSystem : public System {
public:
    void update(float deltaTime) override;
    void setRequiredComponents() override;
    
    // Global resource pool
    void addResource(const std::string& resourceType, float amount);
    void removeResource(const std::string& resourceType, float amount);
    float getResource(const std::string& resourceType) const;
    
    // Entity resource management
    void addResourceToEntity(std::shared_ptr<Entity> entity, 
                            const std::string& resourceType, float amount);
    void removeResourceFromEntity(std::shared_ptr<Entity> entity, 
                                 const std::string& resourceType, float amount);

private:
    std::unordered_map<std::string, float> globalResources;  // Shared pool
};
