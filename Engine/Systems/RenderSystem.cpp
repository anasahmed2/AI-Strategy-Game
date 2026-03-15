#include "RenderSystem.h"
#include "SelectionSystem.h"
#include "ResourceSystem.h"
#include "../AI/AISystem.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ─── Primitive shape generators ──────────────────────────────────────────────
static sf::ConvexShape makeDiamond(float r) {
    sf::ConvexShape s(4);
    s.setPoint(0, { 0.0f,     -r     });
    s.setPoint(1, { r*0.7f,   0.0f   });
    s.setPoint(2, { 0.0f,     r      });
    s.setPoint(3, {-r*0.7f,   0.0f   });
    return s;
}

static sf::ConvexShape makeTriangle(float r) {
    sf::ConvexShape s(3);
    s.setPoint(0, { 0.0f,    -r       });
    s.setPoint(1, { r*0.8f,   r*0.7f  });
    s.setPoint(2, {-r*0.8f,   r*0.7f  });
    return s;
}

static sf::ConvexShape makeArrow(float r) {
    sf::ConvexShape s(5);
    s.setPoint(0, { 0.0f,      -r         });
    s.setPoint(1, { r*0.45f,  -r*0.15f   });
    s.setPoint(2, { r*0.25f,   r          });
    s.setPoint(3, {-r*0.25f,   r          });
    s.setPoint(4, {-r*0.45f,  -r*0.15f   });
    return s;
}

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

void RenderSystem::setSelectionSystem(std::shared_ptr<SelectionSystem> s) {
    selectionSystem = s;
}

void RenderSystem::setResourceSystem(std::shared_ptr<ResourceSystem> r) {
    resourceSystem = r;
}

// ─── Fog of War ──────────────────────────────────────────────────────────────
void RenderSystem::updateFog() {
    for (int row = 0; row < FOG_ROWS; ++row)
        for (int col = 0; col < FOG_COLS; ++col)
            fogGrid[row][col].visible = false;

    auto revealCircle = [&](Vector2 pos, float visionPx) {
        int cr  = static_cast<int>(pos.y / CELL_SIZE);
        int cc  = static_cast<int>(pos.x / CELL_SIZE);
        int rad = static_cast<int>(std::ceil(visionPx / CELL_SIZE));
        for (int dr = -rad; dr <= rad; ++dr) {
            for (int dc = -rad; dc <= rad; ++dc) {
                int nr = cr + dr, nc = cc + dc;
                if (nr < 0 || nr >= FOG_ROWS || nc < 0 || nc >= FOG_COLS) continue;
                if (std::sqrt(float(dr*dr + dc*dc)) * CELL_SIZE <= visionPx) {
                    fogGrid[nr][nc].visible  = true;
                    fogGrid[nr][nc].explored = true;
                }
            }
        }
    };

    for (auto& entity : entities) {
        auto team = entity->getComponent<TeamComponent>();
        if (!team || team->faction != Faction::Player) continue;
        if (!entity->isActive() || entity->isDestroyed()) continue;
        auto transform = entity->getComponent<TransformComponent>();
        auto role      = entity->getComponent<RoleComponent>();
        if (!transform) continue;
        float vision = 140.0f;
        if (role) {
            switch (role->role) {
                case EntityRole::Scout:  vision = 210.0f; break;
                case EntityRole::Tank:   vision = 120.0f; break;
                case EntityRole::Base:   vision = 200.0f; break;
                case EntityRole::Turret: vision = 180.0f; break;
                case EntityRole::Worker: vision = 110.0f; break;
                default: break;
            }
        }
        revealCircle(transform->position, vision);
    }
}

bool RenderSystem::isCellVisible(float worldX, float worldY) const {
    int c = static_cast<int>(worldX / CELL_SIZE);
    int r = static_cast<int>(worldY / CELL_SIZE);
    if (r < 0 || r >= FOG_ROWS || c < 0 || c >= FOG_COLS) return true;
    return fogGrid[r][c].visible;
}

void RenderSystem::renderFog() {
    for (int r = 0; r < FOG_ROWS; ++r) {
        for (int c = 0; c < FOG_COLS; ++c) {
            const auto& cell = fogGrid[r][c];
            sf::Uint8 alpha;
            if      (!cell.explored)                   alpha = 215;
            else if (!cell.visible && cell.explored)   alpha = 105;
            else                                       continue;
            sf::RectangleShape tile(sf::Vector2f(CELL_SIZE, CELL_SIZE));
            tile.setPosition(static_cast<float>(c) * CELL_SIZE,
                             static_cast<float>(r) * CELL_SIZE);
            tile.setFillColor(sf::Color(0, 0, 0, alpha));
            window->draw(tile);
        }
    }
}

// ─── Per-type drawing helpers ─────────────────────────────────────────────────
sf::Color RenderSystem::factionTint(sf::Color base, Faction faction) const {
    if (faction == Faction::Enemy)
        return sf::Color(static_cast<sf::Uint8>(std::min(255, base.r + 55)),
                         static_cast<sf::Uint8>(base.g * 0.45f),
                         static_cast<sf::Uint8>(base.b * 0.45f));
    return base;
}

float RenderSystem::getFacingAngle(const std::shared_ptr<Entity>& e) const {
    auto transform = e->getComponent<TransformComponent>();
    auto path      = e->getComponent<PathComponent>();
    if (path && path->hasPath() && transform) {
        Vector2 t  = path->waypoints[path->currentIndex];
        float dx   = t.x - transform->position.x;
        float dy   = t.y - transform->position.y;
        if (std::sqrt(dx*dx + dy*dy) > 3.0f)
            return std::atan2(dy, dx) * 180.0f / static_cast<float>(M_PI);
    }
    auto phys = e->getComponent<PhysicsComponent>();
    if (phys) {
        float vx = phys->velocity.x, vy = phys->velocity.y;
        if (std::sqrt(vx*vx + vy*vy) > 1.0f)
            return std::atan2(vy, vx) * 180.0f / static_cast<float>(M_PI);
    }
    return -90.0f;
}

void RenderSystem::drawWorker(Vector2 pos, float r, sf::Color col, bool sel) {
    auto body = makeDiamond(r);
    body.setPosition(pos.x, pos.y);
    body.setFillColor(col);
    body.setOutlineColor(sel ? sf::Color::White : sf::Color(0, 0, 0, 180));
    body.setOutlineThickness(sel ? 3.0f : 1.5f);
    window->draw(body);
    // Inner pickaxe X icon
    sf::RectangleShape t1(sf::Vector2f(r*1.1f, r*0.22f));
    sf::RectangleShape t2(sf::Vector2f(r*1.1f, r*0.22f));
    t1.setOrigin(r*0.55f, r*0.11f); t1.setPosition(pos.x, pos.y); t1.setRotation( 45);
    t2.setOrigin(r*0.55f, r*0.11f); t2.setPosition(pos.x, pos.y); t2.setRotation(-45);
    t1.setFillColor(sf::Color(0, 0, 0, 140));
    t2.setFillColor(sf::Color(0, 0, 0, 140));
    window->draw(t1); window->draw(t2);
}

void RenderSystem::drawSoldier(Vector2 pos, float r, sf::Color col, bool sel, float ang) {
    auto tri = makeTriangle(r);
    tri.setPosition(pos.x, pos.y);
    tri.setRotation(ang + 90.0f);
    tri.setFillColor(col);
    tri.setOutlineColor(sel ? sf::Color::White : sf::Color(0, 0, 0, 180));
    tri.setOutlineThickness(sel ? 3.0f : 1.5f);
    window->draw(tri);
    // Gun barrel
    sf::RectangleShape barrel(sf::Vector2f(r*1.2f, r*0.25f));
    barrel.setOrigin(0, r*0.125f); barrel.setPosition(pos.x, pos.y); barrel.setRotation(ang);
    barrel.setFillColor(sf::Color(0, 0, 0, 160));
    window->draw(barrel);
}

void RenderSystem::drawTank(Vector2 pos, float r, sf::Color col, bool sel, float ang) {
    sf::CircleShape hull(r, 6);
    hull.setOrigin(r, r); hull.setPosition(pos.x, pos.y); hull.setRotation(ang + 90.0f);
    hull.setFillColor(col);
    hull.setOutlineColor(sel ? sf::Color::White : sf::Color(0, 0, 0, 200));
    hull.setOutlineThickness(sel ? 3.0f : 2.0f);
    window->draw(hull);
    sf::RectangleShape barrel(sf::Vector2f(r*1.5f, r*0.32f));
    barrel.setOrigin(0, r*0.16f); barrel.setPosition(pos.x, pos.y); barrel.setRotation(ang);
    barrel.setFillColor(sf::Color(col.r/2, col.g/2, col.b/2));
    window->draw(barrel);
}

void RenderSystem::drawScout(Vector2 pos, float r, sf::Color col, bool sel, float ang) {
    auto arrow = makeArrow(r);
    arrow.setPosition(pos.x, pos.y); arrow.setRotation(ang + 90.0f);
    arrow.setFillColor(col);
    arrow.setOutlineColor(sel ? sf::Color::White : sf::Color(0, 0, 0, 140));
    arrow.setOutlineThickness(sel ? 3.0f : 1.2f);
    window->draw(arrow);
}

void RenderSystem::drawBase(Vector2 pos, float r, sf::Color col, bool sel) {
    // Main keep
    sf::RectangleShape keep(sf::Vector2f(r*2.0f, r*2.0f));
    keep.setOrigin(r, r); keep.setPosition(pos.x, pos.y);
    keep.setFillColor(col);
    keep.setOutlineColor(sel ? sf::Color::White : sf::Color(0, 0, 0, 180));
    keep.setOutlineThickness(sel ? 4.0f : 2.0f);
    window->draw(keep);
    // Corner towers
    float tw = r * 0.44f, offset = r + tw * 0.45f;
    for (int ix = -1; ix <= 1; ix += 2) {
        for (int iy = -1; iy <= 1; iy += 2) {
            sf::RectangleShape tower(sf::Vector2f(tw, tw));
            tower.setOrigin(tw*0.5f, tw*0.5f);
            tower.setPosition(pos.x + ix*offset, pos.y + iy*offset);
            tower.setFillColor(sf::Color(col.r>20?col.r-20:0, col.g>20?col.g-20:0, col.b>20?col.b-20:0));
            tower.setOutlineColor(sf::Color(0, 0, 0, 160));
            tower.setOutlineThickness(1.0f);
            window->draw(tower);
        }
    }
    // Flag
    sf::RectangleShape pole(sf::Vector2f(2.0f, r*0.85f));
    pole.setPosition(pos.x - 1.0f, pos.y - r - r*0.85f);
    pole.setFillColor(sf::Color(180, 180, 180));
    window->draw(pole);
    sf::RectangleShape flag(sf::Vector2f(r*0.7f, r*0.38f));
    flag.setPosition(pos.x + 1.0f, pos.y - r - r*0.85f);
    flag.setFillColor(col.b > 150 ? sf::Color::Yellow : sf::Color::Red);
    window->draw(flag);
}

void RenderSystem::drawMine(Vector2 pos, float r, sf::Color col, bool sel) {
    sf::ConvexShape gem(6);
    gem.setPoint(0, { 0.0f,    -r       });
    gem.setPoint(1, { r*0.7f, -r*0.3f  });
    gem.setPoint(2, { r*0.6f,  r*0.6f  });
    gem.setPoint(3, { 0.0f,    r        });
    gem.setPoint(4, {-r*0.6f,  r*0.6f  });
    gem.setPoint(5, {-r*0.7f, -r*0.3f  });
    gem.setPosition(pos.x, pos.y);
    gem.setFillColor(col);
    gem.setOutlineColor(sel ? sf::Color::White : sf::Color(200, 170, 0, 200));
    gem.setOutlineThickness(sel ? 3.0f : 2.0f);
    window->draw(gem);
    sf::RectangleShape shine(sf::Vector2f(2.5f, r*0.7f));
    shine.setPosition(pos.x - r*0.18f, pos.y - r*0.55f);
    shine.setFillColor(sf::Color(255, 255, 200, 180));
    window->draw(shine);
}

void RenderSystem::drawTurret(Vector2 pos, float r, sf::Color col, bool sel, float ang) {
    sf::CircleShape pad(r*0.92f);
    pad.setOrigin(r*0.92f, r*0.92f); pad.setPosition(pos.x, pos.y);
    pad.setFillColor(sf::Color(60, 60, 60));
    window->draw(pad);
    sf::RectangleShape mount(sf::Vector2f(r*1.1f, r*1.1f));
    mount.setOrigin(r*0.55f, r*0.55f); mount.setPosition(pos.x, pos.y);
    mount.setFillColor(col);
    mount.setOutlineColor(sel ? sf::Color::White : sf::Color(0, 0, 0, 200));
    mount.setOutlineThickness(sel ? 3.0f : 1.5f);
    window->draw(mount);
    sf::RectangleShape barrel(sf::Vector2f(r*1.6f, r*0.35f));
    barrel.setOrigin(0, r*0.175f); barrel.setPosition(pos.x, pos.y); barrel.setRotation(ang);
    barrel.setFillColor(sf::Color(30, 30, 30));
    window->draw(barrel);
}

void RenderSystem::drawObstacle(Vector2 pos, float r, sf::Color col) {
    float tw = r*0.38f, th = r*0.85f;
    sf::RectangleShape trunk(sf::Vector2f(tw, th));
    trunk.setOrigin(tw*0.5f, th); trunk.setPosition(pos.x, pos.y + r*0.35f);
    trunk.setFillColor(sf::Color(80, 50, 25));
    window->draw(trunk);
    sf::ConvexShape canopy(3);
    canopy.setPoint(0, { 0.0f,  -r       });
    canopy.setPoint(1, { r,      r*0.45f });
    canopy.setPoint(2, {-r,      r*0.45f });
    canopy.setPosition(pos.x, pos.y - r*0.2f);
    canopy.setFillColor(col);
    canopy.setOutlineColor(sf::Color(20, 60, 10));
    canopy.setOutlineThickness(1.5f);
    window->draw(canopy);
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

    // Update fog visibility each frame
    updateFog();

    // Sort entities by layer
    auto sortedEntities = entities;
    std::sort(sortedEntities.begin(), sortedEntities.end(),
        [](const std::shared_ptr<Entity>& a, const std::shared_ptr<Entity>& b) {
            auto renderA = a->getComponent<RenderComponent>();
            auto renderB = b->getComponent<RenderComponent>();
            if (!renderA || !renderB) return false;
            return renderA->layer < renderB->layer;
        });

    // ── Pass 1: Entity bodies (enemies hidden in fog) ─────────────────────────
    for (auto& entity : sortedEntities) {
        auto transform = entity->getComponent<TransformComponent>();
        auto render    = entity->getComponent<RenderComponent>();
        if (!transform || !render || !render->visible) continue;

        float px = transform->position.x, py = transform->position.y;
        float r  = render->width * 0.5f;

        auto team      = entity->getComponent<TeamComponent>();
        auto selection = entity->getComponent<SelectionComponent>();
        if (team && team->faction == Faction::Enemy && !isCellVisible(px, py)) continue;

        bool sel = selection && selection->isSelected;

        sf::Color baseCol = sf::Color(200, 200, 200);
        if      (render->spriteId == "worker")        baseCol = sf::Color(230, 200,  50);
        else if (render->spriteId == "soldier")       baseCol = sf::Color( 70, 180,  70);
        else if (render->spriteId == "tank")          baseCol = sf::Color(160, 160, 160);
        else if (render->spriteId == "scout")         baseCol = sf::Color( 80, 210, 240);
        else if (render->spriteId == "base")          baseCol = sf::Color( 60, 100, 200);
        else if (render->spriteId == "resource_mine") baseCol = sf::Color(220, 160,   0);
        else if (render->spriteId == "turret")        baseCol = sf::Color(180,  50,  50);
        else if (render->spriteId == "tree")          baseCol = sf::Color( 40, 120,  30);

        sf::Color col    = factionTint(baseCol, team ? team->faction : Faction::Neutral);
        float     facing = getFacingAngle(entity);

        if      (render->spriteId == "worker")        drawWorker  ({px, py}, r, col, sel);
        else if (render->spriteId == "soldier")       drawSoldier ({px, py}, r, col, sel, facing);
        else if (render->spriteId == "tank")          drawTank    ({px, py}, r, col, sel, facing);
        else if (render->spriteId == "scout")         drawScout   ({px, py}, r, col, sel, facing);
        else if (render->spriteId == "base")          drawBase    ({px, py}, r, col, sel);
        else if (render->spriteId == "resource_mine") drawMine    ({px, py}, r, col, sel);
        else if (render->spriteId == "turret")        drawTurret  ({px, py}, r, col, sel, facing);
        else if (render->spriteId == "tree")          drawObstacle({px, py}, r, col);
        else {
            sf::CircleShape c(r);
            c.setOrigin(r, r); c.setPosition(px, py); c.setFillColor(col);
            if (sel) { c.setOutlineColor(sf::Color::White); c.setOutlineThickness(3.0f); }
            window->draw(c);
        }
    }

    // ── Fog overlay (drawn on top of entities) ────────────────────────────────
    renderFog();

    // ── Pass 2: Post-fog overlays (HP bars, paths, command markers) ───────────
    for (auto& entity : sortedEntities) {
        auto transform = entity->getComponent<TransformComponent>();
        auto render    = entity->getComponent<RenderComponent>();
        if (!transform || !render || !render->visible) continue;

        float px = transform->position.x, py = transform->position.y;
        float r  = render->width * 0.5f;

        auto team      = entity->getComponent<TeamComponent>();
        auto health    = entity->getComponent<HealthComponent>();
        auto selection = entity->getComponent<SelectionComponent>();
        auto path      = entity->getComponent<PathComponent>();
        auto command   = entity->getComponent<CommandComponent>();
        if (team && team->faction == Faction::Enemy && !isCellVisible(px, py)) continue;

        if (health) {
            float ratio = health->maxHealth > 0.0f ? health->currentHealth / health->maxHealth : 0.0f;
            ratio = std::max(0.0f, std::min(1.0f, ratio));
            float bw = r * 2.0f;
            sf::RectangleShape hpBg(sf::Vector2f(bw, 4.0f));
            hpBg.setOrigin(bw*0.5f, 2.0f); hpBg.setPosition(px, py - r - 6.0f);
            hpBg.setFillColor(sf::Color(30, 30, 30, 200));
            window->draw(hpBg);
            sf::RectangleShape hpFg(sf::Vector2f(bw*ratio, 4.0f));
            hpFg.setOrigin(bw*0.5f, 2.0f); hpFg.setPosition(px, py - r - 6.0f);
            hpFg.setFillColor(ratio > 0.6f ? sf::Color(60, 200, 60) :
                              ratio > 0.3f ? sf::Color(220, 200, 0) : sf::Color(200, 50, 50));
            window->draw(hpFg);
        }

        if (selection && selection->isSelected && team && team->faction == Faction::Player) {
            if (path && path->hasPath()) {
                std::vector<sf::Vertex> pts;
                pts.push_back({ sf::Vector2f(px, py), sf::Color(180, 220, 255, 180) });
                for (auto& wp : path->waypoints)
                    pts.push_back({ sf::Vector2f(wp.x, wp.y), sf::Color(180, 220, 255, 180) });
                window->draw(pts.data(), pts.size(), sf::LineStrip);
                for (auto& wp : path->waypoints) {
                    sf::CircleShape node(3.0f);
                    node.setOrigin(3.0f, 3.0f); node.setPosition(wp.x, wp.y);
                    node.setFillColor(sf::Color(200, 235, 255));
                    window->draw(node);
                }
            }
            if (command && command->type != CommandType::None) {
                Vector2 m = command->targetPosition;
                sf::Color mc = sf::Color::White;
                if (command->type == CommandType::Attack)       mc = sf::Color(255,  80,  80);
                if (command->type == CommandType::Gather)       mc = sf::Color(255, 220,  80);
                if (command->type == CommandType::Defend)       mc = sf::Color(100, 210, 255);
                if (command->type == CommandType::ReturnToBase) mc = sf::Color(120, 255, 120);
                sf::RectangleShape l1(sf::Vector2f(20, 3)), l2(sf::Vector2f(20, 3));
                l1.setPosition(m.x-10, m.y); l1.setRotation( 45); l1.setFillColor(mc);
                l2.setPosition(m.x-10, m.y); l2.setRotation(-45); l2.setFillColor(mc);
                window->draw(l1); window->draw(l2);
            }
        }
    }

    // ── UI / HUD ──────────────────────────────────────────────────────────────
    renderUI();
}

void RenderSystem::renderUI() {
    if (!window || !selectionSystem) return;

    float winW = static_cast<float>(window->getSize().x);
    float winH = static_cast<float>(window->getSize().y);

    // ── Gold HUD (top-right) ──────────────────────────────────────────────────
    if (fontLoaded && resourceSystem) {
        float gold = resourceSystem->getResource(Faction::Player, "Gold");
        std::ostringstream gs; gs << "Gold: " << static_cast<int>(gold);
        sf::RectangleShape bg(sf::Vector2f(155.0f, 30.0f));
        bg.setPosition(winW - 162.0f, 6.0f);
        bg.setFillColor(sf::Color(0, 0, 0, 170));
        bg.setOutlineColor(sf::Color(255, 220, 80, 140));
        bg.setOutlineThickness(1.5f);
        window->draw(bg);
        sf::Text goldText; goldText.setFont(font); goldText.setCharacterSize(18);
        goldText.setFillColor(sf::Color(255, 220, 80));
        goldText.setString(gs.str()); goldText.setPosition(winW - 157.0f, 9.0f);
        window->draw(goldText);
    }

    // ── Multi-select count (top-left) ─────────────────────────────────────────
    const auto& selList = selectionSystem->getSelectedEntities();
    if (fontLoaded && selList.size() > 1) {
        std::ostringstream ms; ms << selList.size() << " units selected";
        sf::Text mt; mt.setFont(font); mt.setCharacterSize(14);
        mt.setFillColor(sf::Color::White); mt.setString(ms.str());
        mt.setPosition(10.0f, 10.0f); window->draw(mt);
    }

    // ── Selected unit info panel (bottom-left) ────────────────────────────────
    auto selected = selectionSystem->getSelectedEntity();
    if (!selected) return;

    auto render    = selected->getComponent<RenderComponent>();
    auto health    = selected->getComponent<HealthComponent>();
    auto command   = selected->getComponent<CommandComponent>();
    auto path      = selected->getComponent<PathComponent>();
    auto ai        = selected->getComponent<AIComponent>();
    auto role      = selected->getComponent<RoleComponent>();
    auto team      = selected->getComponent<TeamComponent>();
    auto collector = selected->getComponent<ResourceCollectorComponent>();

    bool isPlayerBase = role && role->role == EntityRole::Base &&
                        team && team->faction == Faction::Player;
    float panelH = isPlayerBase ? 175.0f : 145.0f;

    sf::RectangleShape panel(sf::Vector2f(315.0f, panelH));
    panel.setPosition(8.0f, winH - panelH - 8.0f);
    panel.setFillColor(sf::Color(0, 0, 0, 190));
    panel.setOutlineColor(sf::Color(140, 140, 170, 220));
    panel.setOutlineThickness(2.0f);
    window->draw(panel);

    // Colored icon
    sf::RectangleShape icon(sf::Vector2f(34.0f, 34.0f));
    icon.setPosition(16.0f, winH - panelH - 8.0f + 8.0f);
    sf::Color iconCol = sf::Color(200, 200, 200);
    if (render) {
        if      (render->spriteId == "worker")        iconCol = sf::Color(230, 200,  50);
        else if (render->spriteId == "soldier")       iconCol = sf::Color( 70, 180,  70);
        else if (render->spriteId == "tank")          iconCol = sf::Color(160, 160, 160);
        else if (render->spriteId == "scout")         iconCol = sf::Color( 80, 210, 240);
        else if (render->spriteId == "base")          iconCol = sf::Color( 60, 100, 200);
        else if (render->spriteId == "resource_mine") iconCol = sf::Color(220, 160,   0);
        else if (render->spriteId == "turret")        iconCol = sf::Color(180,  50,  50);
        if (team && team->faction == Faction::Enemy)
            iconCol = sf::Color(std::min(255, iconCol.r+50), iconCol.g/2, iconCol.b/2);
    }
    icon.setFillColor(iconCol); icon.setOutlineColor(sf::Color::White);
    icon.setOutlineThickness(1.5f); window->draw(icon);

    if (!fontLoaded) return;

    float tx = 60.0f, ty = winH - panelH - 8.0f + 10.0f, lineH = 19.0f;

    auto makeLine = [&](const std::string& txt, float yOff,
                        sf::Color col = sf::Color::White, unsigned int size = 14) {
        sf::Text t; t.setFont(font); t.setString(txt);
        t.setCharacterSize(size); t.setFillColor(col);
        t.setPosition(tx, ty + yOff); window->draw(t);
    };

    // Type name
    std::string typeName = render ? render->spriteId : "Unknown";
    if (!typeName.empty()) typeName[0] = static_cast<char>(std::toupper(typeName[0]));
    for (char& ch : typeName) if (ch == '_') ch = ' ';
    makeLine(typeName, 0.0f, sf::Color(220, 220, 100), 15);

    // HP
    if (health) {
        float ratio = health->currentHealth / health->maxHealth;
        sf::Color hcol = ratio > 0.6f ? sf::Color(100, 220, 100) :
                         ratio > 0.3f ? sf::Color(220, 200,  60) : sf::Color(220, 80, 80);
        std::ostringstream hs;
        hs << "HP: " << static_cast<int>(health->currentHealth)
           << " / " << static_cast<int>(health->maxHealth);
        makeLine(hs.str(), lineH*1, hcol);
    }

    // Status
    std::string cmdTxt = "Idle";
    if (command) {
        switch (command->type) {
            case CommandType::Move:         cmdTxt = "Moving";    break;
            case CommandType::Attack:       cmdTxt = "Attacking"; break;
            case CommandType::Gather:       cmdTxt = "Gathering"; break;
            case CommandType::Defend:       cmdTxt = "Defending"; break;
            case CommandType::ReturnToBase: cmdTxt = "Returning"; break;
            default: break;
        }
    }
    makeLine("Status: " + cmdTxt, lineH*2);

    // AI state
    if (ai && ai->stateMachine) {
        AIState state = ai->stateMachine->getCurrentState();
        std::string st = "Idle";
        if      (state == AIState::Move)   st = "Moving";
        else if (state == AIState::Attack) st = "Attack";
        else if (state == AIState::Flee)   st = "Fleeing";
        else if (state == AIState::Gather) st = "Gather";
        makeLine("AI: " + st, lineH*3, sf::Color(180, 180, 230));
    }

    // Worker carry amount
    if (collector) {
        std::ostringstream cs;
        cs << "Carries: " << static_cast<int>(collector->carryAmount)
           << "/" << static_cast<int>(collector->carryCapacity) << " gold";
        makeLine(cs.str(), lineH*4, sf::Color(220, 200, 80));
    }

    // Dev: path nodes + FPS
    float fps = lastDeltaTime > 0.0001f ? 1.0f / lastDeltaTime : 0.0f;
    std::ostringstream dev;
    dev << "Path: " << (path ? static_cast<int>(path->waypoints.size()) : 0)
        << "  FPS: " << static_cast<int>(fps);
    makeLine(dev.str(), lineH*5, sf::Color(140, 140, 140), 12);

    // Production hints when base is selected
    if (isPlayerBase) {
        makeLine("[1]Worker 75g  [2]Soldier 120g", lineH*6 + 4.0f, sf::Color(180, 230, 180), 13);
        makeLine("[3]Tank 220g   [4]Scout 140g",   lineH*7 + 4.0f, sf::Color(180, 230, 180), 13);
    }
}

void RenderSystem::loadDebugFont() {
    if (fontLoaded) return;
    const std::vector<std::string> candidates = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationMono-Regular.ttf",
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
        "/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf",
        "/usr/share/fonts/opentype/noto/NotoSans-Regular.ttf",
        "/usr/share/fonts/truetype/msttcorefonts/Arial.ttf",
        "C:/Windows/Fonts/consola.ttf",
        "C:/Windows/Fonts/arial.ttf"
    };
    for (const auto& p : candidates)
        if (font.loadFromFile(p)) { fontLoaded = true; return; }
}
