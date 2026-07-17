#ifndef PIPE_H
#define PIPE_H

#include "Constants.h"

struct Pipe {
    int x;
    int topHeight;
    int bottomY;
    bool scored;
};

class PipeManager {
private:
    Pipe pipes[MAX_PIPES];
    int pipeCount;

    int speed;      // 当前管道速度
    int pipeGap;    // 当前管道间隙

public:
    PipeManager();
    ~PipeManager();

    void init();
    void update();
    void addPipe();
    void removeOutOfScreen();
    void reset();

    // 根据当前分数更新难度
    void updateDifficulty(int score);

    Pipe* getPipes() { return pipes; }
    int getPipeCount() const { return pipeCount; }
    int getSpeed() const { return speed; }
    int getPipeGap() const { return pipeGap; }

    void setSpeed(int s) { speed = s; }
};

#endif