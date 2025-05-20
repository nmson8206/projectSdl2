#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <fstream>

using namespace std;

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern const int FRAME_WIDTH;
extern const int FRAME_HEIGHT;
extern const int GROUND_Y;

extern Mix_Chunk* slash1;
extern Mix_Chunk* slash2;
extern Mix_Chunk* skill1;
extern Mix_Chunk* skill2;
extern Mix_Chunk* sfx;

extern TTF_Font* font;

extern int score;
extern int highScore;
extern int prevScore;
extern int prevHighScore;

extern SDL_Texture* scoreTexture;
extern SDL_Texture* highScoreTexture;
extern SDL_Rect scoreRect;
extern SDL_Rect highScoreRect;

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern SDL_Texture* background;
