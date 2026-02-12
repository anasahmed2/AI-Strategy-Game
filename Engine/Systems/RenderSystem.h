#pragma once

#include "../ECS/System.h"
#include "../ECS/Component.h"
#include <SFML/Graphics.hpp>
#include <memory>

// Forward declaration
class SelectionSystem;

class RenderSystem : public System {
public:
    void update(float deltaTime) override;
    void setRequiredComponents() override;
    
    void setRenderTarget(sf::RenderWindow* window);
    void setSelectionSystem(std::shared_ptr<SelectionSystem> selection);
    void render();
    
    // Simple sprite/shape rendering for now
    void registerShape(const std::string& spriteId, sf::Shape* shape);

private:
    sf::RenderWindow* window = nullptr;
    std::shared_ptr<SelectionSystem> selectionSystem;
    std::unordered_map<std::string, sf::Shape*> shapes;
    sf::Font font;
    bool fontLoaded = false;
    
    void renderUI();
};
