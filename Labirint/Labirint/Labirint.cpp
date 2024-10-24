#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>

using namespace std;

const int CELL_SIZE = 64; // Размер клетки лабиринта
const int SCREEN_WIDTH = 640; // Ширина экрана
const int SCREEN_HEIGHT = 640; // Высота экрана
const int SPEED = 1; // Скорость персонажа

// Определение лабиринта (1 - стена, 0 - проход, 2 - выход)
int maze[10][10] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 1, 0, 1},
    {1, 0, 1, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 1, 1, 1, 0, 1, 0, 1, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {1, 1, 1, 0, 1, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 1, 1, 1, 0, 1, 0, 2}, // 2 - выход
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

// Функция для проверки столкновения с стеной
bool checkCollision(SDL_Rect playerRect) {
    int left = playerRect.x;
    int right = playerRect.x + playerRect.w - 1;
    int top = playerRect.y;
    int bottom = playerRect.y + playerRect.h - 1;

    int gridX1 = left / CELL_SIZE;
    int gridY1 = top / CELL_SIZE;

    int gridX2 = right / CELL_SIZE;
    int gridY2 = top / CELL_SIZE;

    int gridX3 = left / CELL_SIZE;
    int gridY3 = bottom / CELL_SIZE;

    int gridX4 = right / CELL_SIZE;
    int gridY4 = bottom / CELL_SIZE;

    if (gridX1 < 0 || gridX1 >= 10 || gridY1 < 0 || gridY1 >= 10 ||
        gridX2 < 0 || gridX2 >= 10 || gridY2 < 0 || gridY2 >= 10 ||
        gridX3 < 0 || gridX3 >= 10 || gridY3 < 0 || gridY3 >= 10 ||
        gridX4 < 0 || gridX4 >= 10 || gridY4 < 0 || gridY4 >= 10) {
        return true;
    }

    if (maze[gridY1][gridX1] == 1 || maze[gridY2][gridX2] == 1 ||
        maze[gridY3][gridX3] == 1 || maze[gridY4][gridX4] == 1) {
        return true;
    }

    return false;
}

// Функция для проверки достижения выхода
bool reachedExit(SDL_Rect playerRect) {
    int right = playerRect.x + playerRect.w - 1;
    int bottom = playerRect.y + playerRect.h - 1;

    int gridX = right / CELL_SIZE;
    int gridY = bottom / CELL_SIZE;

    return maze[gridY][gridX] == 2;
}

// Функция для отрисовки лабиринта
void drawMaze(SDL_Renderer* renderer) {
    SDL_Rect cellRect;
    cellRect.w = CELL_SIZE;
    cellRect.h = CELL_SIZE;

    for (int row = 0; row < 10; ++row) {
        for (int col = 0; col < 10; ++col) {
            cellRect.x = col * CELL_SIZE;
            cellRect.y = row * CELL_SIZE;

            if (maze[row][col] == 1) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Синие стены
                SDL_RenderFillRect(renderer, &cellRect);
            }
            else if (maze[row][col] == 2) {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Зеленый выход
                SDL_RenderFillRect(renderer, &cellRect);
            }
            else {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Белый проход
                SDL_RenderFillRect(renderer, &cellRect);
            }
        }
    }
}

// Функция для отрисовки текста
void displayText(SDL_Renderer* renderer, const char* message, TTF_Font* font, SDL_Color textColor, SDL_Rect textRect) {
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, message, textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture(textTexture);
}

// Функция для очистки экрана
void clearScreen(SDL_Renderer* renderer, SDL_Color backgroundColor) {
    SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    SDL_RenderClear(renderer); // Очистка экрана
}
int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "Ошибка инициализации SDL: " << SDL_GetError() << endl;
        return -1;
    }

    if (TTF_Init() == -1) {
        cout << "Ошибка инициализации TTF: " << TTF_GetError() << endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Лабиринт с персонажем",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        cout << "Ошибка создания окна: " << SDL_GetError() << endl;
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        cout << "Ошибка создания рендера: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_Surface* tempSurface = SDL_LoadBMP("charactergirl.bmp");
    if (!tempSurface) {
        cout << "Ошибка загрузки изображения: " << SDL_GetError() << endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    TTF_Font* font = TTF_OpenFont("Roboto-Black.ttf", 28); // Подключаем Roboto-Black.ttf
    if (!font) {
        cout << "Ошибка загрузки шрифта: " << TTF_GetError() << endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    SDL_Color textColor = { 255, 255, 255 };
    SDL_Rect textRect = { 150, 300, 340, 80 }; // Позиция текста
    SDL_Color backgroundColor = { 0, 0, 0, 255 }; // Черный фон

    SDL_Texture* characterTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    SDL_Rect playerRect = { CELL_SIZE, CELL_SIZE, 56, 56 };
    bool isRunning = true;
    SDL_Event event;
    bool hasWon = false;

    while (isRunning) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
            // Перезапуск игры
            if (event.type == SDL_KEYDOWN && hasWon) {
                if (event.key.keysym.sym == SDLK_r) {
                    hasWon = false; // Сбросить состояние выигрыша
                    playerRect.x = CELL_SIZE; // Сбросить позицию игрока
                    playerRect.y = CELL_SIZE;
                }
            }
        }

        const Uint8* keystate = SDL_GetKeyboardState(NULL);
        if (!hasWon) {
            if (keystate[SDL_SCANCODE_W]) {
                playerRect.y -= SPEED;
                if (checkCollision(playerRect)) {
                    playerRect.y += SPEED;
                }
            }
            if (keystate[SDL_SCANCODE_A]) {
                playerRect.x -= SPEED;
                if (checkCollision(playerRect)) {
                    playerRect.x += SPEED;
                }
            }
            if (keystate[SDL_SCANCODE_S]) {
                playerRect.y += SPEED;
                if (checkCollision(playerRect)) {
                    playerRect.y -= SPEED;
                }
            }
            if (keystate[SDL_SCANCODE_D]) {
                playerRect.x += SPEED;
                if (checkCollision(playerRect)) {
                    playerRect.x -= SPEED;
                }
            }

            // Проверка достижения выхода
            if (reachedExit(playerRect)) {
                hasWon = true;
            }
        }

        // Очистка экрана
        clearScreen(renderer, backgroundColor);

        // Отрисовка лабиринта и персонажа
        drawMaze(renderer);
        SDL_RenderCopy(renderer, characterTexture, NULL, &playerRect);

        // Отображение текста, если игрок победил
        if (hasWon) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Задаем черный фон
            SDL_RenderFillRect(renderer, &textRect); // Заливаем фон
            displayText(renderer, "You win, balbes!", font, textColor, textRect);
        }

        SDL_RenderPresent(renderer); // Обновление экрана
    }

    // Освобождение ресурсов
    SDL_DestroyTexture(characterTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 0;
}