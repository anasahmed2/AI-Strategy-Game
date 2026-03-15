#pragma once

#include "../ECS/System.h"
#include "../ECS/Component.h"
#include <SFML/Graphics.hpp>
#include <memory>

// Forward declarations
class SelectionSystem;
class ResourceSystem;

struct FogCell {
    bool explored = false;
    bool visible  = false;
};

class RenderSystem : public System {
public:
    void update(float deltaTime) override;
    void setRequiredComponents() override;

    void setRenderTarget(sf::RenderWindow* window);
    void setSelectionSystem(std::shared_ptr<SelectionSystem> selection);
    void setResourceSystem(std::shared_ptr<ResourceSystem> resources);
    void render();

    // Legacy shape registry (kept for compatibility)
    void registerShape(const std::string& spriteId, sf::Shape* shape);

private:
    static constexpr int   FOG_COLS  = 38;
    static constexpr int   FOG_ROWS  = 25;
    static constexpr float CELL_SIZE = 32.0f;

    sf::RenderWindow* window = nullptr;
    std::shared_ptr<SelectionSystem> selectionSystem;
    std::shared_ptr<ResourceSystem>  resourceSystem;
    std::unordered_map<std::string, sf::Shape*> shapes;
    sf::Font  font;
    bool      fontLoaded    = false;
    float     lastDeltaTime = 0.016f;

    FogCell fogGrid[FOG_ROWS][FOG_COLS]{};

    void  updateFog();
    bool  isCellVisible(float worldX, float worldY) const;
    void  renderFog();
    void  renderUI();
    void  loadDebugFont();

    sf::Color factionTint(sf::Color base, Faction faction) const;
    float     getFacingAngle(const std::shared_ptr<Entity>& e) const;

    // Per-type drawing helpers
    void drawWorker  (Vector2 pos, float r, sf::Color col, bool selected);
    void drawSoldier (Vector2 pos, float r, sf::Color col, bool selected, float angleDeg);
    void drawTank    (Vector2 pos, float r, sf::Color col, bool selected, float angleDeg);
    void drawScout   (Vector2 pos, float r, sf::Color col, bool selected, float angleDeg);
    void drawBase    (Vector2 pos, float r, sf::Color col, bool selected);
    void drawMine    (Vector2 pos, float r, sf::Color col, bool selected);
    void drawTurret  (Vector2 pos, float r, sf::Color col, bool selected, float angleDeg);
    void drawObstacle(Vector2 pos, float r, sf::Color col);
};
