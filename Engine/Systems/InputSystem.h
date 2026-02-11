#pragma once

#include "../ECS/System.h"
#include "../Math/Vector2.h"
#include <SFML/Window.hpp>

class InputSystem : public System {
public:
    void update(float deltaTime) override;
    void setRequiredComponents() override;
    
    // Input state
    Vector2 getMousePosition() const;
    bool isMouseButtonPressed(int button) const;
    bool isKeyPressed(sf::Keyboard::Key key) const;
    
    // Process SFML events
    void handleEvent(const sf::Event& event);

private:
    Vector2 mousePosition;
    bool mousePressed[3] = {false, false, false};  // Left, Right, Middle
};
