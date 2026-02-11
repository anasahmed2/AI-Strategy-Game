#include "RenderSystem.h"
#include <iostream>
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
        
        // Draw as circle with color based on type
        sf::CircleShape circle(render->width / 2.0f);
        circle.setPosition(transform->position.x, transform->position.y);
        
        // Color based on sprite ID
        sf::Color color = sf::Color::White;
        if (render->spriteId == "worker") color = sf::Color::Yellow;
        else if (render->spriteId == "soldier") color = sf::Color::Green;
        else if (render->spriteId == "tank") color = sf::Color(150, 150, 150); // Gray
        else if (render->spriteId == "base") color = sf::Color::Blue;
        else if (render->spriteId == "resource_mine") color = sf::Color(255, 165, 0); // Orange
        else if (render->spriteId == "turret") color = sf::Color::Red;
        
        circle.setFillColor(color);
        circle.setOutlineColor(sf::Color::Black);
        circle.setOutlineThickness(2.0f);
        
        window->draw(circle);
    }
}

void RenderSystem::registerShape(const std::string& spriteId, sf::Shape* shape) {
    shapes[spriteId] = shape;
}
