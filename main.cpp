#include "Game.h"
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

int main() {
    Game game;

    game.init();

    // ≤•∑≈±≥æ∞“Ù¿÷
    mciSendString(L"open images/bgm.mp3 alias bgm", NULL, 0, NULL);
    mciSendString(L"play bgm repeat", NULL, 0, NULL);

    game.showStartScreen();

    while (game.isGameActive()) {
        game.handleInput();
        game.update();
        game.render();
        Sleep(16);
    }

    // πÿ±’±≥æ∞“Ù¿÷
    mciSendString(L"stop bgm", NULL, 0, NULL);
    mciSendString(L"close bgm", NULL, 0, NULL);

    return 0;
}