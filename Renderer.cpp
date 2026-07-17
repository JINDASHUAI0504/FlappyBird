#include "Renderer.h"
#include <stdio.h>
#include <direct.h>
#include <graphics.h>
#include <windows.h>

#pragma comment(lib, "Msimg32.lib")

// 透明绘制
static void putimage_transparent(int x, int y, IMAGE* img, COLORREF transColor)
{
    if (img == NULL) return;
    if (img->getwidth() <= 0 || img->getheight() <= 0) return;

    HDC dstDC = GetImageHDC(NULL);
    HDC srcDC = GetImageHDC(img);

    TransparentBlt(
        dstDC,
        x, y,
        img->getwidth(),
        img->getheight(),
        srcDC,
        0, 0,
        img->getwidth(),
        img->getheight(),
        transColor
    );
}

// 拉伸管道图片
static void stretch_image_part(
    int dstX, int dstY, int dstW, int dstH,
    IMAGE* img,
    int srcX, int srcY, int srcW, int srcH)
{
    if (img == NULL) return;
    if (dstW <= 0 || dstH <= 0 || srcW <= 0 || srcH <= 0) return;

    HDC dstDC = GetImageHDC(NULL);
    HDC srcDC = GetImageHDC(img);

    StretchBlt(
        dstDC,
        dstX, dstY, dstW, dstH,
        srcDC,
        srcX, srcY, srcW, srcH,
        SRCCOPY
    );
}

Renderer::Renderer() {
    score = 0;
    gameOver = false;
    frameCounter = 0;
    easterEggActive = false;
    easterEggScore = 0;
}

Renderer::~Renderer() {
    closegraph();
}

void Renderer::initWindow() {
    initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
    setbkcolor(WHITE);
    cleardevice();

    printf("=== FlyBird Game ===\n");
    printf("窗口: %d x %d\n", WINDOW_WIDTH, WINDOW_HEIGHT);
}

// 加载所有游戏资源（背景、角色、管道、UI等）
void Renderer::loadResources() {
    printf("加载背景图片...\n");
    if (loadimage(&background, L"images/background.bmp") == 0) {
        printf("背景加载成功\n");
    }
    else {
        printf("背景加载失败\n");
    }

    printf("加载开始界面图片...\n");
    if (loadimage(&startImg, L"images/start.png") == 0) {
        printf("  start.png 加载成功\n");
    }
    else {
        printf("  start.png 加载失败\n");
    }

    if (loadimage(&guideImg, L"images/guid.png") == 0) {
        printf("  guid.png 加载成功\n");
    }
    else {
        printf("  guid.png 加载失败\n");
    }

    printf("加载结束界面图片...\n");
    if (loadimage(&endImg, L"images/end.png") == 0) {
        printf("  end.png 加载成功\n");
    }
    else {
        printf("  end.png 加载失败\n");
    }

    printf("加载结束界面标签图片...\n");
    if (loadimage(&scoreLabelImg, L"images/scoreLabel.png") == 0) {
        printf("  scoreLabel.png 加载成功\n");
    }
    else {
        printf("  scoreLabel.png 加载失败\n");
    }

    if (loadimage(&restartLabelImg, L"images/restartLabel.png") == 0) {
        printf("  restartLabel.png 加载成功\n");
    }
    else {
        printf("  restartLabel.png 加载失败\n");
    }

    printf("加载像素数字图片...\n");
    for (int i = 0; i < 10; i++) {
        wchar_t filename[100];
        swprintf_s(filename, 100, L"images/num%d.png", i);

        if (loadimage(&numImgs[i], filename) == 0) {
            printf("  num%d.png 加载成功\n", i);
        }
        else {
            printf("  num%d.png 加载失败\n", i);
        }
    }

    printf("加载金币动画图片...\n");
    for (int i = 0; i < 3; i++) {
        wchar_t filename[100];
        swprintf_s(filename, 100, L"images/coin%d.png", i);

        if (loadimage(&coinImgs[i], filename) == 0) {
            printf("  coin%d.png 加载成功\n", i);
        }
        else {
            printf("  coin%d.png 加载失败\n", i);
        }
    }

    printf("加载小鸟动画...\n");
    for (int i = 0; i < 3; i++) {
        wchar_t filename[100];
        swprintf_s(filename, 100, L"images/bird%d.png", i);

        if (loadimage(&birdImages[i], filename) == 0) {
            printf("  bird%d.png 加载成功\n", i);
        }
        else {
            printf("  bird%d.png 加载失败\n", i);
        }
    }

    for (int i = 0; i < 3; i++) {
        wchar_t filename[100];
        swprintf_s(filename, 100, L"images/birdy%d.png", i);

        if (loadimage(&birdDeadImages[i], filename) == 0) {
            printf("  birdy%d.png 加载成功\n", i);
        }
        else {
            printf("  birdy%d.png 加载失败\n", i);
            birdDeadImages[i] = birdImages[i];
        }
    }

    printf("加载管道图片...\n");

    if (loadimage(&pipeUp, L"images/down.png") == 0) {
        printf("  上管道图片加载成功\n");
    }
    else {
        printf("  上管道图片加载失败\n");
    }

    if (loadimage(&pipeDown, L"images/up.png") == 0) {
        printf("  下管道图片加载成功\n");
    }
    else {
        printf("  下管道图片加载失败\n");
    }

    if (loadimage(&pipeUpDead, L"images/downy.png") == 0) {
        printf("  死亡上管道加载成功\n");
    }
    else {
        pipeUpDead = pipeUp;
    }

    if (loadimage(&pipeDownDead, L"images/upy.png") == 0) {
        printf("  死亡下管道加载成功\n");
    }
    else {
        pipeDownDead = pipeDown;
    }
}

void Renderer::clear() {
    cleardevice();
}

void Renderer::drawBackground() {
    if (background.getwidth() > 0) {
        putimage(0, 0, &background);
    }
    else {
        setbkcolor(WHITE);
        cleardevice();
    }
}

// 绘制小鸟
void Renderer::drawBird(Bird* bird, bool isDead) {
    IMAGE* img = bird->getCurrentImage(isDead);

    if (img && img->getwidth() > 0) {
        int x = (int)(bird->getX() - img->getwidth() / 2);
        int y = (int)(bird->getY() - img->getheight() / 2);
        putimage_transparent(x, y, img, RGB(255, 255, 255));
    }
}

// 绘制所有管道（支持不同高度）
void Renderer::drawPipes(Pipe* pipes, int count, bool isDead) {
    for (int i = 0; i < count; i++) {
        int pipeX = pipes[i].x;
        int topHeight = pipes[i].topHeight;
        int bottomY = pipes[i].bottomY;

        IMAGE* upImg = isDead ? &pipeUpDead : &pipeUp;
        IMAGE* downImg = isDead ? &pipeDownDead : &pipeDown;

        int drawWidth = PIPE_WIDTH-10;

        // 上管道
        if (topHeight > 0 && upImg->getwidth() > 0) {
            int imgW = upImg->getwidth();
            int imgH = upImg->getheight();

            int capH = 40;
            if (capH > imgH) capH = imgH;

            if (topHeight <= capH) {
                int srcY = imgH - topHeight;
                if (srcY < 0) srcY = 0;

                putimage(pipeX, 0, drawWidth, topHeight, upImg, 0, srcY);
            }
            else {
                int bodySrcH = imgH - capH;
                int bodyDstH = topHeight - capH;

                if (bodySrcH > 0 && bodyDstH > 0) {
                    stretch_image_part(
                        pipeX, 0, drawWidth, bodyDstH,
                        upImg,
                        0, 0, imgW, bodySrcH
                    );
                }

                putimage(
                    pipeX, bodyDstH,
                    drawWidth, capH,
                    upImg,
                    0, imgH - capH
                );
            }
        }

        // 下管道
        if (bottomY < WINDOW_HEIGHT && downImg->getwidth() > 0) {
            int imgW = downImg->getwidth();
            int imgH = downImg->getheight();

            int capH = 40;
            if (capH > imgH) capH = imgH;

            int pipeHeight = WINDOW_HEIGHT - bottomY;
            if (pipeHeight <= 0) continue;

            if (pipeHeight <= capH) {
                putimage(
                    pipeX, bottomY,
                    drawWidth, pipeHeight,
                    downImg,
                    0, 0
                );
            }
            else {
                putimage(
                    pipeX, bottomY,
                    drawWidth, capH,
                    downImg,
                    0, 0
                );

                int bodySrcY = capH;
                int bodySrcH = imgH - capH;
                int bodyDstY = bottomY + capH;
                int bodyDstH = pipeHeight - capH;

                if (bodySrcH > 0 && bodyDstH > 0) {
                    stretch_image_part(
                        pipeX, bodyDstY, drawWidth, bodyDstH,
                        downImg,
                        0, bodySrcY, imgW, bodySrcH
                    );
                }
            }
        }
    }
}

// 绘制分数
void Renderer::drawPixelScore(int value, int centerX, int y) {
    wchar_t scoreStr[20];
    swprintf_s(scoreStr, 20, L"%d", value);

    int len = lstrlenW(scoreStr);
    if (len <= 0) return;

    int digitWidth = numImgs[0].getwidth();
    int digitHeight = numImgs[0].getheight();

    if (digitWidth <= 0 || digitHeight <= 0) return;

    int spacing = 2;
    int totalWidth = len * digitWidth + (len - 1) * spacing;
    int startX = centerX - totalWidth / 2;

    for (int i = 0; i < len; i++) {
        int num = scoreStr[i] - L'0';
        if (num >= 0 && num <= 9) {
            putimage_transparent(
                startX + i * (digitWidth + spacing),
                y,
                &numImgs[num],
                RGB(0, 255, 0)
            );
        }
    }
}

void Renderer::drawScore(int score) {
    drawPixelScore(score, WINDOW_WIDTH / 2, 30);
}

// 根据分数显示彩蛋提示文字
void Renderer::drawEasterEgg() {
    if (!easterEggActive) return;

    setbkmode(TRANSPARENT);

    wchar_t eggText[100];

    if (easterEggScore == 10 || easterEggScore == 11) {
        swprintf_s(eggText, 100, L"=== 这操作，简直就是人形脚本！ ===");
    }
    else if (easterEggScore == 20 || easterEggScore == 21) {
        swprintf_s(eggText, 100, L"=== 大神，请收下我的膝盖! ===");
    }
    else if (easterEggScore == 50 || easterEggScore == 51) {
        swprintf_s(eggText, 100, L"=== 这操作也太细了! ===");
    }
    else if (easterEggScore == 80 || easterEggScore == 81) {
        swprintf_s(eggText, 100, L"=== 有你在，这游戏突然变得好简单 ===");
    }
    else if (easterEggScore == 100 || easterEggScore == 101) {
        swprintf_s(eggText, 100, L"=== 这不仅是技术，更是艺术。 ===");
    }
    else if (easterEggScore == 150 || easterEggScore == 151) {
        swprintf_s(eggText, 100, L"=== 游戏的上限由你来定义! ===");
    }
    else {
        return;
    }

    settextcolor(RGB(255, 215, 0));
    settextstyle(28, 0, L"Arial");

    int textWidth = textwidth(eggText);
    int x = (WINDOW_WIDTH - textWidth) / 2;
    int y = 100;

    outtextxy(x, y, eggText);
}

// 绘制游戏结束界面
void Renderer::drawGameOver(int finalScore) {
    setbkmode(TRANSPARENT);

    if (endImg.getwidth() > 0) {
        int x = (WINDOW_WIDTH - endImg.getwidth()) / 2;
        int y = (WINDOW_HEIGHT - endImg.getheight()) / 2 - 100;
        putimage_transparent(x, y, &endImg, RGB(255, 255, 255));
    }

    if (scoreLabelImg.getwidth() > 0) {
        int x = (WINDOW_WIDTH - scoreLabelImg.getwidth()) / 2;
        int y = WINDOW_HEIGHT / 2 + 10;
        putimage_transparent(x, y, &scoreLabelImg, RGB(0, 255, 0));
    }

    drawPixelScore(finalScore, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 55);

    if (restartLabelImg.getwidth() > 0) {
        int x = (WINDOW_WIDTH - restartLabelImg.getwidth()) / 2;
        int y = WINDOW_HEIGHT / 2 + 120;
        putimage_transparent(x, y, &restartLabelImg, RGB(0, 255, 0));
    }
}

void Renderer::drawStartScreen() {
    clear();
    drawBackground();

    if (startImg.getwidth() > 0) {
        int x = (WINDOW_WIDTH - startImg.getwidth()) / 2;
        int y = (WINDOW_HEIGHT - startImg.getheight()) / 2 - 80;
        putimage_transparent(x, y, &startImg, RGB(255, 255, 255));
    }

    if (guideImg.getwidth() > 0) {
        int x = (WINDOW_WIDTH - guideImg.getwidth()) / 2;
        int y = (WINDOW_HEIGHT - guideImg.getheight()) / 2 + 60;
        putimage_transparent(x, y, &guideImg, RGB(255, 255, 255));
    }
}

void Renderer::setBirdImages(Bird* bird) {
    bird->setImages(&birdImages[0], &birdImages[1], &birdImages[2]);
    bird->setDeadImages(&birdDeadImages[0], &birdDeadImages[1], &birdDeadImages[2]);
}

void Renderer::setEasterEgg(bool active, int score) {
    easterEggActive = active;
    easterEggScore = score;
}

// 显示双倍积分剩余时间
void Renderer::drawDoubleScoreStatus(int remainingSeconds) {
    setbkmode(TRANSPARENT);
    settextcolor(RGB(255, 215, 0));
    settextstyle(20, 0, L"Arial");

    wchar_t status[50];
    swprintf_s(status, 50, L"2x SCORE! %ds", remainingSeconds + 1);

    int textWidth = textwidth(status);
    outtextxy(WINDOW_WIDTH - textWidth - 10, 10, status);
}

// 绘制屏幕边缘闪烁效果
void Renderer::drawDoubleScoreGlow() {
    static int blinkCounter = 0;
    blinkCounter++;

    if (blinkCounter > 5) {
        blinkCounter = 0;
    }

    if (blinkCounter < 3) {
        setlinecolor(RGB(255, 215, 0));
        setfillcolor(RGB(255, 215, 0));

        for (int i = 0; i < 5; i++) {
            rectangle(i, i, WINDOW_WIDTH - i, WINDOW_HEIGHT - i);
        }
    }
}

// 绘制道具金币
void Renderer::drawPowerUp(int x, int y, int type) {
    if (type == 1) {
        static int coinFrameCounter = 0;
        static int coinFrame = 0;

        coinFrameCounter++;
        if (coinFrameCounter >= 8) {
            coinFrameCounter = 0;
            coinFrame = (coinFrame + 1) % 3;
        }

        if (coinImgs[coinFrame].getwidth() > 0) {
            putimage_transparent(x, y, &coinImgs[coinFrame], RGB(0, 255, 0));
        }
    }
}