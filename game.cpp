#include "Game.h"




Game::Game() {
    window = 0;
    renderer = 0;
}

Game::~Game() {

}


bool Game::Init() {
    SDL_Init(SDL_INIT_VIDEO);

    // Create window
    window = SDL_CreateWindow("Break the bricks",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    if (!window) {
        std::cout << "Error creating window:" << SDL_GetError() << std::endl;
        return false;
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cout << "Error creating renderer:" << SDL_GetError() << std::endl;
        return false;
    }


    // Khởi tạo thời gian
    lasttick = SDL_GetTicks();
    fpstick = lasttick;
    fps = 0;
    framecount = 0; 

    testx = 0;
    testy = 0;

    return true;
}

void Game::Clean() {
    // Clean resources
    SDL_DestroyTexture(texture);

    // Clean renderer and window
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

}

void Game::Run() {
    board = new Board(renderer);
    paddle = new Paddle(renderer);
    ball = new Ball(renderer);

    NewGame();

    // Vòng lặp chính
    while (1) {
        // Xử lí 
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                break;
            }
        }

       // Tính toán delta và fps
        unsigned int curtick = SDL_GetTicks();
        float delta = (curtick - lasttick) / 1000.0f;
        if (curtick - fpstick >= FPS_DELAY) {
            fps = framecount * (1000.0f / (curtick - fpstick));
            fpstick = curtick;
            framecount = 0;
            char buf[100];
            snprintf(buf, 100, "Break the bricks (fps: %u) (bricks: %1.02f)", fps, (float)oldBrickCount / (BOARD_WIDTH * BOARD_HEIGHT));
            SDL_SetWindowTitle(window, buf);
        }
        else {
            framecount++;
        }
        lasttick = curtick;

        

        // Update and render  game
        Update(delta);
        Render(delta);  
    }

    delete board;
    delete paddle;
    delete ball;

    Clean();

    SDL_Quit();
}

void Game::NewGame() {
    board->CreateLevel();
    ResetPaddle();
}

void Game::ResetPaddle() {
    paddlestick = true;
    StickBall();
}

void Game::StickBall() {
    ball->x = paddle->x + paddle->width / 2 - ball->width / 2;
    ball->y = paddle->y - ball->height;
}

void Game::Update(float delta) {
    // logic của game

    // Input
    int mx, my;
    Uint8 mstate = SDL_GetMouseState(&mx, &my);
    SetPaddleX(mx - paddle->width / 2.0f);

    if (mstate & SDL_BUTTON(1)) {
        if (paddlestick) {
            paddlestick = false;
            ball->SetDirection(1, -1);
        }
    }

    if (paddlestick) {
        StickBall();
    }

    CheckBoardCollisions();
    CheckPaddleCollisions();
    CheckBrickCollisions2();

    if (GetBrickCount() == 0) {
        NewGame();
    }

    board->Update(delta);
    paddle->Update(delta);

    if (!paddlestick) {
        ball->Update(delta);
    }
}

void Game::SetPaddleX(float x) {
    float newx;
    if (x < board->x) {
        //  Giới hạn trên
        newx = board->x;
    }
    else if (x + paddle->width > board->x + board->width) {
        //Giới hạn dưới
        newx = board->x + board->width - paddle->width;
    }
    else {
        newx = x;
    }
    paddle->x = newx;
}

void Game::CheckBoardCollisions() {
    // Va chạm trên và dưới
    if (ball->y < board->y) {
        // Trên
        // Giữ bóng trong bảng và phản xạ theo hướng y
        ball->y = board->y;
        ball->diry *= -1;


    }
    else if (ball->y + ball->height > board->y + board->height) {
        // Phía dưới
        ResetPaddle();
    }

    // Va chạm trái và phải
    if (ball->x <= board->x) {
        // Trái
        // Giữ bóng trong bảng và phản xạ hướng x
        
        ball->x = board->x;
        ball->dirx *= -1;

    }
    else if (ball->x + ball->width >= board->x + board->width) {
        // Phải
        // Giữ bóng trong bảng và phản xạ hướng x
        
        ball->x = board->x + board->width - ball->width;
        ball->dirx *= -1;
    }
}

float Game::GetReflection(float hitx) {
    // Đảm bảo rằng biến hitx nằm trong chiều rộng của paddle
    if (hitx < 0) {
        hitx = 0;
    }
    else if (hitx > paddle->width) {
        hitx = paddle->width;
    }

    // Mọi thứ ở bên trái tâm của paddle được phản chiếu sang bên trái
    // trong khi mọi thứ bên phải của tâm được phản ánh sang bên phải
    
    hitx -= paddle->width / 2.0f;

    // Chia tỷ lệ phản xạ, làm cho nó rơi vào phạm vi - 2.0f đến 2.0f
    
    return 2.0f * (hitx / (paddle->width / 2.0f));
}


void Game::CheckPaddleCollisions() {
    // Lấy tọa độ trung tâm của quả bóng
    float ballcenterx = ball->x + ball->width / 2.0f;

    // Kiểm tra va cham của paddle
    if (ball->Collides(paddle)) {
        ball->y = paddle->y - ball->height;
        ball->SetDirection(GetReflection(ballcenterx - paddle->x), -1);
    }
}

void Game::CheckBrickCollisions() {
    for (int i = 0; i < BOARD_WIDTH; i++) {
        for (int j = 0; j < BOARD_HEIGHT; j++) {
            Brick brick = board->bricks[i][j];

            // Kiẻm tra brick
            if (brick.state) {
                // Brick ở tọa độ x và y
                float brickx = board->brickoffsetx + board->x + i * BOARD_BRWIDTH;
                float bricky = board->brickoffsety + board->y + j * BOARD_BRHEIGHT;

                // Kiểm tra va chạm giữa bóng với gạch
                // Xác định va chạm bằng cách sử dụng nửa chiều rộng của các hình chữ nhật
                // Các link tham khảo va chạm
                // http://stackoverflow.com/questions/16198437/minkowski-sum-for-rectangle-intersection-calculation
                // http://gamedev.stackexchange.com/questions/29786/a-simple-2d-rectangle-collision-algorithm-that-also-determines-which-sides-that
                // http://gamedev.stackexchange.com/questions/24078/which-side-was-hit/24091#24091
                float w = 0.5f * (ball->width + BOARD_BRWIDTH);
                float h = 0.5f * (ball->height + BOARD_BRHEIGHT);
                float dx = (ball->x + 0.5f * ball->width) - (brickx + 0.5f * BOARD_BRWIDTH);
                float dy = (ball->y + 0.5f * ball->height) - (bricky + 0.5f * BOARD_BRHEIGHT);

                if (fabs(dx) <= w && fabs(dy) <= h) {
                    
                    //phát hiện va chạm
                    board->bricks[i][j].state = false;

                    float wy = w * dy;
                    float hx = h * dx;

                    if (wy > hx) {
                        if (wy > -hx) {
                            // dưới (y bị lật)
                            BallBrickResponse(3);
                        }
                        else {
                            //trái
                            BallBrickResponse(0);
                        }
                    }
                    else {
                        if (wy > -hx) {
                            // phải
                            BallBrickResponse(2);
                        }
                        else {
                            // trên (y bị lật)
                            BallBrickResponse(1);
                        }
                    }
                    return;
                }
            }
        }
    }
}

void Game::CheckBrickCollisions2() {
    for (int i = 0; i < BOARD_WIDTH; i++) {
        for (int j = 0; j < BOARD_HEIGHT; j++) {
            Brick brick = board->bricks[i][j];

            // Kiểm tra brick
            if (brick.state) {
                // Brick ở tọa đọ x và y
                float brickx = board->brickoffsetx + board->x + i * BOARD_BRWIDTH;
                float bricky = board->brickoffsety + board->y + j * BOARD_BRHEIGHT;

                // Tọa độ tâm x và y của quả bóng
                
                float ballcenterx = ball->x + 0.5f * ball->width;
                float ballcentery = ball->y + 0.5f * ball->height;

                
                //Tâm của viên gạch tọa độ x và y
                float brickcenterx = brickx + 0.5f * BOARD_BRWIDTH;
                float brickcentery = bricky + 0.5f * BOARD_BRHEIGHT;

                if (ball->x <= brickx + BOARD_BRWIDTH && ball->x + ball->width >= brickx && ball->y <= bricky + BOARD_BRHEIGHT && ball->y + ball->height >= bricky) {
                    // phát hiện va chạm, loại bỏ viên gạch
                    board->bricks[i][j].state = false;

                    // Giả sử bóng đi đủ chậm để không xuyên qua các viên gạch

                    // Tính kích thước y
                    float ymin = 0;
                    if (bricky > ball->y) {
                        ymin = bricky;
                    }
                    else {
                        ymin = ball->y;
                    }

                    float ymax = 0;
                    if (bricky + BOARD_BRHEIGHT < ball->y + ball->height) {
                        ymax = bricky + BOARD_BRHEIGHT;
                    }
                    else {
                        ymax = ball->y + ball->height;
                    }

                    float ysize = ymax - ymin;

                    
                    //Tính toán kích thước x
                    float xmin = 0;
                    if (brickx > ball->x) {
                        xmin = brickx;
                    }
                    else {
                        xmin = ball->x;
                    }

                    float xmax = 0;
                    if (brickx + BOARD_BRWIDTH < ball->x + ball->width) {
                        xmax = brickx + BOARD_BRWIDTH;
                    }
                    else {
                        xmax = ball->x + ball->width;
                    }

                    float xsize = xmax - xmin;

                    // gốc nằm ở góc trên bên trái của màn hình
                    // Đặt phản ứng va chạm
                    if (xsize > ysize) {
                        if (ballcentery > brickcentery) {
                            //dưới
                            ball->y += ysize + 0.01f; // Di chuyển ra khỏi va chạm
                            
                            BallBrickResponse(3);
                        }
                        else {
                            // trên
                            ball->y -= ysize + 0.01f; //Di chuyển ra khỏi va chạm
                            BallBrickResponse(1);
                        }
                    }
                    else {
                        if (ballcenterx < brickcenterx) {
                            // trái
                            ball->x -= xsize + 0.01f; // Di chuyển ra khỏi va chạm
                            BallBrickResponse(0);
                        }
                        else {
                            // phải
                            ball->x += xsize + 0.01f; // Di chuyển ra khỏi va chạm
                            BallBrickResponse(2);
                        }
                    }

                    return;
                }
            }
        }
    }
}

void Game::BallBrickResponse(int dirindex) {
    // Hướng 0: trái, 1: trên, 2: phải, 3: dưới

    // Hướng
    int mulx = 1;
    int muly = 1;

    if (ball->dirx > 0) {
        // Quả cầu đang chuyển động theo chiều dương x
        if (ball->diry > 0) {
            // Quả cầu đang chuyển động theo chiều dương y
            // +1 +1
            if (dirindex == 0 || dirindex == 3) {
                mulx = -1;
            }
            else {
                muly = -1;
            }
        }
        else if (ball->diry < 0) {
            // Quả cầu đang chuyển động theo chiều âm y
            // +1 -1
            if (dirindex == 0 || dirindex == 1) {
                mulx = -1;
            }
            else {
                muly = -1;
            }
        }
    }
    else if (ball->dirx < 0) {
        // Quả cầu đang chuyển động theo chiều âm x
        if (ball->diry > 0) {
            // Quả cầu đang chuyển động theo chiều dương y
            // -1 +1
            if (dirindex == 2 || dirindex == 3) {
                mulx = -1;
            }
            else {
                muly = -1;
            }
        }
        else if (ball->diry < 0) {
            // Quả cầu đang chuyển động theo chiều âm y
            // -1 -1
            if (dirindex == 1 || dirindex == 2) {
                mulx = -1;
            }
            else {
                muly = -1;
            }
        }
    }

    // Đặt hướng mới của quả bóng, bằng cách nhân với hướng cũ
    // với các yếu tố hướng xác định
    
    ball->SetDirection(mulx * ball->dirx, muly * ball->diry);

}

int Game::GetBrickCount() {
    int brickcount = 0;
    for (int i = 0; i < BOARD_WIDTH; i++) {
        for (int j = 0; j < BOARD_HEIGHT; j++) {
            Brick brick = board->bricks[i][j];
            if (brick.state) {
                brickcount++;
            }
        }
    }

    if (brickcount != oldBrickCount)
    {
        oldBrickCount = brickcount;
    }
    return brickcount;
}

void Game::Render(float delta) {
    SDL_RenderClear(renderer);

    board->Render(delta);
    paddle->Render(delta);
    ball->Render(delta);

    SDL_RenderPresent(renderer);
}


