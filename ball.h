#pragma once
#ifndef BALL_H_
#define BALL_H_

#include "Entity.h"

#include <math.h>

// Xác định tốc độ bóng tính trên giây
const float BALL_SPEED = 550;

class Ball : public Entity {
public:
    Ball(SDL_Renderer* renderer);
    ~Ball();

    void Update(float delta);
    void Render(float delta);

    void SetDirection(float dirx, float diry);

    float dirx, diry;

private:
    SDL_Texture* texture;

};

#endif
