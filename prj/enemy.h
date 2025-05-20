#pragma once
#include "bien.h"
#include "game.h"
#include "tien_ich.h"

struct Player;
enum EnemyState {
    ENEMY_FALLING,
    ENEMY_RUNNING,
    ENEMY_DEAD,
    ENEMY_ATTACKING
};
struct Enemy {
    int x, y;
    int vx = 0, vy = 0;
    int speed = 2;
    int frame = 0;
    int frameCounter = 0;
    bool facingRight = true;
    bool onGround = false;
    EnemyState state = ENEMY_FALLING;
    bool isHit = false;
    Uint32 hitTime = 0;
    bool isAttacking = false;
    Uint32 lastAttackTime = 0;
    const Uint32 attackCooldown = 1500;

    vector<SDL_Texture*> deadFrames;
    vector<SDL_Texture*> runFrames;
    vector<SDL_Texture*> fallingFrames;
    vector<SDL_Texture*> attackFrames;
};
void loadEnemyFrames(Enemy& enemy);
void updateEnemy(Enemy& enemy,  Player& player);
void renderEnemy(const Enemy& enemy);
