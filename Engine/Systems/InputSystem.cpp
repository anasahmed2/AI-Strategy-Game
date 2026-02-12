#include "InputSystem.h"

void InputSystem::update(float deltaTime) {
    // Click events are consumed each frame
}

void InputSystem::setRequiredComponents() {
    // InputSystem doesn't require any components
    // It's a global input manager
}

Vector2 InputSystem::getMousePosition() const {
    return mousePosition;
}

bool InputSystem::isMouseButtonPressed(int button) const {
    if (button >= 0 && button < 3) {
        return mousePressed[button];
    }
    return false;
}

bool InputSystem::wasMouseButtonClicked(int button) {
    if (button >= 0 && button < 3) {
        bool clicked = mouseClicked[button];
        mouseClicked[button] = false;  // Consume the click
        return clicked;
    }
    return false;
}

bool InputSystem::isKeyPressed(sf::Keyboard::Key key) const {
    return sf::Keyboard::isKeyPressed(key);
}

void InputSystem::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        mousePosition = Vector2(static_cast<float>(event.mouseMove.x), 
                               static_cast<float>(event.mouseMove.y));
    } else if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button < 3) {
            mousePressed[event.mouseButton.button] = true;
            mouseClicked[event.mouseButton.button] = true;  // Register click event
            mousePosition = Vector2(static_cast<float>(event.mouseButton.x), 
                                   static_cast<float>(event.mouseButton.y));
        }
    } else if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button < 3) {
            mousePressed[event.mouseButton.button] = false;
        }
    }
}
