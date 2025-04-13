#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <string>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int GROUND_Y = 500;

const int FRAME_WIDTH = 80;
const int FRAME_HEIGHT = 80;

enum PlayerState {
    IDLE,
    RUN,
    JUMP
};

struct Player {
    int x, y;
    int vx, vy;
    bool onGround = true;
    int frame = 0;
    int frameCounter = 0;
    bool facingRight = true;
    PlayerState state = IDLE;

    std::vector<SDL_Texture*> idleFrames;
    std::vector<SDL_Texture*> runFrames;
    std::vector<SDL_Texture*> jumpFrames;
};

bool initSDL(SDL_Window** window, SDL_Renderer** renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
    if (!IMG_Init(IMG_INIT_PNG)) return false;

    *window = SDL_CreateWindow("Samurai Vengeance",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!*window) return false;

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    return *renderer != nullptr;
}

SDL_Texture* loadTexture(const std::string& path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cerr << "Failed to load " << path << ": " << IMG_GetError() << "\n";
        return nullptr;
    }
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return tex;
}

void loadFrames(std::vector<SDL_Texture*>& frames, const std::string& prefix, int count, SDL_Renderer* renderer) {
    for (int i = 1; i <= count; ++i) {
        std::string path = prefix + std::to_string(i) + ".png";
        SDL_Texture* tex = loadTexture(path, renderer);
        if (tex) frames.push_back(tex);
        else std::cerr << "Could not load frame: " << path << "\n";
    }
}

void loadPlayerFrames(Player& player, SDL_Renderer* renderer) {
    loadFrames(player.idleFrames, "idle", 6, renderer);
    loadFrames(player.runFrames, "run", 8, renderer);
    loadFrames(player.jumpFrames, "jump", 9, renderer);
}

void handleInput(const Uint8* keystate, Player& player) {
    player.vx = 0;

    if (keystate[SDL_SCANCODE_A]) {
        player.vx = -5;
        player.facingRight = false;
    }
    if (keystate[SDL_SCANCODE_D]) {
        player.vx = 5;
        player.facingRight = true;
    }

    if (keystate[SDL_SCANCODE_SPACE] && player.onGround) {
        player.vy = -15;
        player.onGround = false;
    }
}

void updatePlayer(Player& player) {
    player.x += player.vx;
    player.y += player.vy;

    if (!player.onGround) {
        player.vy += 1; // gravity
    }

    if (player.y >= GROUND_Y - FRAME_HEIGHT) {
        player.y = GROUND_Y - FRAME_HEIGHT;
        player.vy = 0;
        player.onGround = true;
    }

    // Update state
    if (!player.onGround) {
        player.state = JUMP;
    } else if (player.vx != 0) {
        player.state = RUN;
    } else {
        player.state = IDLE;
    }

    // Frame animation
    player.frameCounter++;
    int frameDelay = 6;

    if (player.frameCounter >= frameDelay) {
        player.frameCounter = 0;
        if (player.state == IDLE) {
            player.frame = (player.frame + 1) % player.idleFrames.size();
        } else if (player.state == RUN) {
            player.frame = (player.frame + 1) % player.runFrames.size();
        } else if (player.state == JUMP) {
            player.frame = (player.frame + 1) % player.jumpFrames.size();
        }
    }
}

void renderPlayer(SDL_Renderer* renderer, Player& player) {
    SDL_Rect dest = { player.x, player.y, FRAME_WIDTH, FRAME_HEIGHT };

    SDL_Texture* currentFrame = nullptr;
    if (player.state == IDLE) {
        currentFrame = player.idleFrames[player.frame % player.idleFrames.size()];
    } else if (player.state == RUN) {
        currentFrame = player.runFrames[player.frame % player.runFrames.size()];
    } else if (player.state == JUMP) {
        currentFrame = player.jumpFrames[player.frame % player.jumpFrames.size()];
    }

    SDL_RendererFlip flip = player.facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_RenderCopyEx(renderer, currentFrame, nullptr, &dest, 0.0, nullptr, flip);
}

void cleanup(Player& player, SDL_Texture* background) {
    for (auto tex : player.idleFrames) SDL_DestroyTexture(tex);
    for (auto tex : player.runFrames) SDL_DestroyTexture(tex);
    for (auto tex : player.jumpFrames) SDL_DestroyTexture(tex);

    if (background) SDL_DestroyTexture(background);
}

int main(int argc, char* argv[]) {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    if (!initSDL(&window, &renderer)) return -1;

    // Load background
    SDL_Texture* background = loadTexture("background.png", renderer);
    if (!background) {
        std::cerr << "Could not load background\n";
    }

    Player player = {400, GROUND_Y - FRAME_HEIGHT, 0, 0};
    loadPlayerFrames(player, renderer);

    if (player.idleFrames.empty() || player.runFrames.empty() || player.jumpFrames.empty()) {
        std::cerr << "Failed to load all animation frames.\n";
        cleanup(player, background);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;
        }

        const Uint8* keystate = SDL_GetKeyboardState(NULL);
        handleInput(keystate, player);
        updatePlayer(player);
        SDL_RenderClear(renderer);

        // Vẽ background trước
        if (background) {
            SDL_RenderCopy(renderer, background, nullptr, nullptr);
        }

        // Sau đó vẽ player
        renderPlayer(renderer, player);

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }

    cleanup(player, background);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
