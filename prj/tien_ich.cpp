#include "player.h"
#include "enemy.h"
#include "game.h"
#include "tien_ich.h"
#include "bien.h"

SDL_Texture* loadTexture(const string& path) {
    SDL_Texture* tex = IMG_LoadTexture(renderer, path.c_str());
    return tex;
}
void loadFrames(vector<SDL_Texture*>& frames, const string& baseName, int count) {
    for (int i = 1; i <= count; ++i) {
        string path = baseName + to_string(i) + ".png";
        SDL_Texture* tex = loadTexture(path);
        if (tex) frames.push_back(tex);
    }
}
