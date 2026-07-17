#ifndef BIRD_H
#define BIRD_H

#include <graphics.h>

struct Pipe;

class Bird {
private:
    float x, y;                 //小鸟的坐标
    float velocity;
    int width, height;          //小鸟图片的尺寸
    IMAGE* images[3];
    IMAGE* deadImages[3];
    int currentFrame;

public:
    Bird();
    ~Bird();                    //清除小鸟资源

    void init(int startX, int startY);
    void update();
    void jump();
    void reset();
    void nextFrame();
    bool checkCollision(const struct Pipe& pipe);    //碰撞检测

    float getX() const { return x; }
    float getY() const { return y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    IMAGE* getCurrentImage(bool isDead);

    void setImages(IMAGE* img0, IMAGE* img1, IMAGE* img2);
    void setDeadImages(IMAGE* img0, IMAGE* img1, IMAGE* img2);
};

#endif