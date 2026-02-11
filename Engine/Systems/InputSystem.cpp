#include "InputSystem.h"

void InputSystem::update(float deltaTime) {
    // Reset mouse button states each frame
    // Real implementation would track mouse down vs just clicked
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
        }
    } else if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button < 3) {
            mousePressed[event.mouseButton.button] = false;
        }
    }
}
