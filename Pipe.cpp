#include "Pipe.h"
#include <stdlib.h>
#include <time.h>

PipeManager::PipeManager() {
    pipeCount = 0;
    speed = PIPE_SPEED;   // 初始速度
    pipeGap = PIPE_GAP;   // 初始间隙
    srand((unsigned int)time(NULL));
}

PipeManager::~PipeManager() {
}

void PipeManager::init() {
    pipeCount = 0;
    speed = PIPE_SPEED;
    pipeGap = PIPE_GAP;
    addPipe();
}

void PipeManager::update() 
{
    for (int i = 0; i < pipeCount; i++) 
    {
        pipes[i].x -= speed;
    }

    removeOutOfScreen();

    if (pipeCount == 0 || pipes[pipeCount - 1].x <= WINDOW_WIDTH - PIPE_INTERVAL) 
    {
        if (pipeCount < MAX_PIPES) 
        {
            addPipe();
        }
    }
}

void PipeManager::addPipe() {
    if (pipeCount >= MAX_PIPES) return;

    // 给上管道一个合理随机范围
    int minTop = 120;
    int maxTop = WINDOW_HEIGHT - pipeGap - 120;

    if (maxTop <= minTop) {
        maxTop = minTop + 1;
    }

    int topHeight = rand() % (maxTop - minTop + 1) + minTop;
    int bottomY = topHeight + pipeGap;

    pipes[pipeCount].x = WINDOW_WIDTH;
    pipes[pipeCount].topHeight = topHeight;
    pipes[pipeCount].bottomY = bottomY;
    pipes[pipeCount].scored = false;

    pipeCount++;
}

void PipeManager::removeOutOfScreen() {
    int removeCount = 0;

    for (int i = 0; i < pipeCount; i++) {
        if (pipes[i].x + PIPE_WIDTH >= 0) {
            if (removeCount > 0) {
                pipes[i - removeCount] = pipes[i];
            }
        }
        else {
            removeCount++;
        }
    }

    pipeCount -= removeCount;
}

void PipeManager::reset() {
    pipeCount = 0;
    speed = PIPE_SPEED;
    pipeGap = PIPE_GAP;
    init();
}

void PipeManager::updateDifficulty(int score) {
    // 每15分升一级
    int level = score / 15;

    // 速度：初始速度 + level，最大不超过8
    speed = PIPE_SPEED + level;
    if (speed > 8) {
        speed = 8;
    }

    // 间隙：初始间隙 - level*15，最小不低于120
    pipeGap = PIPE_GAP - level * 15;
    if (pipeGap < 120) {
        pipeGap = 120;
    }
}