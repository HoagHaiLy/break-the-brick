#pragma once
#ifndef GAME_H_
#define GAME_H_

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <stdio.h>
#include <SDL_ttf.h>

#include "Board.h"
#include "Paddle.h"
#include "Ball.h"



#define FPS_DELAY 500




class Game {
public:
    Game();
    ~Game();

    bool Init();
    void Run();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;

    SDL_Texture* texture;
    SDL_Texture* menutexture;

    // thời gian
    unsigned int lasttick, fpstick, fps, framecount;

    // Test
    float testx, testy;

    Board* board;
    Paddle* paddle;
    Ball* ball;
    bool paddlestick;

    int oldBrickCount = -1;

    void Clean();
    void Update(float delta);
    void Render(float delta);

    void NewGame();
    void ResetPaddle();
    void StickBall();

    void SetPaddleX(float x);
    void CheckBoardCollisions();
    float GetReflection(float hitx);
    void CheckPaddleCollisions();
    void CheckBrickCollisions();
    void CheckBrickCollisions2();
    void BallBrickResponse(int dirindex);
    int GetBrickCount();

    int ShowMenu(SDL_Surface* des,TTF_Font* font);
    // void PlayOneShotSound(const char* name);
};

#endif
 