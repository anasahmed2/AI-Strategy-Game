#pragma once

#include "../ECS/System.h"
#include "../ECS/Component.h"
#include <SFML/Graphics.hpp>

class RenderSystem : public System {
public:
    void update(float deltaTime) override;
    void setRequiredComponents() override;
    
    void setRenderTarget(sf::RenderWindow* window);
    void render();
    
    // Simple sprite/shape rendering for now
    void registerShape(const std::string& spriteId, sf::Shape* shape);

private:
    sf::RenderWindow* window = nullptr;
    std::unordered_map<std::string, sf::Shape*> shapes;
};
