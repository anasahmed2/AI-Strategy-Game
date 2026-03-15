#include "ResourceMine.h"

void ResourceMine::setupComponents(Vector2 position) {
    Building::setupComponents(position);
    
    auto render = entity->getComponent<RenderComponent>();
    if (render) {
        render->spriteId = "resource_mine";
    }
    
    // Mines are neutral world resources that workers gather from.
    auto team = entity->getComponent<TeamComponent>();
    if (team) {
        team->faction = Faction::Neutral;
        team->isAIControlled = false;
    }

    auto node = std::make_shared<ResourceNodeComponent>("Gold", 3000.0f);
    entity->addComponent(node);
}
