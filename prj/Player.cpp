// Player.cpp
#include "Player.h"
#include <iostream>

Player::Player(int x, int y, int w, int h, SDL_Renderer* renderer, const std::string& imagePath)
    : x(x), y(y), width(w), height(h), speed(5), renderer(renderer) {
    SDL_Surface* surface = IMG_Load(imagePath.c_str());
    if (!surface) {
        std::cerr << "Không thể load ảnh: " << IMG_GetError() << std::endl;
        texture = nullptr;
    } else {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
}

Player::~Player() {
    if (texture) SDL_DestroyTexture(texture);
}

void Player::handleInput(const Uint8* keystate) {
    if (keystate[SDL_SCANCODE_W]) y -= speed;
    if (keystate[SDL_SCANCODE_S]) y += speed;
    if (keystate[SDL_SCANCODE_A]) x -= speed;
    if (keystate[SDL_SCANCODE_D]) x += speed;

    // Giới hạn trong màn hình
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x + width > 800) x = 800 - width;
    if (y + height > 600) y = 600 - height;
}

void Player::render() {
    SDL_Rect rect = { x, y, width, height };
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}
