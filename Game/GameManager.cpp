#include "GameManager.h"
#include <iostream>
#include <algorithm>
#include "Units/Worker.h"
#include "Units/Soldier.h"
#include "Units/Tank.h"
#include "Units/Scout.h"
#include "Buildings/Base.h"
#include "Buildings/ResourceMine.h"
#include "Buildings/Turret.h"

GameManager::GameManager() {
    // Create engine (800x600 window)
    engine = std::make_shared<Engine>(1200, 800, "AI Strategy Game");
}

GameManager::~GameManager() {
    shutdown();
}

void GameManager::initialize() {
    setupLevel();
    setupAI();
}

void GameManager::shutdown() {
    // Clean up handled by smart pointers
}

void GameManager::update(float deltaTime) {
    engine->pollEvents();
    handlePlayerActions(deltaTime);
    engine->update(deltaTime);
}

void GameManager::render() {
    engine->render();
}

bool GameManager::isRunning() {
    return engine->isRunning();
}

std::shared_ptr<Entity> GameManager::spawnWorker(Vector2 position, Faction faction, bool isAIControlled) {
    auto worker = std::make_shared<Worker>();
    return worker->create(engine, position, faction, isAIControlled);
}

std::shared_ptr<Entity> GameManager::spawnSoldier(Vector2 position, Faction faction, bool isAIControlled) {
    auto soldier = std::make_shared<Soldier>();
    return soldier->create(engine, position, faction, isAIControlled);
}

std::shared_ptr<Entity> GameManager::spawnTank(Vector2 position, Faction faction, bool isAIControlled) {
    auto tank = std::make_shared<Tank>();
    return tank->create(engine, position, faction, isAIControlled);
}

std::shared_ptr<Entity> GameManager::spawnScout(Vector2 position, Faction faction, bool isAIControlled) {
    auto scout = std::make_shared<Scout>();
    return scout->create(engine, position, faction, isAIControlled);
}

std::shared_ptr<Entity> GameManager::spawnBase(Vector2 position, Faction faction, bool isAIControlled) {
    auto base = std::make_shared<Base>();
    return base->create(engine, position, faction, isAIControlled);
}

std::shared_ptr<Entity> GameManager::spawnResourceMine(Vector2 position) {
    auto mine = std::make_shared<ResourceMine>();
    return mine->create(engine, position, Faction::Neutral, false);
}

std::shared_ptr<Entity> GameManager::spawnTurret(Vector2 position, Faction faction, bool isAIControlled) {
    auto turret = std::make_shared<Turret>();
    return turret->create(engine, position, faction, isAIControlled);
}

std::shared_ptr<Entity> GameManager::spawnObstacle(Vector2 position, Vector2 size) {
    auto obstacle = engine->getRegistry()->createEntity();

    auto transform = std::make_shared<TransformComponent>(position);
    obstacle->addComponent(transform);

    auto render = std::make_shared<RenderComponent>("tree");
    render->width = static_cast<int>(size.x);
    render->height = static_cast<int>(size.y);
    render->layer = 1;
    obstacle->addComponent(render);

    auto collider = std::make_shared<ColliderComponent>(size.x * 0.45f);
    obstacle->addComponent(collider);

    auto selection = std::make_shared<SelectionComponent>(false);
    obstacle->addComponent(selection);

    auto role = std::make_shared<RoleComponent>(EntityRole::Obstacle);
    obstacle->addComponent(role);

    auto team = std::make_shared<TeamComponent>(Faction::Neutral, false);
    obstacle->addComponent(team);

    engine->getRegistry()->notifySystems(obstacle);
    return obstacle;
}

void GameManager::setupLevel() {
    // Spawn player base
    spawnBase(Vector2(130.0f, 120.0f), Faction::Player, false);
    
    // Spawn enemy base
    spawnBase(Vector2(1060.0f, 680.0f), Faction::Enemy, true);
    
    // Spawn resources
    spawnResourceMine(Vector2(400.0f, 400.0f));
    spawnResourceMine(Vector2(800.0f, 300.0f));
    spawnResourceMine(Vector2(620.0f, 560.0f));

    // Spawn map obstacles to force pathing choices.
    spawnObstacle(Vector2(520.0f, 240.0f), Vector2(56.0f, 56.0f));
    spawnObstacle(Vector2(560.0f, 280.0f), Vector2(56.0f, 56.0f));
    spawnObstacle(Vector2(600.0f, 320.0f), Vector2(56.0f, 56.0f));
    spawnObstacle(Vector2(640.0f, 360.0f), Vector2(56.0f, 56.0f));
    spawnObstacle(Vector2(700.0f, 500.0f), Vector2(56.0f, 56.0f));
    spawnObstacle(Vector2(740.0f, 540.0f), Vector2(56.0f, 56.0f));
    
    // Spawn initial units
    spawnWorker(Vector2(200.0f, 150.0f), Faction::Player, false);
    spawnWorker(Vector2(250.0f, 150.0f), Faction::Player, false);
    spawnSoldier(Vector2(290.0f, 180.0f), Faction::Player, false);
    spawnScout(Vector2(240.0f, 220.0f), Faction::Player, false);

    spawnWorker(Vector2(980.0f, 640.0f), Faction::Enemy, true);
    spawnSoldier(Vector2(940.0f, 620.0f), Faction::Enemy, true);
    spawnSoldier(Vector2(900.0f, 680.0f), Faction::Enemy, true);
    spawnTank(Vector2(1020.0f, 610.0f), Faction::Enemy, true);
    spawnScout(Vector2(960.0f, 560.0f), Faction::Enemy, true);

    spawnTurret(Vector2(980.0f, 720.0f), Faction::Enemy, true);
    spawnTurret(Vector2(270.0f, 90.0f), Faction::Player, false);

    // Initial faction resource banks.
    engine->getResourceSystem()->addResource(Faction::Player, "Gold", 450.0f);
    engine->getResourceSystem()->addResource(Faction::Enemy, "Gold", 450.0f);
}

void GameManager::setupAI() {
    // AI setup will be done per-unit
}

void GameManager::handlePlayerActions(float deltaTime) {
    actionCooldown = std::max(0.0f, actionCooldown - deltaTime);
    enemyProductionTimer += deltaTime;

    handleProduction();
    handleBuildingPlacement();

    if (enemyProductionTimer >= 12.0f) {
        enemyProductionTimer = 0.0f;

        std::shared_ptr<Entity> enemyBase = nullptr;
        for (const auto& [id, entity] : engine->getRegistry()->getEntities()) {
            if (!entity || !entity->isActive() || entity->isDestroyed()) {
                continue;
            }
            auto role = entity->getComponent<RoleComponent>();
            auto team = entity->getComponent<TeamComponent>();
            if (role && team && role->role == EntityRole::Base && team->faction == Faction::Enemy) {
                enemyBase = entity;
                break;
            }
        }

        if (enemyBase) {
            auto transform = enemyBase->getComponent<TransformComponent>();
            if (transform) {
                Vector2 spawnA(transform->position.x - 70.0f, transform->position.y - 40.0f);
                Vector2 spawnB(transform->position.x - 95.0f, transform->position.y + 20.0f);
                auto resourceSystem = engine->getResourceSystem();

                if (resourceSystem->spendResource(Faction::Enemy, "Gold", 120.0f)) {
                    spawnSoldier(spawnA, Faction::Enemy, true);
                }
                if (resourceSystem->spendResource(Faction::Enemy, "Gold", 140.0f)) {
                    spawnScout(spawnB, Faction::Enemy, true);
                }
            }
        }
    }
}

void GameManager::handleProduction() {
    if (actionCooldown > 0.0f) {
        return;
    }

    auto selected = engine->getSelectionSystem()->getSelectedEntity();
    if (!selected) {
        return;
    }

    auto role = selected->getComponent<RoleComponent>();
    auto team = selected->getComponent<TeamComponent>();
    auto transform = selected->getComponent<TransformComponent>();
    if (!role || !team || !transform || role->role != EntityRole::Base || team->faction != Faction::Player) {
        return;
    }

    auto resourceSystem = engine->getResourceSystem();
    Vector2 spawnPoint(transform->position.x + 80.0f, transform->position.y + 60.0f);

    if (engine->getInputSystem()->isKeyPressed(sf::Keyboard::Num1)) {
        if (resourceSystem->spendResource(Faction::Player, "Gold", 75.0f)) {
            spawnWorker(spawnPoint, Faction::Player, false);
            actionCooldown = 0.18f;
        }
    } else if (engine->getInputSystem()->isKeyPressed(sf::Keyboard::Num2)) {
        if (resourceSystem->spendResource(Faction::Player, "Gold", 120.0f)) {
            spawnSoldier(spawnPoint, Faction::Player, false);
            actionCooldown = 0.18f;
        }
    } else if (engine->getInputSystem()->isKeyPressed(sf::Keyboard::Num3)) {
        if (resourceSystem->spendResource(Faction::Player, "Gold", 220.0f)) {
            spawnTank(spawnPoint, Faction::Player, false);
            actionCooldown = 0.20f;
        }
    } else if (engine->getInputSystem()->isKeyPressed(sf::Keyboard::Num4)) {
        if (resourceSystem->spendResource(Faction::Player, "Gold", 140.0f)) {
            spawnScout(spawnPoint, Faction::Player, false);
            actionCooldown = 0.18f;
        }
    }
}

void GameManager::handleBuildingPlacement() {
    if (actionCooldown > 0.0f) {
        return;
    }

    if (engine->getInputSystem()->isKeyPressed(sf::Keyboard::B)) {
        buildMode = BuildMode::Base;
    } else if (engine->getInputSystem()->isKeyPressed(sf::Keyboard::T)) {
        buildMode = BuildMode::Turret;
    }

    if (buildMode == BuildMode::None) {
        return;
    }

    if (!engine->getInputSystem()->wasMouseButtonClicked(0)) {
        return;
    }

    Vector2 placePos = engine->getInputSystem()->getMousePosition();

    auto isBlocked = [&]() {
        for (const auto& [id, entity] : engine->getRegistry()->getEntities()) {
            if (!entity || !entity->isActive() || entity->isDestroyed()) {
                continue;
            }

            auto transform = entity->getComponent<TransformComponent>();
            auto collider = entity->getComponent<ColliderComponent>();
            if (!transform || !collider) {
                continue;
            }

            if (transform->position.distance(placePos) < collider->radius + 48.0f) {
                return true;
            }
        }
        return false;
    };

    if (isBlocked()) {
        buildMode = BuildMode::None;
        actionCooldown = 0.12f;
        return;
    }

    auto resourceSystem = engine->getResourceSystem();
    if (buildMode == BuildMode::Turret) {
        if (resourceSystem->spendResource(Faction::Player, "Gold", 180.0f)) {
            spawnTurret(placePos, Faction::Player, false);
        }
    } else if (buildMode == BuildMode::Base) {
        if (resourceSystem->spendResource(Faction::Player, "Gold", 320.0f)) {
            spawnBase(placePos, Faction::Player, false);
        }
    }

    buildMode = BuildMode::None;
    actionCooldown = 0.18f;
}
