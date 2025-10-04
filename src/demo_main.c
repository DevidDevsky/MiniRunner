#include <stdlib.h>
#include <time.h>
#include "game.h"

int main() {
    srand((unsigned)time(NULL));
    setenv("DEMO_MODE", "1", 1);
    setenv("START_DIFFICULTY", "5", 1); // старт с более насыщенного уровня
    game_loop();
    return 0;
}
