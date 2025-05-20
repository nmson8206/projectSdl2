#include "player.h"
#include "enemy.h"
#include "game.h"
#include "tien_ich.h"
#include "bien.h"

void renderScore(SDL_Renderer* renderer, int score, int highScore) {
    static SDL_Color white = {0, 0, 0};
    if (score != prevScore){
        prevScore = score;
        if (scoreTexture){
            SDL_DestroyTexture(scoreTexture);
            scoreTexture = nullptr;
        }
        string scoreText = "Score: " + to_string(score);
        SDL_Surface* surface = TTF_RenderText_Solid(font, scoreText.c_str(), white);
        scoreTexture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!scoreTexture){
            SDL_FreeSurface(surface);
            return;
        }
        scoreRect = { 30, 20, surface->w, surface->h};
        SDL_FreeSurface(surface);
    }
    if (highScore != prevHighScore) {
        prevHighScore = highScore;
        if (highScoreTexture) {
            SDL_DestroyTexture(highScoreTexture);
            highScoreTexture = nullptr;
        }
        string highScoreText = "High Score: " + std::to_string(highScore);
        SDL_Surface* surface = TTF_RenderText_Solid(font, highScoreText.c_str(), white);
        highScoreTexture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!highScoreTexture) {
            SDL_FreeSurface(surface);
            return;
        }
        highScoreRect = {30, 50, surface->w, surface->h};
        SDL_FreeSurface(surface);
    }

    if (scoreTexture) {
        SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect);
    if (highScoreTexture) {
        SDL_RenderCopy(renderer, highScoreTexture, nullptr, &highScoreRect);
    }
    }
}
void restartGame(Player& player, Enemy& enemy){
    score = 0;
    prevScore = -1;
    player.x = 100;
    player.y = GROUND_Y - FRAME_HEIGHT;
    player.vx = 0;
    player.vy = 0;
    player.state = PLAYER_IDLE;
    player.isDead = false;
    player.isAttacking = false;
    player.comboStep = 0;
    player.frame = 0;
    player.frameCounter = 0;
    enemy.x = rand() % (SCREEN_WIDTH - FRAME_WIDTH);
    enemy.y = -FRAME_HEIGHT;
    enemy.vx = 0;
    enemy.vy = 0;
    enemy.speed = 2;
    enemy.frame = 0;
    enemy.frameCounter = 0;
    enemy.isHit = false;
    enemy.state = ENEMY_FALLING;
    enemy.onGround = false;
}
int loadHighscore(string filename){
    ifstream file(filename);
    int highscore = 0;
    if (file.is_open()){
        file >> highscore;
        file.close();
    }
    return highscore;
}
void saveHighscore(string filename, int highscore){
    ofstream file(filename);
    if (file.is_open()){
        file << highscore;
        file.close();
    }
}
