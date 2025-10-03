#include <stdlib.h>
#include <time.h>
#include "game.h"

int main() {
    srand(time(NULL));
    game_loop();
    return 0;
}