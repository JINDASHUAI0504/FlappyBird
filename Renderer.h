#ifndef RENDERER_H
#define RENDERER_H

#include <graphics.h>
#include "Bird.h"
#include "Pipe.h"
#include "Constants.h"

class Renderer {
private:
    IMAGE background;
    IMAGE startImg;
    IMAGE guideImg;
    IMAGE endImg;

    IMAGE birdImages[3];
    IMAGE birdDeadImages[3];

    IMAGE pipeUp;
    IMAGE pipeDown;
    IMAGE pipeUpDead;
    IMAGE pipeDownDead;

    // 结束界面标签
    IMAGE scoreLabelImg;
    IMAGE restartLabelImg;

    // 像素数字 0~9
    IMAGE numImgs[10];

    // 金币三帧动画
    IMAGE coinImgs[3];

    int score;
    bool gameOver;
    int frameCounter;
    bool easterEggActive;
    int easterEggScore;

public:
    Renderer();
    ~Renderer();

    void initWindow();
    void loadResources();
    void clear();

    void drawBackground();
    void drawBird(Bird* bird, bool isDead);
    void drawPipes(Pipe* pipes, int count, bool isDead);

    void drawScore(int score);
    void drawPixelScore(int value, int centerX, int y);

    void drawGameOver(int finalScore);
    void drawStartScreen();
    void drawEasterEgg();

    void drawDoubleScoreStatus(int remainingSeconds);
    void drawDoubleScoreGlow();

    void drawPowerUp(int x, int y, int type);

    void setBirdImages(Bird* bird);
    void setEasterEgg(bool active, int score);
};

#endif