#include "player.h"
#include "enemy.h"
#include "game.h"
#include "tien_ich.h"
#include "bien.h"

void loadPlayerFrames(Player& player) {
    loadFrames(player.idleFrames, "idle", 6);
    loadFrames(player.runFrames, "run", 8);
    loadFrames(player.jumpFrames, "jump", 9);
    loadFrames(player.attack1Frames, "attack(1)", 4);
    loadFrames(player.attack2Frames, "attack(2)", 5);
    loadFrames(player.attack3Frames, "attack(3)", 4);
    loadFrames(player.dashFrames, "dash", 8);
    loadFrames(player.deadFrames, "die", 6);
    loadFrames(player.ultimateFrames, "ultimate", 10);
}
void handlePlayerInput(Player& player, const Uint8* keystate) {
    if (player.isDead) return;
    Uint32 currentTime = SDL_GetTicks();
    if (player.isAttacking) {
        player.vx = 0;
        return;
    }
    player.vx = 0;

    if (keystate[SDL_SCANCODE_A]) {
        player.vx = -4;
        player.facingRight = false;
        if (player.onGround && !player.isAttacking && !player.isDashing && !player.isUltimate) player.state = PLAYER_RUNNING;
    } else if (keystate[SDL_SCANCODE_D]) {
        player.vx = 4;
        player.facingRight = true;
        if (player.onGround && !player.isAttacking && !player.isDashing && !player.isUltimate) player.state = PLAYER_RUNNING;
    } else if (player.onGround && !player.isAttacking && !player.isDashing && !player.isUltimate) {
        player.state = PLAYER_IDLE;
    }

    if (keystate[SDL_SCANCODE_SPACE] && player.onGround) {
        player.vy = -20;
        player.onGround = false;
        player.state = PLAYER_JUMPING;
        player.frame = 0;
        player.frameCounter = 0;
    }

    if (keystate[SDL_SCANCODE_J] && !player.isAttacking) {
        player.isAttacking = true;
        player.comboStep = (player.comboStep % 3) + 1;
        player.frame = 0;
        player.frameCounter = 0;
        player.state = PLAYER_ATTACKING;
        player.lastAttackTime = SDL_GetTicks();
    }
    if (keystate[SDL_SCANCODE_L] && !player.isDashing && player.onGround && !player.isAttacking && player.canDash) {
        player.isDashing = true;
        player.canDash = false;
        player.lastDashTime = currentTime;
        player.dashStartTime = SDL_GetTicks();
        player.state = PLAYER_DASHING;
        player.frame = 0;
        player.frameCounter = 0;
        Mix_PlayChannel(1, skill1 ,0);
    }
    if(keystate[SDL_SCANCODE_I] && player.onGround && !player.isAttacking && player.canUltimate){
        player.isUltimate = true;
        player.canUltimate = false;
        player.lastUltimatetime = currentTime;
        player.ultimateStartTime = SDL_GetTicks();
        player.state = PLAYER_ULTIMATE;
        player.frame = 0;
        player.frameCounter = 0;
        Mix_PlayChannel(3, skill2 ,0);
        player.isFlashing = true;
        player.flashStartTime = SDL_GetTicks();
        Mix_PlayChannel(4, sfx ,0);

    }

}
void updatePlayer(Player& player, Enemy& enemy) {
    Uint32 currentTime = SDL_GetTicks();
     if (player.state == PLAYER_DEAD) {
        player.frameCounter++;
        if (player.frameCounter >= 10){
            player.frameCounter = 0;
            if (player.frame < player.deadFrames.size()-1) player.frame++;
        }
        return;
    }
     if (!player.canUltimate && currentTime - player.lastUltimatetime >= player.ultimateCooldown){
        player.canUltimate = true;
    }

    if (player.isUltimate) {
        player.state = PLAYER_ULTIMATE;
        if (enemy.state != ENEMY_DEAD) {
            enemy.state = ENEMY_DEAD;
            enemy.frame = 0;
            enemy.frameCounter = 0;
            enemy.vx = 0;
            score += 100;
    }

        if (SDL_GetTicks() - player.ultimateStartTime >= player.ultimateDuration) {
            player.isUltimate = false;
            player.state = PLAYER_IDLE;
            player.frame = 0;
    }
}
    if (!player.canDash && currentTime - player.lastDashTime >= player.dashCoolDown){
        player.canDash = true;
    }
    if (player.isDashing) {
        int direction = player.facingRight ? 1 : -1;
        player.vx = player.dashSpeed * direction;

        if (SDL_GetTicks() - player.dashStartTime >= player.dashDuration) {
            player.isDashing = false;
            player.vx = 0;
            player.state = PLAYER_IDLE;
            player.frame = 0;
        }
    }
    if (!player.canUltimate && currentTime - player.lastUltimatetime >= player.ultimateCooldown){
        player.canUltimate = true;
    }

    player.x += player.vx;
    player.vy += 1;
    player.y += player.vy;
    player.frameCounter++;


    int delay = 10;
    if (player.state == PLAYER_DASHING) delay = 2;

    if (player.y >= GROUND_Y - FRAME_HEIGHT) {
        player.y = GROUND_Y - FRAME_HEIGHT;
        player.vy = 0;
        player.onGround = true;
        if (player.state != PLAYER_ULTIMATE){
            if (player.vx == 0 && !player.isAttacking && !player.isDashing) player.state = PLAYER_IDLE;
            else if (!player.isAttacking && !player.isDashing) player.state = PLAYER_RUNNING;
    }
    }

    if (player.isAttacking && SDL_GetTicks() - player.lastAttackTime > 600) {
        player.comboStep = 0;
        player.isAttacking = false;
        player.state = PLAYER_IDLE;
        player.frame = 0;
    }

    if (player.state == PLAYER_ATTACKING){
        if (player.comboStep == 2){
            delay = 8;
        }
    }

    if (player.isAttacking){
        SDL_Rect attackRect = {
            player.facingRight ? player.x + FRAME_WIDTH / 2 : player.x - FRAME_WIDTH / 2,
            player.y,
            FRAME_WIDTH-20,
            FRAME_HEIGHT-20
        };
        SDL_Rect enemyRect = { enemy.x, enemy.y, FRAME_WIDTH, FRAME_HEIGHT};
        if (SDL_HasIntersection(&attackRect, &enemyRect)) {
            Mix_PlayChannel(1, slash2, 0);
            if (enemy.state != ENEMY_DEAD){
                enemy.state = ENEMY_DEAD;
                enemy.frame = 0;
                enemy.frameCounter = 0;
                enemy.vx = 0;
                score += 100;
            }
        }
        else if (player.frame == 1 ){
            Mix_PlayChannel(0, slash1 ,0);
        }
    }

    if (player.frameCounter >= delay) {
        player.frameCounter = 0;

        if (player.state == PLAYER_ATTACKING) {
            int frameLimit = 0;
            if (player.comboStep == 1) frameLimit = player.attack1Frames.size();
            else if (player.comboStep == 2) frameLimit = player.attack2Frames.size();
            else if (player.comboStep == 3) frameLimit = player.attack3Frames.size();

            player.frame++;
            if (player.frame >= frameLimit) {
                player.isAttacking = false;
                player.frame = 0;
                player.state = PLAYER_IDLE;
                player.lastAttackTime = SDL_GetTicks();
            }
        } else {
            if (player.state == PLAYER_IDLE && !player.idleFrames.empty())
                player.frame = (player.frame + 1) % player.idleFrames.size();
            else if (player.state == PLAYER_RUNNING && !player.runFrames.empty())
                player.frame = (player.frame + 1) % player.runFrames.size();
            else if (player.state == PLAYER_JUMPING && !player.jumpFrames.empty())
                player.frame = (player.frame + 1) % player.jumpFrames.size();
            else if (player.state == PLAYER_DASHING && !player.dashFrames.empty())
                player.frame = (player.frame + 1) % player.dashFrames.size();
            else if (player.state == PLAYER_ULTIMATE && !player.ultimateFrames.empty())
                player.frame = (player.frame + 1) % player.ultimateFrames.size();
        }
    }
}
void renderPlayer(const Player& player) {
    SDL_Texture* currentFrame = nullptr;
    SDL_RendererFlip flip = player.facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_Rect dest = { player.x, player.y, FRAME_WIDTH, FRAME_HEIGHT };

    if (player.state == PLAYER_IDLE && !player.idleFrames.empty())
        currentFrame = player.idleFrames[player.frame % player.idleFrames.size()];
    else if (player.state == PLAYER_RUNNING && !player.runFrames.empty())
        currentFrame = player.runFrames[player.frame % player.runFrames.size()];
    else if (player.state == PLAYER_JUMPING && !player.jumpFrames.empty())
        currentFrame = player.jumpFrames[player.frame % player.jumpFrames.size()];
    else if (player.state == PLAYER_ATTACKING) {
        if (player.comboStep == 1 && !player.attack1Frames.empty()) {
            currentFrame = player.attack1Frames[player.frame % player.attack1Frames.size()];
            dest.w = 112; dest.h = 80;
        } else if (player.comboStep == 2 && !player.attack2Frames.empty()) {
            currentFrame = player.attack2Frames[player.frame % player.attack2Frames.size()];
            dest.w = 112;
            dest.h = 112;
            dest.y = player.y - (112 - FRAME_HEIGHT);
        } else if (player.comboStep == 3 && !player.attack3Frames.empty()) {
            currentFrame = player.attack3Frames[player.frame % player.attack3Frames.size()];
            dest.w = 112; dest.h = 80;
        }
    }
    else if (player.state == PLAYER_DASHING && !player.dashFrames.empty()) {
        currentFrame = player.dashFrames[player.frame % player.dashFrames.size()];
    }
    else if (player.state == PLAYER_ULTIMATE && !player.ultimateFrames.empty()) {
        currentFrame = player.ultimateFrames[player.frame % player.ultimateFrames.size()];
    }
    else if (player.state == PLAYER_DEAD && !player.deadFrames.empty()) {
        currentFrame = player.deadFrames[player.frame % player.deadFrames.size()];
    }
    if (currentFrame)
        SDL_RenderCopyEx(renderer, currentFrame, nullptr, &dest, 0.0, nullptr, flip);
}
