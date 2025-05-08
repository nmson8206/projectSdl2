#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <SDL_mixer.h>

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int FRAME_WIDTH = 80;
const int FRAME_HEIGHT = 80;
const int GROUND_Y = SCREEN_HEIGHT - 1;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* background = nullptr;

enum PlayerState {
    PLAYER_IDLE,
    PLAYER_RUNNING,
    PLAYER_JUMPING,
    PLAYER_ATTACKING,
};

struct Player {
    int x = 100, y = GROUND_Y - FRAME_HEIGHT;
    int vx = 0, vy = 0;
    int frame = 0;
    int frameCounter = 0;
    int attackCombo = 0;
    int attackTimer = 0;
    bool facingRight = true;
    bool onGround = true;
    bool attacking = false;
    int comboStep = 0;
    Uint32 lastAttackTime = 0;
    bool isAttacking = false;
    PlayerState state = PLAYER_IDLE;

    vector<SDL_Texture*> idleFrames;
    vector<SDL_Texture*> runFrames;
    vector<SDL_Texture*> jumpFrames;
    vector<SDL_Texture*> attack1Frames;
    vector<SDL_Texture*> attack2Frames;
    vector<SDL_Texture*> attack3Frames;
};

enum EnemyState {
    ENEMY_FALLING,
    ENEMY_RUNNING,
    ENEMY_DEAD
};

struct Enemy {
    int x, y;
    int vx = 0, vy = 0;
    int speed = 2;
    int frame = 0;
    int frameCounter = 0;
    bool facingRight = true;
    bool onGround = false;
    EnemyState state = ENEMY_FALLING;
    bool isHit = false;
    Uint32 hitTime = 0;

    vector<SDL_Texture*> deadFrames;
    vector<SDL_Texture*> runFrames;
    vector<SDL_Texture*> fallingFrames;
};

SDL_Texture* loadTexture(const string& path) {
    SDL_Texture* tex = IMG_LoadTexture(renderer, path.c_str());
    if (!tex) {
        cout << "Failed to load " << path << ": " << IMG_GetError() << endl;
    }
    return tex;
}

void loadFrames(vector<SDL_Texture*>& frames, const string& baseName, int count) {
    for (int i = 1; i <= count; ++i) {
        string path = baseName + to_string(i) + ".png";
        SDL_Texture* tex = loadTexture(path);
        if (tex) frames.push_back(tex);
    }
}

void loadPlayerFrames(Player& player) {
    loadFrames(player.idleFrames, "idle", 6);
    loadFrames(player.runFrames, "run", 8);
    loadFrames(player.jumpFrames, "jump", 9);
    loadFrames(player.attack1Frames, "attack(1)", 4);
    loadFrames(player.attack2Frames, "attack(2)", 5);
    loadFrames(player.attack3Frames, "attack(3)", 4);
}

void loadEnemyFrames(Enemy& enemy) {
    loadFrames(enemy.fallingFrames, "fall", 6);
    loadFrames(enemy.runFrames, "ewalk(1)", 8);
    loadFrames(enemy.deadFrames, "edead", 6);
}

void handlePlayerInput(Player& player, const Uint8* keystate) {
    if (player.isAttacking) {
        player.vx = 0;
        return;
    }
    player.vx = 0;

    if (keystate[SDL_SCANCODE_A]) {
        player.vx = -4;
        player.facingRight = false;
        if (player.onGround && !player.isAttacking) player.state = PLAYER_RUNNING;
    } else if (keystate[SDL_SCANCODE_D]) {
        player.vx = 4;
        player.facingRight = true;
        if (player.onGround && !player.isAttacking) player.state = PLAYER_RUNNING;
    } else if (player.onGround && !player.isAttacking) {
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
}


void updatePlayer(Player& player, Enemy& enemy) {
    player.x += player.vx;
    player.vy += 1;
    player.y += player.vy;
    player.frameCounter++;
    int delay = 6;
    if (player.y >= GROUND_Y - FRAME_HEIGHT) {
        player.y = GROUND_Y - FRAME_HEIGHT;
        player.vy = 0;
        player.onGround = true;
        if (player.vx == 0 && !player.isAttacking) player.state = PLAYER_IDLE;
        else if (!player.isAttacking) player.state = PLAYER_RUNNING;
    }

    if (player.isAttacking && SDL_GetTicks() - player.lastAttackTime > 600) {
        player.comboStep = 0;
        player.isAttacking = false;
        player.state = PLAYER_IDLE;
        player.frame = 0;
    }
    if (player.state == PLAYER_ATTACKING){
        if (player.comboStep == 2){
            delay = 4;
        }
    }
    if (player.isAttacking){
        SDL_Rect attackRect = {
            player.facingRight ? player.x + FRAME_WIDTH / 2 : player.x - FRAME_WIDTH / 2,
            player.y,
            FRAME_WIDTH,
            FRAME_HEIGHT
        };
        SDL_Rect enemyRect = { enemy.x, enemy.y, FRAME_WIDTH, FRAME_HEIGHT};
        if (SDL_HasIntersection(&attackRect, &enemyRect)) {
            enemy.state = ENEMY_DEAD;
            enemy.frame = 0;
            enemy.frameCounter = 0;
            enemy.vx = 0;
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

    if (currentFrame)
        SDL_RenderCopyEx(renderer, currentFrame, nullptr, &dest, 0.0, nullptr, flip);
}


void updateEnemy(Enemy& enemy, const Player& player) {
    if (enemy.state == ENEMY_DEAD) {
        enemy.frameCounter++;
        if (enemy.frameCounter >= 2) {
            enemy.frameCounter = 0;
            if (!enemy.deadFrames.empty())
                enemy.frame++;
            if (enemy.frame >= enemy.deadFrames.size()){
                enemy.x = rand() % (SCREEN_WIDTH - FRAME_WIDTH);
                enemy.y = -FRAME_HEIGHT;
                enemy.vy = 0;
                enemy.onGround = false;
                enemy.state = ENEMY_FALLING;
                enemy.frame = 0;
            }
        }
        return;
    }
    if (!enemy.onGround) {
        enemy.vy += 1;
        enemy.y += enemy.vy;

        if (enemy.y >= GROUND_Y - FRAME_HEIGHT) {
            enemy.y = GROUND_Y - FRAME_HEIGHT;
            enemy.vy = 0;
            enemy.onGround = true;
            enemy.state = ENEMY_RUNNING;
        } else {
            enemy.state = ENEMY_FALLING;
        }
    }

    if (enemy.state == ENEMY_RUNNING) {
        if (enemy.x < player.x) {
            enemy.vx = enemy.speed;
            enemy.facingRight = true;
        } else if (enemy.x > player.x) {
            enemy.vx = -enemy.speed;
            enemy.facingRight = false;
        } else {
            enemy.vx = 0;
        }
        enemy.x += enemy.vx;
    }

    enemy.frameCounter++;
    int delay = (enemy.state == ENEMY_RUNNING) ? 12 : 15;
    if (enemy.frameCounter >= delay) {
        enemy.frameCounter = 0;
        if (enemy.state == ENEMY_RUNNING && !enemy.runFrames.empty())
            enemy.frame = (enemy.frame + 1) % enemy.runFrames.size();
        else if (enemy.state == ENEMY_FALLING && !enemy.fallingFrames.empty())
            enemy.frame = (enemy.frame + 1) % enemy.fallingFrames.size();
    }
}

void renderEnemy(const Enemy& enemy) {
    SDL_Rect dest = { enemy.x, enemy.y, FRAME_WIDTH, FRAME_HEIGHT };
    SDL_RendererFlip flip = enemy.facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

    SDL_Texture* currentFrame = nullptr;
    if (enemy.state == ENEMY_RUNNING && !enemy.runFrames.empty())
        currentFrame = enemy.runFrames[enemy.frame % enemy.runFrames.size()];
    else if (enemy.state == ENEMY_FALLING && !enemy.fallingFrames.empty())
        currentFrame = enemy.fallingFrames[enemy.frame % enemy.fallingFrames.size()];
    else if (enemy.state == ENEMY_DEAD && !enemy.deadFrames.empty())
        currentFrame = enemy.deadFrames[enemy.frame % enemy.deadFrames.size()];

    if (currentFrame)
        SDL_RenderCopyEx(renderer, currentFrame, nullptr, &dest, 0.0, nullptr, flip);
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << endl;
}

    window = SDL_CreateWindow("Samurai Vengeance", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    background = loadTexture("background.png");
    if (!background) {
        cout << "Could not load background" << endl;
    }
    Mix_Music* bgMusic = Mix_LoadMUS("bg_music.mp3");
    if (!bgMusic) {
    cout << "Failed to load background music: " << Mix_GetError() << endl;
    } else {
    Mix_PlayMusic(bgMusic, -1);
    }

    srand(static_cast<unsigned int>(time(nullptr)));

    Player player;
    loadPlayerFrames(player);

    Enemy enemy = { rand() % (SCREEN_WIDTH - FRAME_WIDTH), -FRAME_HEIGHT };

    loadEnemyFrames(enemy);

    bool running = true;
    SDL_Event e;

    while (running) {
        const Uint8* keystate = SDL_GetKeyboardState(nullptr);
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
        }

        handlePlayerInput(player, keystate);
        updatePlayer(player, enemy);
        updateEnemy(enemy, player);

        SDL_RenderClear(renderer);

        if (background) {
            SDL_RenderCopy(renderer, background, nullptr, nullptr);
        }

        renderPlayer(player);
        renderEnemy(enemy);
        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    for (auto tex : player.idleFrames) SDL_DestroyTexture(tex);
    for (auto tex : player.runFrames) SDL_DestroyTexture(tex);
    for (auto tex : player.jumpFrames) SDL_DestroyTexture(tex);
    for (auto tex : player.attack1Frames) SDL_DestroyTexture(tex);
    for (auto tex : player.attack2Frames) SDL_DestroyTexture(tex);
    for (auto tex : player.attack3Frames) SDL_DestroyTexture(tex);
    for (auto tex : enemy.runFrames) SDL_DestroyTexture(tex);
    for (auto tex : enemy.fallingFrames) SDL_DestroyTexture(tex);
    for (auto tex : enemy.deadFrames) SDL_DestroyTexture(tex);

    if (background) SDL_DestroyTexture(background);

    Mix_FreeMusic(bgMusic);
    Mix_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
