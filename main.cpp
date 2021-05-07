#include "game.h"





int main(int argc, char* argv[]) {

  
    // Tạo đối tượng game
    Game* game = new Game();

    //  Khởi tạo và chạy trò chơi
   
    if (game->Init()) {
        game->Run();
    }

    // Clean up
    delete game;
    return 0;
}
