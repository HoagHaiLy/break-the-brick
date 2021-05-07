#pragma once
#ifndef BOARD_H_
#define BOARD_H_

#include "Entity.h"
#include <stdlib.h>
#include <time.h>

// Xác định kích thước của bảng 
#define BOARD_WIDTH 12
#define BOARD_HEIGHT 12
#define BOARD_BRWIDTH 64
#define BOARD_BRHEIGHT 24

class Brick {
public:
    int type;
    bool state;
};

class Board : public Entity {
public:
    Board(SDL_Renderer* renderer);
    ~Board();

    void Update(float delta);
    void Render(float delta);
    void CreateLevel();

    float brickoffsetx, brickoffsety;

    
    //Xác định mảng gạch hai chiều
    Brick bricks[BOARD_WIDTH][BOARD_HEIGHT];

private:
    SDL_Texture* bricktexture;
    SDL_Texture* sidetexture;
};

#endif
