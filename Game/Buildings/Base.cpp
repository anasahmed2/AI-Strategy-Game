#include "Base.h"

void Base::setupComponents(Vector2 position) {
    Building::setupComponents(position);
    
    // Base is larger
    auto render = entity->getComponent<RenderComponent>();
    if (render) {
        render->width = 96;
        render->height = 96;
        render->spriteId = "base";
    }
    
    // Add resource container for base
    auto container = std::make_shared<ResourceContainerComponent>();
    container->resources["Gold"] = 100.0f;
    container->resources["Energy"] = 50.0f;
    container->capacity["Gold"] = 1000.0f;
    container->capacity["Energy"] = 500.0f;
    entity->addComponent(container);
}
