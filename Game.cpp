#include "Game.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Constants.h"

Game::Game() {
    bird = nullptr;
    pipeManager = nullptr;
    renderer = nullptr;
    score = 0;
    isGameOver = false;
    isRunning = true;
    isDead = false;
    easterEggShown = false;
    lastScoreCount = -1;

    // 双倍积分
    doubleScoreActive = false;
    doubleScoreTimer = 0;
    doubleScoreDuration = 120;   // 2秒

    // 原始速度
    originalPipeSpeed = PIPE_SPEED;

    // 道具
    powerUp.active = false;
    powerUp.type = POWERUP_NONE;
    powerUp.width = 20;
    powerUp.height = 20;
    powerUpSpawnTimer = 0;
    powerUpSpawnDelay = 180;     // 3秒生成一次
}

Game::~Game() {
    delete bird;
    delete pipeManager;
    delete renderer;
}

void Game::init() {
    renderer = new Renderer();
    renderer->initWindow();
    renderer->loadResources();

    bird = new Bird();
    bird->init(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    renderer->setBirdImages(bird);

    pipeManager = new PipeManager();
    pipeManager->init();

    score = 0;
    isGameOver = false;
    isDead = false;
    easterEggShown = false;
    lastScoreCount = -1;

    doubleScoreActive = false;
    doubleScoreTimer = 0;
    powerUp.active = false;
    powerUpSpawnTimer = 0;

    // 保持原始速度
    pipeManager->setSpeed(originalPipeSpeed);

    srand((unsigned int)time(NULL));
}

void Game::handleInput() {
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
        if (!isGameOver) {
            bird->jump();
        }
        Sleep(50);
    }

    if (GetAsyncKeyState('R') & 0x8000) {
        restart();
        Sleep(50);
    }

    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
        isRunning = false;
        Sleep(50);
    }
}

void Game::updatePhysics() {
    bird->update();
}

void Game::updatePipes() {
    pipeManager->update();
}

void Game::checkCollisions() {
    if (bird->getY() - bird->getHeight() / 2 <= 3 ||
        bird->getY() + bird->getHeight() / 2 >= WINDOW_HEIGHT - 3) {
        isGameOver = true;
        isDead = true;
        return;
    }

    Pipe* pipes = pipeManager->getPipes();
    int pipeCount = pipeManager->getPipeCount();

    for (int i = 0; i < pipeCount; i++) {
        if (bird->checkCollision(pipes[i])) {
            isGameOver = true;
            isDead = true;
            return;
        }
    }
}

void Game::updateScore() {
    Pipe* pipes = pipeManager->getPipes();
    int pipeCount = pipeManager->getPipeCount();

    for (int i = 0; i < pipeCount; i++)
    {
        int pipeRight = pipes[i].x + PIPE_WIDTH;
        int birdLeft = (int)(bird->getX() - bird->getWidth() / 2);

        if (pipeRight < birdLeft && !pipes[i].scored) {
            int oldScore = score;
            int addScore = doubleScoreActive ? 2 : 1;
            score += addScore;
            pipes[i].scored = true;

            // 彩蛋触发
            if (!easterEggShown) {
                if (oldScore < 10 && score >= 10) {
                    easterEggShown = true;
                }
                else if (oldScore < 20 && score >= 20) {
                    easterEggShown = true;
                }
                else if (oldScore < 50 && score >= 50) {
                    easterEggShown = true;
                }
            }
        }
    }
}

void Game::spawnPowerUp() {
    Pipe* pipes = pipeManager->getPipes();
    int pipeCount = pipeManager->getPipeCount();

    if (pipeCount <= 0) return;

    // 选择屏幕右半部分、还没出屏的管道
    int candidateIndex = -1;
    for (int i = 0; i < pipeCount; i++) {
        if (pipes[i].x > WINDOW_WIDTH / 2 && pipes[i].x < WINDOW_WIDTH) {
            candidateIndex = i;
            break;
        }
    }

    if (candidateIndex == -1) {
        candidateIndex = rand() % pipeCount;
    }

    int gapTop = pipes[candidateIndex].topHeight;
    int gapBottom = pipes[candidateIndex].bottomY;

    powerUp.width = 20;
    powerUp.height = 20;
    powerUp.type = POWERUP_DOUBLE_SCORE;
    powerUp.active = true;

    // 金币放在管道的中间位置
    powerUp.x = pipes[candidateIndex].x + PIPE_WIDTH / 2 - powerUp.width / 2;

    // 金币Y坐标限制在管道间隙内
    int minY = gapTop + 10;
    int maxY = gapBottom - powerUp.height - 10;

    if (maxY >= minY) {
        powerUp.y = rand() % (maxY - minY + 1) + minY;
    }
    else {
        powerUp.y = gapTop + (gapBottom - gapTop - powerUp.height) / 2;
    }
}

void Game::checkPowerUpCollision()
{
    if (!powerUp.active) return;

    int birdLeft = (int)(bird->getX() - bird->getWidth() / 2);
    int birdRight = (int)(bird->getX() + bird->getWidth() / 2);
    int birdTop = (int)(bird->getY() - bird->getHeight() / 2);
    int birdBottom = (int)(bird->getY() + bird->getHeight() / 2);

    if (birdRight > powerUp.x && birdLeft < powerUp.x + powerUp.width &&
        birdBottom > powerUp.y && birdTop < powerUp.y + powerUp.height) {

        powerUp.active = false;

        if (powerUp.type == POWERUP_DOUBLE_SCORE)
        {
            // 双倍积分
            doubleScoreActive = true;
            doubleScoreTimer = doubleScoreDuration;
        }
    }
}

void Game::updatePowerUp() {
    // 生成道具
    powerUpSpawnTimer++;
    if (powerUpSpawnTimer >= powerUpSpawnDelay && !powerUp.active) {
        powerUpSpawnTimer = 0;
        spawnPowerUp();
    }

    // 移动道具，和管道同速
    if (powerUp.active) {
        powerUp.x -= pipeManager->getSpeed();

        if (powerUp.x + powerUp.width < 0) {
            powerUp.active = false;
        }
    }
}

void Game::update()
{
    static int animCounter = 0;
    animCounter++;
    if (animCounter >= 6)
    {
        animCounter = 0;
        bird->nextFrame();
    }

    // 更新双倍积分计时器
    if (doubleScoreActive)
    {
        doubleScoreTimer--;
        if (doubleScoreTimer <= 0)
        {
            doubleScoreActive = false;
        }
    }

    if (!isGameOver) {
        updatePhysics();
        updatePipes();
        updatePowerUp();
        checkPowerUpCollision();
        checkCollisions();
        updateScore();
    }
}

void Game::render() {
    renderer->clear();
    renderer->drawBackground();
    renderer->drawPipes(pipeManager->getPipes(), pipeManager->getPipeCount(), isDead);
    renderer->drawBird(bird, isDead);
    renderer->drawScore(score);

    if (doubleScoreActive)
    {
        renderer->drawDoubleScoreStatus(doubleScoreTimer / 60);
        renderer->drawDoubleScoreGlow();
    }

    if (powerUp.active)
    {
        renderer->drawPowerUp(powerUp.x, powerUp.y, powerUp.type);
    }

    renderer->setEasterEgg(easterEggShown, score);
    renderer->drawEasterEgg();

    if (isGameOver)
    {
        renderer->drawGameOver(score);
    }
}

bool Game::isGameActive()
{
    return isRunning;
}

void Game::restart()
{
    bird->reset();
    pipeManager->reset();
    score = 0;
    isGameOver = false;
    isDead = false;
    easterEggShown = false;
    lastScoreCount = -1;

    doubleScoreActive = false;
    doubleScoreTimer = 0;
    powerUp.active = false;
    powerUpSpawnTimer = 0;

    // 重开时恢复原始速度
    pipeManager->setSpeed(originalPipeSpeed);
}

void Game::showStartScreen() {
    renderer->drawStartScreen();
    while (true) {
        if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
            break;
        }
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            isRunning = false;
            break;
        }
        Sleep(50);
    }
}