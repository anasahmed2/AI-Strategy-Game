#include "RenderSystem.h"
#include "SelectionSystem.h"
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

void RenderSystem::setSelectionSystem(std::shared_ptr<SelectionSystem> selection) {
    this->selectionSystem = selection;
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
        auto selection = entity->getComponent<SelectionComponent>();
        
        if (!render->visible) continue;
        
        // Draw as circle with color based on type
        sf::CircleShape circle(render->width / 2.0f);
        circle.setPosition(transform->position.x - render->width / 2.0f, 
                          transform->position.y - render->height / 2.0f);
        
        // Color based on sprite ID
        sf::Color color = sf::Color::White;
        if (render->spriteId == "worker") color = sf::Color::Yellow;
        else if (render->spriteId == "soldier") color = sf::Color::Green;
        else if (render->spriteId == "tank") color = sf::Color(150, 150, 150); // Gray
        else if (render->spriteId == "base") color = sf::Color::Blue;
        else if (render->spriteId == "resource_mine") color = sf::Color(255, 165, 0); // Orange
        else if (render->spriteId == "turret") color = sf::Color::Red;
        
        circle.setFillColor(color);
        
        // Add selection highlight
        if (selection && selection->isSelected) {
            circle.setOutlineColor(sf::Color::White);
            circle.setOutlineThickness(3.0f);
        } else {
            circle.setOutlineColor(sf::Color::Black);
            circle.setOutlineThickness(2.0f);
        }
        
        window->draw(circle);
        
        // Draw movement target if entity has one
        auto movement = entity->getComponent<MovementComponent>();
        if (movement && movement->hasTarget) {
            // Draw X at target position
            sf::RectangleShape line1(sf::Vector2f(20, 3));
            sf::RectangleShape line2(sf::Vector2f(20, 3));
            
            line1.setPosition(movement->targetPosition.x - 10, movement->targetPosition.y);
            line2.setPosition(movement->targetPosition.x - 10, movement->targetPosition.y);
            
            line1.setRotation(45);
            line2.setRotation(-45);
            
            line1.setFillColor(sf::Color::White);
            line2.setFillColor(sf::Color::White);
            
            window->draw(line1);
            window->draw(line2);
        }
    }
    
    // Draw UI overlay
    renderUI();
}

void RenderSystem::renderUI() {
    if (!window || !selectionSystem) return;
    
    auto selected = selectionSystem->getSelectedEntity();
    if (!selected) return;
    
    // Create simple text UI without loading font
    // Draw colored rect as UI background
    sf::RectangleShape uiPanel(sf::Vector2f(200, 80));
    uiPanel.setPosition(10, window->getSize().y - 90);
    uiPanel.setFillColor(sf::Color(0, 0, 0, 180));  // Semi-transparent black
    uiPanel.setOutlineColor(sf::Color::White);
    uiPanel.setOutlineThickness(2);
    window->draw(uiPanel);
    
    // Get entity info
    auto render = selected->getComponent<RenderComponent>();
    auto health = selected->getComponent<HealthComponent>();
    auto movement = selected->getComponent<MovementComponent>();
    
    // Draw unit type indicator (colored square)
    sf::RectangleShape typeIndicator(sf::Vector2f(30, 30));
    typeIndicator.setPosition(20, window->getSize().y - 80);
    
    if (render) {
        if (render->spriteId == "worker") typeIndicator.setFillColor(sf::Color::Yellow);
        else if (render->spriteId == "soldier") typeIndicator.setFillColor(sf::Color::Green);
        else if (render->spriteId == "tank") typeIndicator.setFillColor(sf::Color(150, 150, 150));
        else if (render->spriteId == "base") typeIndicator.setFillColor(sf::Color::Blue);
        else if (render->spriteId == "resource_mine") typeIndicator.setFillColor(sf::Color(255, 165, 0));
        else if (render->spriteId == "turret") typeIndicator.setFillColor(sf::Color::Red);
    }
    window->draw(typeIndicator);
    
    // Draw health bar
    if (health) {
        float healthPercent = health->currentHealth / health->maxHealth;
        
        // Background
        sf::RectangleShape healthBg(sf::Vector2f(150, 15));
        healthBg.setPosition(60, window->getSize().y - 75);
        healthBg.setFillColor(sf::Color(60, 60, 60));
        window->draw(healthBg);
        
        // Health
        sf::RectangleShape healthBar(sf::Vector2f(150 * healthPercent, 15));
        healthBar.setPosition(60, window->getSize().y - 75);
        
        if (healthPercent > 0.6f) healthBar.setFillColor(sf::Color::Green);
        else if (healthPercent > 0.3f) healthBar.setFillColor(sf::Color::Yellow);
        else healthBar.setFillColor(sf::Color::Red);
        
        window->draw(healthBar);
    }
    
    // Draw movement status indicator
    if (movement && movement->hasTarget) {
        sf::CircleShape movingIndicator(5);
        movingIndicator.setPosition(60, window->getSize().y - 50);
        movingIndicator.setFillColor(sf::Color::Cyan);
        window->draw(movingIndicator);
    }
}

void RenderSystem::registerShape(const std::string& spriteId, sf::Shape* shape) {
    shapes[spriteId] = shape;
}
