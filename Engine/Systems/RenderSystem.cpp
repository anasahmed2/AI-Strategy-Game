#include "RenderSystem.h"
#include "SelectionSystem.h"
#include "../AI/AISystem.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <vector>

void RenderSystem::update(float deltaTime) {
    lastDeltaTime = deltaTime;
}

void RenderSystem::setRequiredComponents() {
    require<TransformComponent>();
    require<RenderComponent>();
}

void RenderSystem::setRenderTarget(sf::RenderWindow* window) {
    this->window = window;
    loadDebugFont();
}

void RenderSystem::setSelectionSystem(std::shared_ptr<SelectionSystem> selection) {
    this->selectionSystem = selection;
}

void RenderSystem::render() {
    if (!window) return;

    // Terrain backdrop.
    const int tile = 64;
    for (unsigned int y = 0; y < window->getSize().y; y += tile) {
        for (unsigned int x = 0; x < window->getSize().x; x += tile) {
            sf::RectangleShape ground(sf::Vector2f(static_cast<float>(tile), static_cast<float>(tile)));
            ground.setPosition(static_cast<float>(x), static_cast<float>(y));

            bool even = ((x / tile) + (y / tile)) % 2 == 0;
            ground.setFillColor(even ? sf::Color(60, 96, 52) : sf::Color(52, 86, 46));
            window->draw(ground);
        }
    }

    sf::RectangleShape waterA(sf::Vector2f(250.0f, 100.0f));
    waterA.setPosition(420.0f, 120.0f);
    waterA.setFillColor(sf::Color(40, 80, 130, 200));
    window->draw(waterA);

    sf::RectangleShape waterB(sf::Vector2f(180.0f, 90.0f));
    waterB.setPosition(760.0f, 470.0f);
    waterB.setFillColor(sf::Color(35, 70, 120, 200));
    window->draw(waterB);
    
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
        auto health = entity->getComponent<HealthComponent>();
        auto team = entity->getComponent<TeamComponent>();
        auto role = entity->getComponent<RoleComponent>();
        auto command = entity->getComponent<CommandComponent>();
        auto path = entity->getComponent<PathComponent>();
        
        if (!transform || !render || !render->visible) continue;
        
        // Draw as circle with color based on type
        sf::CircleShape circle(render->width / 2.0f);
        circle.setPosition(transform->position.x - render->width / 2.0f, 
                          transform->position.y - render->height / 2.0f);
        
        // Color based on sprite ID
        sf::Color color = sf::Color::White;
        if (render->spriteId == "worker") color = sf::Color::Yellow;
        else if (render->spriteId == "soldier") color = sf::Color::Green;
        else if (render->spriteId == "tank") color = sf::Color(150, 150, 150); // Gray
        else if (render->spriteId == "scout") color = sf::Color(120, 235, 255);
        else if (render->spriteId == "base") color = sf::Color::Blue;
        else if (render->spriteId == "resource_mine") color = sf::Color(255, 165, 0); // Orange
        else if (render->spriteId == "turret") color = sf::Color::Red;
        else if (render->spriteId == "tree") color = sf::Color(72, 50, 35);

        // Apply faction tint for better readability.
        if (team && team->faction == Faction::Enemy && role && role->role != EntityRole::ResourceMine) {
            color = sf::Color(static_cast<sf::Uint8>(std::min(255, color.r + 40)),
                              static_cast<sf::Uint8>(color.g * 0.6f),
                              static_cast<sf::Uint8>(color.b * 0.6f));
        }
        
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

        if (health) {
            float ratio = health->maxHealth > 0.0f ? health->currentHealth / health->maxHealth : 0.0f;
            ratio = std::max(0.0f, std::min(1.0f, ratio));

            sf::RectangleShape hpBg(sf::Vector2f(28.0f, 4.0f));
            hpBg.setPosition(transform->position.x - 14.0f, transform->position.y - render->height * 0.7f);
            hpBg.setFillColor(sf::Color(30, 30, 30, 220));
            window->draw(hpBg);

            sf::RectangleShape hpFill(sf::Vector2f(28.0f * ratio, 4.0f));
            hpFill.setPosition(transform->position.x - 14.0f, transform->position.y - render->height * 0.7f);
            hpFill.setFillColor(ratio > 0.6f ? sf::Color::Green : (ratio > 0.3f ? sf::Color::Yellow : sf::Color::Red));
            window->draw(hpFill);
        }
        
        // Draw command and path overlays.
        if (path && path->hasPath() && selection && selection->isSelected) {
            sf::VertexArray lines(sf::LineStrip, path->waypoints.size() + 1);
            lines[0].position = sf::Vector2f(transform->position.x, transform->position.y);
            lines[0].color = sf::Color(180, 220, 255, 180);
            for (std::size_t i = 0; i < path->waypoints.size(); ++i) {
                lines[i + 1].position = sf::Vector2f(path->waypoints[i].x, path->waypoints[i].y);
                lines[i + 1].color = sf::Color(180, 220, 255, 180);

                sf::CircleShape node(3.0f);
                node.setOrigin(3.0f, 3.0f);
                node.setPosition(path->waypoints[i].x, path->waypoints[i].y);
                node.setFillColor(sf::Color(190, 230, 255));
                window->draw(node);
            }
            window->draw(lines);
        }

        if (command && command->type != CommandType::None) {
            Vector2 marker = command->targetPosition;
            sf::Color markerColor = sf::Color::White;
            if (command->type == CommandType::Attack) markerColor = sf::Color(255, 80, 80);
            if (command->type == CommandType::Gather) markerColor = sf::Color(255, 220, 80);
            if (command->type == CommandType::Defend) markerColor = sf::Color(120, 220, 255);

            // Draw X at target position
            sf::RectangleShape line1(sf::Vector2f(20, 3));
            sf::RectangleShape line2(sf::Vector2f(20, 3));
            
            line1.setPosition(marker.x - 10, marker.y);
            line2.setPosition(marker.x - 10, marker.y);
            
            line1.setRotation(45);
            line2.setRotation(-45);
            
            line1.setFillColor(markerColor);
            line2.setFillColor(markerColor);
            
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
    
    // Draw colored rect as UI background
    sf::RectangleShape uiPanel(sf::Vector2f(310, 132));
    uiPanel.setPosition(10, window->getSize().y - 142);
    uiPanel.setFillColor(sf::Color(0, 0, 0, 180));  // Semi-transparent black
    uiPanel.setOutlineColor(sf::Color::White);
    uiPanel.setOutlineThickness(2);
    window->draw(uiPanel);
    
    // Get entity info
    auto render = selected->getComponent<RenderComponent>();
    auto health = selected->getComponent<HealthComponent>();
    auto movement = selected->getComponent<MovementComponent>();
    auto command = selected->getComponent<CommandComponent>();
    auto path = selected->getComponent<PathComponent>();
    auto ai = selected->getComponent<AIComponent>();
    
    // Draw unit type indicator (colored square)
    sf::RectangleShape typeIndicator(sf::Vector2f(30, 30));
    typeIndicator.setPosition(20, window->getSize().y - 126);
    
    if (render) {
        if (render->spriteId == "worker") typeIndicator.setFillColor(sf::Color::Yellow);
        else if (render->spriteId == "soldier") typeIndicator.setFillColor(sf::Color::Green);
        else if (render->spriteId == "tank") typeIndicator.setFillColor(sf::Color(150, 150, 150));
        else if (render->spriteId == "scout") typeIndicator.setFillColor(sf::Color(120, 235, 255));
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
        healthBg.setPosition(60, window->getSize().y - 120);
        healthBg.setFillColor(sf::Color(60, 60, 60));
        window->draw(healthBg);
        
        // Health
        sf::RectangleShape healthBar(sf::Vector2f(150 * healthPercent, 15));
        healthBar.setPosition(60, window->getSize().y - 120);
        
        if (healthPercent > 0.6f) healthBar.setFillColor(sf::Color::Green);
        else if (healthPercent > 0.3f) healthBar.setFillColor(sf::Color::Yellow);
        else healthBar.setFillColor(sf::Color::Red);
        
        window->draw(healthBar);
    }
    
    // Draw movement status indicator
    if (movement && movement->hasTarget) {
        sf::CircleShape movingIndicator(5);
        movingIndicator.setPosition(60, window->getSize().y - 96);
        movingIndicator.setFillColor(sf::Color::Cyan);
        window->draw(movingIndicator);
    }

    // Developer overlay text.
    if (fontLoaded) {
        auto makeLine = [&](const std::string& text, float y) {
            sf::Text line;
            line.setFont(font);
            line.setString(text);
            line.setCharacterSize(14);
            line.setFillColor(sf::Color::White);
            line.setPosition(64.0f, y);
            window->draw(line);
        };

        float fps = lastDeltaTime > 0.0001f ? (1.0f / lastDeltaTime) : 0.0f;
        std::ostringstream stateLine;
        stateLine << "FPS: " << static_cast<int>(fps);
        makeLine(stateLine.str(), static_cast<float>(window->getSize().y - 140));

        std::string commandText = "None";
        if (command) {
            if (command->type == CommandType::Move) commandText = "Move";
            else if (command->type == CommandType::Attack) commandText = "Attack";
            else if (command->type == CommandType::Gather) commandText = "Gather";
            else if (command->type == CommandType::Defend) commandText = "Defend";
            else if (command->type == CommandType::ReturnToBase) commandText = "Return";
        }
        makeLine("Cmd: " + commandText, static_cast<float>(window->getSize().y - 122));

        if (ai && ai->stateMachine) {
            AIState state = ai->stateMachine->getCurrentState();
            std::string stateText = "Idle";
            if (state == AIState::Move) stateText = "Move";
            else if (state == AIState::Attack) stateText = "Attack";
            else if (state == AIState::Flee) stateText = "Flee";
            else if (state == AIState::Gather) stateText = "Gather";
            makeLine("State: " + stateText, static_cast<float>(window->getSize().y - 104));
        }

        std::ostringstream targetLine;
        targetLine << "Target: " << (command ? command->targetEntityId : 0);
        makeLine(targetLine.str(), static_cast<float>(window->getSize().y - 86));

        std::ostringstream pathLine;
        pathLine << "Path nodes: " << (path ? static_cast<int>(path->waypoints.size()) : 0);
        makeLine(pathLine.str(), static_cast<float>(window->getSize().y - 68));
    }
}

void RenderSystem::registerShape(const std::string& spriteId, sf::Shape* shape) {
    shapes[spriteId] = shape;
}

void RenderSystem::loadDebugFont() {
    if (fontLoaded) {
        return;
    }

    const std::vector<std::string> candidates = {
        "C:/Windows/Fonts/consola.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf"
    };

    for (const auto& path : candidates) {
        if (font.loadFromFile(path)) {
            fontLoaded = true;
            return;
        }
    }
}
