#pragma once
#include "bien.h"
#include "player.h"
#include "enemy.h"
#include "game.h"

SDL_Texture* loadTexture(const string& path);
void loadFrames(vector<SDL_Texture*>& frames, const string& baseName, int count);
