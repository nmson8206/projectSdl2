#include "player.h"
#include "enemy.h"
#include "game.h"
#include "tien_ich.h"
#include "bien.h"

void loadEnemyFrames(Enemy& enemy) {
    loadFrames(enemy.fallingFrames, "fall", 6);
    loadFrames(enemy.runFrames, "ewalk(1)", 8);
    loadFrames(enemy.deadFrames, "edead", 6);
    loadFrames(enemy.attackFrames, "eattack", 3);
}
void updateEnemy(Enemy& enemy,  Player& player) {
    if (player.isDead && enemy.frame == 3) return;
    if (enemy.state == ENEMY_DEAD) {
        enemy.frameCounter++;
        if (enemy.frameCounter >= 6) {
            enemy.frameCounter = 0;
            if (!enemy.deadFrames.empty())
                enemy.frame++;
            if (enemy.frame >= enemy.deadFrames.size()){
                enemy.x = rand() % (SCREEN_WIDTH - FRAME_WIDTH);
                enemy.y = -FRAME_HEIGHT;
                enemy.vy = 0;
                enemy.onGround = false;
                enemy.state = ENEMY_FALLING;
                enemy.frame = 0;
            }
        }
        return;
    }
    if (!enemy.onGround) {
        enemy.vy += 1;
        enemy.y += enemy.vy;

        if (enemy.y >= GROUND_Y - FRAME_HEIGHT) {
            enemy.y = GROUND_Y - FRAME_HEIGHT;
            enemy.vy = 0;
            enemy.onGround = true;
            enemy.state = ENEMY_RUNNING;
        } else {
            enemy.state = ENEMY_FALLING;
        }
    }

    if (enemy.state == ENEMY_RUNNING) {
        int distanceX = abs(enemy.x - player.x);
        if (distanceX < 60 && SDL_GetTicks() - enemy.lastAttackTime > enemy.attackCooldown){
            enemy.state = ENEMY_ATTACKING;
            enemy.frame = 0;
            enemy.frameCounter = 0;
            enemy.isAttacking = true;
            enemy.lastAttackTime = SDL_GetTicks();
            return;

        }
        if (enemy.x < player.x) {
            enemy.vx = enemy.speed;
            enemy.facingRight = true;
        } else if (enemy.x > player.x) {
            enemy.vx = -enemy.speed;
            enemy.facingRight = false;
        } else {
            enemy.vx = 0;
        }
        enemy.x += enemy.vx;
    }
    if (enemy.state == ENEMY_ATTACKING) {
        if (!player.isDead){
            SDL_Rect enemyAttackRect = {
            enemy.facingRight ? enemy.x + FRAME_WIDTH/2 : enemy.x - FRAME_WIDTH/2,
            enemy.y,
            FRAME_WIDTH,
            FRAME_HEIGHT
        };
        SDL_Rect playerRect = { player.x, player.y, FRAME_WIDTH, FRAME_HEIGHT };
        if (SDL_HasIntersection(&enemyAttackRect, &playerRect)) {
            Mix_PlayChannel(2, slash2, 0);
            if (!player.isDead){
                player.state = PLAYER_DEAD;
                player.isDead = true;
                player.vx = 0;
                player.vy = 0;
                player.frame = 0;
                player.frameCounter = 0;
    }
    }
        if(!player.isDead) Mix_PlayChannel(3, slash1 ,0);
    }
        enemy.frameCounter++;
        if (enemy.frameCounter >= 6) {
            enemy.frameCounter = 0;
            if (!enemy.attackFrames.empty())
            enemy.frame++;

            if (enemy.frame >= enemy.attackFrames.size()) {
                enemy.frame = 0;
                enemy.state = ENEMY_RUNNING;
                enemy.isAttacking = false;
        }
    }
    return;
}

    enemy.frameCounter++;
    int delay = (enemy.state == ENEMY_RUNNING) ? 12 : 15;
    if (enemy.frameCounter >= delay) {
        enemy.frameCounter = 0;
        if (enemy.state == ENEMY_RUNNING && !enemy.runFrames.empty())
            enemy.frame = (enemy.frame + 1) % enemy.runFrames.size();
        else if (enemy.state == ENEMY_FALLING && !enemy.fallingFrames.empty())
            enemy.frame = (enemy.frame + 1) % enemy.fallingFrames.size();
    }
}
void renderEnemy(const Enemy& enemy) {
    SDL_Rect dest = { enemy.x, enemy.y, FRAME_WIDTH, FRAME_HEIGHT };
    SDL_RendererFlip flip = enemy.facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

    SDL_Texture* currentFrame = nullptr;
    if (enemy.state == ENEMY_RUNNING && !enemy.runFrames.empty())
        currentFrame = enemy.runFrames[enemy.frame % enemy.runFrames.size()];
    else if (enemy.state == ENEMY_FALLING && !enemy.fallingFrames.empty())
        currentFrame = enemy.fallingFrames[enemy.frame % enemy.fallingFrames.size()];
    else if (enemy.state == ENEMY_DEAD && !enemy.deadFrames.empty())
        currentFrame = enemy.deadFrames[enemy.frame % enemy.deadFrames.size()];
    else if (enemy.state == ENEMY_ATTACKING && !enemy.attackFrames.empty())
        currentFrame = enemy.attackFrames[enemy.frame % enemy.attackFrames.size()];

    if (currentFrame)
        SDL_RenderCopyEx(renderer, currentFrame, nullptr, &dest, 0.0, nullptr, flip);
}
