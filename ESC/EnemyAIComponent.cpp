#include "EnemyAIComponent.h"
#include "../Game.h"           // for Game::colliders
#include "../Collision.h"      // for Collision::AABB
#include <cstdlib>
#include <ctime>
#include <cmath>

// Helper to generate a random unit vector
static Vector2D randomUnitVector() {
    float angle = (std::rand() / (float)RAND_MAX) * 2.0f * M_PI;
    return { std::cos(angle), std::sin(angle) };
}

EnemyAIComponent::EnemyAIComponent(float range,
                                   float followSpd,
                                   float wanderSpd,
                                   float radius,
                                   Uint32 interval)
  : sightRange(range)
  , followSpeed(followSpd)
  , wanderSpeed(wanderSpd)
  , wanderRadius(radius)
  , lastWanderChange(0)
  , wanderInterval(interval)
  , wanderDirection{0, 0}
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

void EnemyAIComponent::init() {
    auto& tx = entity->getComponent<TransformComponent>();
    spawnCenter = tx.position;
    lastWanderChange = SDL_GetTicks();
}

void EnemyAIComponent::update() {
    auto& tx = entity->getComponent<TransformComponent>();
    Vector2D center{
        tx.position.x + tx.width * tx.scale / 2,
        tx.position.y + tx.height * tx.scale / 2
    };

    // Chase logic
    if (player) {
        auto& ptx = player->getComponent<TransformComponent>();
        Vector2D pCenter{
            ptx.position.x + ptx.width * ptx.scale / 2,
            ptx.position.y + ptx.height * ptx.scale / 2
        };
        float dx = pCenter.x - center.x;
        float dy = pCenter.y - center.y;
        float dist = std::sqrt(dx*dx + dy*dy);

        if (dist > 0.0f && dist <= sightRange) {
            bool blocked = false;
            for (auto& col : Game::colliders) {
                if (col->tag == "wall" &&
                    lineIntersectsRect(center, pCenter, col->collider))
                {
                    blocked = true;
                    break;
                }
            }
            if (!blocked) {
                tx.velocity.x = (dx / dist) * followSpeed;
                tx.velocity.y = (dy / dist) * followSpeed;
                return;
            }
        }
    }

    // Wander logic
    wander(tx);
}

bool EnemyAIComponent::lineIntersectsRect(const Vector2D& p1,
                                           const Vector2D& p2,
                                           const SDL_Rect& rect)
{
    auto intersects = [&](const Vector2D& a, const Vector2D& b,
                          const Vector2D& c, const Vector2D& d) {
        float denom = (b.x - a.x)*(d.y - c.y) - (b.y - a.y)*(d.x - c.x);
        if (denom == 0) return false;
        float ua = ((d.x - c.x)*(a.y - c.y) - (d.y - c.y)*(a.x - c.x)) / denom;
        float ub = ((b.x - a.x)*(a.y - c.y) - (b.y - a.y)*(a.x - c.x)) / denom;
        return ua >= 0 && ua <= 1 && ub >= 0 && ub <= 1;
    };

    Vector2D tl{float(rect.x),          float(rect.y)};
    Vector2D tr{float(rect.x + rect.w), float(rect.y)};
    Vector2D br{float(rect.x + rect.w), float(rect.y + rect.h)};
    Vector2D bl{float(rect.x),          float(rect.y + rect.h)};

    if (intersects(p1, p2, tl, tr)) return true;
    if (intersects(p1, p2, tr, br)) return true;
    if (intersects(p1, p2, br, bl)) return true;
    if (intersects(p1, p2, bl, tl)) return true;
    return false;
}

void EnemyAIComponent::wander(TransformComponent& tx) {
    Uint32 now = SDL_GetTicks();
    if (now - lastWanderChange > wanderInterval) {
        wanderDirection = randomUnitVector();
        lastWanderChange = now;
    }

    auto& cc = entity->getComponent<ColliderComponent>();
    float step = wanderSpeed * tx.speed;

    Vector2D bestDir = wanderDirection;
    SDL_Rect baseRect = cc.collider;
    bool found = false;
    // try up to 8 random directions including original
    for (int i = 0; i < 8; ++i) {
        Vector2D dir = (i == 0 ? wanderDirection : randomUnitVector());
        SDL_Rect nextRect = baseRect;
        nextRect.x += static_cast<int>(dir.x * step);
        nextRect.y += static_cast<int>(dir.y * step);

        bool hit = false;
        for (auto& col : Game::colliders) {
            if (col->tag == "wall" && Collision::AABB(nextRect, col->collider)) {
                hit = true;
                break;
            }
        }
        if (!hit) {
            bestDir = dir;
            found = true;
            break;
        }
    }

    if (!found) {
        // stuck: don't move
        tx.velocity.x = 0;
        tx.velocity.y = 0;
        return;
    }

    wanderDirection = bestDir;
    tx.velocity.x = bestDir.x * wanderSpeed;
    tx.velocity.y = bestDir.y * wanderSpeed;

    // clamp within spawn radius
    Vector2D offset{ tx.position.x - spawnCenter.x,
                     tx.position.y - spawnCenter.y };
    float d = std::sqrt(offset.x*offset.x + offset.y*offset.y);
    if (d > wanderRadius && d > 0.0f) {
        Vector2D back = { -offset.x/d, -offset.y/d };
        wanderDirection = back;
        lastWanderChange = now;
        tx.velocity.x = back.x * wanderSpeed;
        tx.velocity.y = back.y * wanderSpeed;
    }
}
