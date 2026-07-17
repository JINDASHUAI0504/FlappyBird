#ifndef GAME_H
#define GAME_H

#include "Bird.h"
#include "Pipe.h"
#include "Renderer.h"

// 道具类型
enum PowerUpType {
    POWERUP_NONE,
    POWERUP_DOUBLE_SCORE
};

// 道具结构
struct PowerUp {
    int x;
    int y;
    bool active;
    PowerUpType type;
    int width;
    int height;
};

class Game {
private:
    Bird* bird;
    PipeManager* pipeManager;
    Renderer* renderer;

    int score;
    bool isGameOver;
    bool isRunning;
    bool isDead;
    bool easterEggShown;

    int lastScoreCount;

    // 双倍积分
    bool doubleScoreActive;
    int doubleScoreTimer;
    int doubleScoreDuration;

    // 原始速度
    int originalPipeSpeed;

    // 道具相关
    PowerUp powerUp;
    int powerUpSpawnTimer;
    int powerUpSpawnDelay;

    void updatePhysics();
    void updatePipes();
    void checkCollisions();
    void updateScore();
    void updatePowerUp();
    void spawnPowerUp();
    void checkPowerUpCollision();

public:
    Game();
    ~Game();

    void init();
    void handleInput();
    void update();
    void render();
    bool isGameActive();
    void restart();
    void showStartScreen();
};

#endif