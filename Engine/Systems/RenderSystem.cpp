#include "RenderSystem.h"
#include <algorithm>

void RenderSystem::update(float deltaTime) {
    // Rendering happens in render() call
}

void RenderSystem::setRequiredComponents() {
    require<TransformComponent>();
    require<RenderComponent>();
}

void RenderSystem::setRenderTarget(sf::RenderWindow* window) {
    this->window = window;
}

void RenderSystem::render() {
    if (!window) return;
    
    // Sort entities by layer
    auto sortedEntities = entities;
    std::sort(sortedEntities.begin(), sortedEntities.end(),
        [](const std::shared_ptr<Entity>& a, const std::shared_ptr<Entity>& b) {
            auto renderA = a->getComponent<RenderComponent>();
            auto renderB = b->getComponent<RenderComponent>();
            if (!renderA || !renderB) return false;
            return renderA->layer < renderB->layer;
        });
    
    // Draw each entity
    for (auto& entity : sortedEntities) {
        auto transform = entity->getComponent<TransformComponent>();
        auto render = entity->getComponent<RenderComponent>();
        
        if (!render->visible) continue;
        
        // Draw as circle for now (simple debug rendering)
        sf::CircleShape circle(render->width / 2.0f);
        circle.setPosition(transform->position.x, transform->position.y);
        circle.setFillColor(sf::Color::Blue);
        
        window->draw(circle);
    }
}

void RenderSystem::registerShape(const std::string& spriteId, sf::Shape* shape) {
    shapes[spriteId] = shape;
}
