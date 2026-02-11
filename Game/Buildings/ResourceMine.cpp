#include "ResourceMine.h"

void ResourceMine::setupComponents(Vector2 position) {
    Building::setupComponents(position);
    
    auto render = entity->getComponent<RenderComponent>();
    if (render) {
        render->spriteId = "resourcemine";
    }
    
    // Resource collector to generate resources
    auto collector = std::make_shared<ResourceCollectorComponent>();
    collector->collectionRate = 10.0f;
    collector->resourceType = "Gold";
    entity->addComponent(collector);
}
