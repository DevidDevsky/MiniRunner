#include <stdlib.h>
#include "level.h"

Level level_create() {
    Level lvl;
    lvl.platformCount = 3;
    lvl.platforms = malloc(sizeof(Platform) * lvl.platformCount);

    lvl.platforms[0] = (Platform){200, 400, 150, 20};
    lvl.platforms[1] = (Platform){450, 300, 120, 20};
    lvl.platforms[2] = (Platform){650, 450, 180, 20};

    return lvl;
}

void level_destroy(Level *lvl) {
    free(lvl->platforms);
    lvl->platforms = NULL;
    lvl->platformCount = 0;
}