
// Player.h
#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <SDL_image.h>
#include <string>

class Player {
public:
    Player(int x, int y, int w, int h, SDL_Renderer* renderer, const std::string& imagePath);
    ~Player();

    void handleInput(const Uint8* keystate);
    void render();

private:
    int x, y;
    int width, height;
    int speed;
    SDL_Texture* texture;
    SDL_Renderer* renderer;
};

#endif
