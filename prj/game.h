#pragma once
#include "bien.h"
#include "player.h"
#include "enemy.h"
#include "game.h"

struct Player;
struct Enemy;
void renderScore(SDL_Renderer* renderer, int score, int highScore);
void restartGame(Player& player, Enemy& enemy);
int loadHighscore(string filename);
void saveHighscore(string filename, int highscore);
