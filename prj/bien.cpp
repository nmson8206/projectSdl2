#include "bien.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 750;
const int FRAME_WIDTH = 80;
const int FRAME_HEIGHT = 80;
const int GROUND_Y = SCREEN_HEIGHT - 100;

Mix_Chunk* slash1 = nullptr;
Mix_Chunk* slash2 = nullptr;
Mix_Chunk* skill1 = nullptr;
Mix_Chunk* skill2 = nullptr;
Mix_Chunk* sfx = nullptr;

TTF_Font* font = nullptr;

int score = 0;
int highScore = 0;
int prevScore = -1;
int prevHighScore = -1;

SDL_Texture* scoreTexture = nullptr;
SDL_Texture* highScoreTexture = nullptr;
SDL_Rect scoreRect;
SDL_Rect highScoreRect;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* background = nullptr;
