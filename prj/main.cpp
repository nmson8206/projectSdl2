#include "bien.h"
#include "player.h"
#include "enemy.h"
#include "tien_ich.h"
#include "game.h"

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    font = TTF_OpenFont("Minecraft.ttf", 32);
    Uint32 now = SDL_GetTicks();
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << endl;
}

    window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    background = loadTexture("background.png");
    if (!background) {
        cout << "Could not load background" << endl;
    }
    Mix_Music* bgMusic = Mix_LoadMUS("bg_music.mp3");
    slash1 = Mix_LoadWAV("slash1.wav");
    slash2 = Mix_LoadWAV("slash2.wav");
    skill2 = Mix_LoadWAV("skill2.wav");
    skill1 = Mix_LoadWAV("skill1.wav");
    sfx = Mix_LoadWAV("sfx.wav");
    if (!slash1) cout << "Failed to load attack.wav: " << Mix_GetError() << endl;
    if (!slash2) cout << "Failed to load hit.wav: " << Mix_GetError() << endl;
    if (!bgMusic) {
        cout << "Failed to load background music: " << Mix_GetError() << endl;
    }  else {
    Mix_PlayMusic(bgMusic, -1);
    }

    srand(static_cast<unsigned int>(time(nullptr)));

    Player player;
    loadPlayerFrames(player);
    Enemy enemy1 = { rand() % (SCREEN_WIDTH - FRAME_WIDTH), -FRAME_HEIGHT };
    Enemy enemy2 = { rand() % (SCREEN_WIDTH - FRAME_WIDTH), -FRAME_HEIGHT };

    loadEnemyFrames(enemy1);
    loadEnemyFrames(enemy2);


    bool running = true;
    SDL_Event e;
    SDL_Texture* skill1Texture = IMG_LoadTexture(renderer, "skill1.png");
    SDL_Texture* skill2Texture = IMG_LoadTexture(renderer, "skill2.png");
    SDL_Texture* HUD = IMG_LoadTexture(renderer, "hud.png");
    SDL_Texture* restart = IMG_LoadTexture(renderer, "restart.png");
    SDL_Rect restartButton = {SCREEN_WIDTH / 2 - 170 , SCREEN_HEIGHT / 2 - 150 , 360, 360};
    int highScore = loadHighscore("highscore.txt");

    while (running) {
        const Uint8* keystate = SDL_GetKeyboardState(nullptr);
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                int x = e.button.x;
                int y = e.button.y;

            if (x >= restartButton.x && x <= restartButton.x + restartButton.w &&
                y >= restartButton.y && y <= restartButton.y + restartButton.h){
                restartGame (player, enemy1);
                restartGame (player, enemy2);
                }
        }
        }
        Uint32 now = SDL_GetTicks();

        handlePlayerInput(player, keystate);
        updatePlayer(player, enemy1);
        updatePlayer(player, enemy2);
        updateEnemy(enemy1, player);
        updateEnemy(enemy2, player);


        SDL_RenderClear(renderer);

        if (background) {
            SDL_RenderCopy(renderer, background, nullptr, nullptr);
        }
        renderPlayer(player);
        renderEnemy(enemy1);
        renderEnemy(enemy2);

        if (player.isDead && player.frame >= 5){
            SDL_RenderCopy(renderer, restart, NULL, &restartButton);
        }
        if (player.isFlashing) {
            Uint32 elapsed = SDL_GetTicks() - player.flashStartTime;
            if (elapsed < player.flashDuration){
                Uint8 alpha = 255 - (Uint8)((elapsed)/(float)player.flashDuration)*255;
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, alpha);
                SDL_Rect screenRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
                SDL_RenderFillRect(renderer, &screenRect);
            }
            else player.isFlashing = false;
        }
        SDL_Rect hudRect = {0, SCREEN_HEIGHT-100, SCREEN_WIDTH, 100};
        SDL_RenderCopy(renderer, HUD, NULL, &hudRect);
        SDL_Rect skill1Rect = {20, SCREEN_HEIGHT - 80, 80, 80};
        Uint32 elapsed1 = now - player.lastDashTime;
        SDL_RenderCopy(renderer, skill1Texture, NULL, &skill1Rect);
        if (elapsed1 < player.dashCoolDown) {
            float ratio = 1.0f - (float)elapsed1 / player.dashCoolDown;
            SDL_Rect overlay = skill1Rect;
            overlay.h = (int)(skill1Rect.h * ratio);
            overlay.y = skill1Rect.y + skill1Rect.h - overlay.h;
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
            SDL_RenderFillRect(renderer, &overlay);
}
        SDL_Rect skill2Rect = {200, SCREEN_HEIGHT - 80, 80, 80};
        SDL_RenderCopy(renderer, skill2Texture, NULL, &skill2Rect);
        Uint32 elapsed2 = now - player.lastUltimatetime;
        if (elapsed2 < player.ultimateCooldown) {
            float ratio = 1.0f - (float)elapsed2 / player.ultimateCooldown;
            SDL_Rect overlay = skill2Rect;
            overlay.h = (int)(skill2Rect.h * ratio);
            overlay.y = skill2Rect.y + skill2Rect.h - overlay.h;
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
            SDL_RenderFillRect(renderer, &overlay);
}
        renderScore(renderer, score, highScore);
        if (score > highScore) {
            highScore = score;
            saveHighscore("highscore.txt", highScore);
    }
        SDL_RenderPresent(renderer);
        SDL_Delay(20);

    }

    for (auto tex : player.idleFrames) SDL_DestroyTexture(tex);
    for (auto tex : player.runFrames) SDL_DestroyTexture(tex);
    for (auto tex : player.jumpFrames) SDL_DestroyTexture(tex);
    for (auto tex : player.attack1Frames) SDL_DestroyTexture(tex);
    for (auto tex : player.attack2Frames) SDL_DestroyTexture(tex);
    for (auto tex : player.attack3Frames) SDL_DestroyTexture(tex);
    for (auto tex : enemy1.runFrames) SDL_DestroyTexture(tex);
    for (auto tex : enemy1.fallingFrames) SDL_DestroyTexture(tex);
    for (auto tex : enemy1.deadFrames) SDL_DestroyTexture(tex);
    for (auto tex : enemy2.runFrames) SDL_DestroyTexture(tex);
    for (auto tex : enemy2.fallingFrames) SDL_DestroyTexture(tex);
    for (auto tex : enemy2.deadFrames) SDL_DestroyTexture(tex);
    for (auto tex : player.dashFrames) SDL_DestroyTexture(tex);
    for (auto tex : player.deadFrames) SDL_DestroyTexture(tex);
    for (auto tex : player.ultimateFrames) SDL_DestroyTexture(tex);

    if (background) SDL_DestroyTexture(background);

    Mix_FreeMusic(bgMusic);
    Mix_FreeChunk(slash1);
    Mix_FreeChunk(slash2);
    Mix_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(skill1Texture);
    SDL_DestroyTexture(skill2Texture);
    TTF_CloseFont(font);
    SDL_DestroyTexture(scoreTexture);
    TTF_Quit;
    IMG_Quit();
    SDL_Quit();

    return 0;
}
