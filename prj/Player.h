#pragma once
#include "bien.h"
#include "enemy.h"
#include "game.h"
#include "tien_ich.h"

using namespace std;
enum PlayerState {
    PLAYER_IDLE,
    PLAYER_RUNNING,
    PLAYER_JUMPING,
    PLAYER_ATTACKING,
    PLAYER_DASHING,
    PLAYER_DEAD,
    PLAYER_ULTIMATE
};
struct Player {
    int x = 100, y = GROUND_Y - FRAME_HEIGHT;
    int vx = 0, vy = 0;
    int frame = 0;
    int frameCounter = 0;
    int attackCombo = 0;
    int attackTimer = 0;
    bool facingRight = true;
    bool onGround = true;
    bool attacking = false;
    int comboStep = 0;
    Uint32 lastAttackTime = 0;
    bool isAttacking = false;
    PlayerState state = PLAYER_IDLE;
    bool isDead = false;
    bool isDashing = false;
    Uint32 dashStartTime = 0;
    const int dashDuration = 200;
    const int dashSpeed = 10;
    bool canDash = true;
    Uint32 lastDashTime = 0;
    Uint32 dashCoolDown = 1000;
    bool isUltimate = false;
    bool canUltimate = true;
    int ultimateDuration = 500;
    Uint32 ultimateStartTime = 0;
    Uint32 lastUltimatetime = 0;
    Uint32 ultimateCooldown = 5000;
    bool isFlashing = false;
    Uint32 flashStartTime = 0;
    const Uint32 flashDuration = 100;

    vector<SDL_Texture*> idleFrames;
    vector<SDL_Texture*> runFrames;
    vector<SDL_Texture*> jumpFrames;
    vector<SDL_Texture*> attack1Frames;
    vector<SDL_Texture*> attack2Frames;
    vector<SDL_Texture*> attack3Frames;
    vector<SDL_Texture*> dashFrames;
    vector<SDL_Texture*> deadFrames;
    vector<SDL_Texture*> ultimateFrames;
};
void handlePlayerInput(Player& player, const Uint8* keystate);
void loadPlayerFrames(Player& player);
void updatePlayer(Player& player, Enemy& enemy);
void renderPlayer(const Player& player);
