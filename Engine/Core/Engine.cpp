#include "Engine.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <limits>

Engine::Engine(int windowWidth, int windowHeight, const std::string& title) {
    // Create window
    window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode(windowWidth, windowHeight), title);
    window->setFramerateLimit(60);
    
    // Initialize registry
    registry = std::make_shared<ComponentRegistry>();
    
    // Initialize systems
    inputSystem = std::make_shared<InputSystem>();
    renderSystem = registry->registerSystem<RenderSystem>();
    physicsSystem = registry->registerSystem<PhysicsSystem>();
    resourceSystem = registry->registerSystem<ResourceSystem>();
    combatSystem = registry->registerSystem<CombatSystem>();
    selectionSystem = registry->registerSystem<SelectionSystem>();
    movementSystem = registry->registerSystem<MovementSystem>();
    aiSystem = registry->registerSystem<AISystem>();
    
    // Event system (standalone)
    eventSystem = std::make_shared<EventSystem>();
    
    // Pathfinding
    pathfinder = std::make_shared<Pathfinder>(
        windowWidth / 32, windowHeight / 32, 32.0f);
    
    // Connect render system to window
    renderSystem->setRenderTarget(window.get());
    renderSystem->setSelectionSystem(selectionSystem);
    renderSystem->setResourceSystem(resourceSystem);
    
    // Reset clock
    clock.restart();

    // Sound system (procedural — works without audio files)
    soundSystem = std::make_shared<SoundSystem>();
}

Engine::~Engine() {
    if (window) {
        window->close();
    }
}

bool Engine::isRunning() const {
    return window && window->isOpen();
}

void Engine::pollEvents() {
    inputSystem->update(0.0f);

    sf::Event event;
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window->close();
        }

        // Pass events to input system
        inputSystem->handleEvent(event);

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            leftDragInProgress = true;
            leftDragStart = Vector2(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        }

        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            Vector2 releasePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
            bool additive = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) ||
                            sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);

            float dragDistance = leftDragStart.distance(releasePos);
            if (dragDistance > 12.0f) {
                selectionSystem->handleBoxSelection(leftDragStart, releasePos, additive);
            } else {
                selectionSystem->handleSelection(releasePos, true, additive);
            }

            leftDragInProgress = false;
            // Play selection sound
            if (soundSystem) soundSystem->playSelect();
        }

        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right) {
            Vector2 clickPos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
            auto clickedEntity = getEntityAtPoint(clickPos);

            auto selectedEntities = selectionSystem->getSelectedEntities();
            if (selectedEntities.empty()) {
                auto single = selectionSystem->getSelectedEntity();
                if (single) {
                    selectedEntities.push_back(single);
                }
            }

            bool defendCommand = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) ||
                                 sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);

            for (const auto& selected : selectedEntities) {
                if (!selected || selected->isDestroyed()) {
                    continue;
                }

                auto selectedTeam = selected->getComponent<TeamComponent>();
                auto selectedRole = selected->getComponent<RoleComponent>();
                auto selectedCommand = selected->getComponent<CommandComponent>();
                if (!selectedTeam || !selectedCommand || !selectedRole) {
                    continue;
                }
                if (selectedTeam->faction != Faction::Player) {
                    continue;
                }

                bool issued = false;
                if (clickedEntity && !clickedEntity->isDestroyed()) {
                    auto clickedTeam = clickedEntity->getComponent<TeamComponent>();
                    auto clickedRole = clickedEntity->getComponent<RoleComponent>();
                    auto clickedTransform = clickedEntity->getComponent<TransformComponent>();

                    if (clickedTeam && clickedTransform && clickedTeam->faction != Faction::Player &&
                        clickedTeam->faction != Faction::Neutral && selected->hasComponent<CombatComponent>()) {
                        selectedCommand->type = CommandType::Attack;
                        selectedCommand->targetEntityId = clickedEntity->getId();
                        selectedCommand->targetPosition = clickedTransform->position;
                        assignPathToEntity(selected, clickedTransform->position);
                        issued = true;
                    } else if (clickedRole && clickedRole->role == EntityRole::ResourceMine &&
                               selected->hasComponent<ResourceCollectorComponent>()) {
                        selectedCommand->type = CommandType::Gather;
                        selectedCommand->targetEntityId = clickedEntity->getId();
                        selectedCommand->targetPosition = clickedTransform->position;
                        assignPathToEntity(selected, clickedTransform->position);
                        issued = true;
                    }
                }

                if (!issued && selectedRole->role != EntityRole::Base && selectedRole->role != EntityRole::Turret) {
                    selectedCommand->type = defendCommand ? CommandType::Defend : CommandType::Move;
                    selectedCommand->targetEntityId = 0;
                    selectedCommand->targetPosition = clickPos;
                    selectedCommand->defendPosition = clickPos;
                    assignPathToEntity(selected, clickPos);
                }
            }
            // Play command sound
            if (soundSystem && !selectedEntities.empty()) {
                bool anyAttack = false;
                bool anyGather = false;
                for (auto& e : selectedEntities) {
                    auto cmd = e->getComponent<CommandComponent>();
                    if (cmd && cmd->type == CommandType::Attack) { anyAttack = true; break; }
                    if (cmd && cmd->type == CommandType::Gather) { anyGather = true; }
                }
                if (anyAttack) soundSystem->playAttack();
                else if (anyGather) soundSystem->playGather();
                else          soundSystem->playMove();
            }
        }
    }
}

void Engine::update(float deltaTime) {
    rebuildPathGrid();

    // Update all systems
    registry->update(deltaTime);
    registry->cleanup();
}

void Engine::render() {
    window->clear(sf::Color(40, 40, 40));  // Dark gray background
    
    // Render all entities
    renderSystem->render();

    if (leftDragInProgress) {
        Vector2 mouse = inputSystem->getMousePosition();
        float minX = std::min(leftDragStart.x, mouse.x);
        float minY = std::min(leftDragStart.y, mouse.y);
        float width = std::abs(leftDragStart.x - mouse.x);
        float height = std::abs(leftDragStart.y - mouse.y);

        sf::RectangleShape box(sf::Vector2f(width, height));
        box.setPosition(minX, minY);
        box.setFillColor(sf::Color(100, 170, 255, 40));
        box.setOutlineThickness(1.0f);
        box.setOutlineColor(sf::Color(150, 200, 255, 160));
        window->draw(box);
    }
    
    window->display();
}

float Engine::getDeltaTime() const {
    return deltaTime;
}

void Engine::rebuildPathGrid() {
    if (!pathfinder) {
        return;
    }

    pathfinder->clearGrid();

    constexpr float cellSize = 32.0f;
    int gridWidth = pathfinder->getGridWidth();
    int gridHeight = pathfinder->getGridHeight();

    for (const auto& [id, entity] : registry->getEntities()) {
        if (!entity || !entity->isActive() || entity->isDestroyed()) {
            continue;
        }

        auto transform = entity->getComponent<TransformComponent>();
        auto collider = entity->getComponent<ColliderComponent>();
        auto role = entity->getComponent<RoleComponent>();
        if (!transform || !collider || !role) {
            continue;
        }

        bool blocksPath = role->role == EntityRole::Obstacle || role->role == EntityRole::Base ||
                  role->role == EntityRole::Turret;
        if (!blocksPath) {
            continue;
        }

        int minX = std::max(0, static_cast<int>((transform->position.x - collider->radius) / cellSize));
        int maxX = std::min(gridWidth - 1, static_cast<int>((transform->position.x + collider->radius) / cellSize));
        int minY = std::max(0, static_cast<int>((transform->position.y - collider->radius) / cellSize));
        int maxY = std::min(gridHeight - 1, static_cast<int>((transform->position.y + collider->radius) / cellSize));

        for (int y = minY; y <= maxY; ++y) {
            for (int x = minX; x <= maxX; ++x) {
                float centerX = x * cellSize + cellSize * 0.5f;
                float centerY = y * cellSize + cellSize * 0.5f;
                float dx = centerX - transform->position.x;
                float dy = centerY - transform->position.y;
                if ((dx * dx + dy * dy) <= (collider->radius * collider->radius)) {
                    pathfinder->setObstacle(x, y, true);
                }
            }
        }
    }
}

std::shared_ptr<Entity> Engine::getEntityAtPoint(Vector2 point) const {
    std::shared_ptr<Entity> clicked = nullptr;
    float bestDistance = std::numeric_limits<float>::max();

    for (const auto& [id, entity] : registry->getEntities()) {
        if (!entity || !entity->isActive() || entity->isDestroyed()) {
            continue;
        }

        auto transform = entity->getComponent<TransformComponent>();
        if (!transform) {
            continue;
        }

        float radius = 18.0f;
        auto collider = entity->getComponent<ColliderComponent>();
        auto render = entity->getComponent<RenderComponent>();
        if (collider) {
            radius = collider->radius;
        } else if (render) {
            radius = std::max(render->width, render->height) * 0.5f;
        }

        float d = transform->position.distance(point);
        if (d <= radius && d < bestDistance) {
            clicked = entity;
            bestDistance = d;
        }
    }

    return clicked;
}

void Engine::assignPathToEntity(const std::shared_ptr<Entity>& entity, Vector2 target) {
    if (!entity || !pathfinder) {
        return;
    }

    auto transform = entity->getComponent<TransformComponent>();
    auto movement = entity->getComponent<MovementComponent>();
    auto path = entity->getComponent<PathComponent>();
    if (!transform || !movement || !path) {
        return;
    }

    auto waypoints = pathfinder->findPath(transform->position, target);
    if (waypoints.empty()) {
        waypoints.push_back(target);
    }

    path->waypoints = waypoints;
    path->currentIndex = 0;
    movement->setTarget(path->waypoints.front());
}
