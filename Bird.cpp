#include "Bird.h"
#include "Pipe.h"
#include "Constants.h"

Bird::Bird()
{
    x = 0.0f;
    y = 0.0f;
    velocity = 0.0f;
    width = 0;
    height = 0;
    currentFrame = 0;

    for (int i = 0; i < 3; i++)
    {
        images[i] = nullptr;
        deadImages[i] = nullptr;
    }
}

Bird::~Bird() {}

void Bird::init(int startX, int startY)
{
    x = (float)startX;
    y = (float)startY;
    velocity = 0.0f;
    currentFrame = 0;
}

void Bird::update()
{
    velocity += GRAVITY;  //受重力影响
    y += velocity;        //更新Y坐标
}

void Bird::jump()
{
    velocity = JUMP_POWER;  //设置向上的初速度
}

void Bird::reset()
{
    y = (float)(WINDOW_HEIGHT / 2);
    velocity = 0.0f;
    currentFrame = 0;
}

void Bird::nextFrame()
{
    currentFrame = (currentFrame + 1) % 3;
}

bool Bird::checkCollision(const Pipe& pipe)
{
    // 小鸟碰撞箱适当缩小，减少误判
    int shrinkX = 8;
    int shrinkY = 8;

    int birdLeft = (int)(x - width / 2) + shrinkX;
    int birdRight = (int)(x + width / 2) - shrinkX;
    int birdTop = (int)(y - height / 2) + shrinkY;
    int birdBottom = (int)(y + height / 2) - shrinkY;

    // 管道横向范围
    int pipeLeft = pipe.x;
    int pipeRight = pipe.x + PIPE_WIDTH;

    // 上管道矩形
    int topPipeTop = 0;
    int topPipeBottom = pipe.topHeight;

    // 下管道矩形
    int bottomPipeTop = pipe.bottomY;
    int bottomPipeBottom = WINDOW_HEIGHT;

    // 横向先重叠
    bool overlapX = (birdRight > pipeLeft && birdLeft < pipeRight);

    if (!overlapX) 
    {
        return false;
    }

    // 和上管道碰撞
    bool hitTopPipe = (birdBottom > topPipeTop && birdTop < topPipeBottom);

    // 和下管道碰撞
    bool hitBottomPipe = (birdBottom > bottomPipeTop && birdTop < bottomPipeBottom);

    return hitTopPipe || hitBottomPipe;
}

IMAGE* Bird::getCurrentImage(bool isDead)
{
    if (isDead)
    {
        if (deadImages[currentFrame])
        {
            return deadImages[currentFrame];
        }
        return images[currentFrame];
    }
    return images[currentFrame];
}

void Bird::setImages(IMAGE* img0, IMAGE* img1, IMAGE* img2)
{
    images[0] = img0;
    images[1] = img1;
    images[2] = img2;

    if (images[0])
    {
        width = images[0]->getwidth();
        height = images[0]->getheight();
    }
}

void Bird::setDeadImages(IMAGE* img0, IMAGE* img1, IMAGE* img2)
{
    deadImages[0] = img0;
    deadImages[1] = img1;
    deadImages[2] = img2;
}