#include <stdlib.h>
#include <time.h>
#include "level.h"
#include "config.h"

#define MIN_PLATFORMS 5
#define MAX_PLATFORM_WIDTH 200
#define MIN_PLATFORM_WIDTH 80
#define MAX_GAP_X 150
#define MAX_GAP_Y 150

Level level_create(int difficulty) {
    Level lvl;

    lvl.platformCount = MIN_PLATFORMS + difficulty * 2;
    lvl.platforms = malloc(sizeof(Platform) * lvl.platformCount);

    // Начальная платформа
    lvl.platforms[0] = (Platform){100, GROUND_Y - 50, 200, 20};
    lvl.startPlatform = 0;

    float lastX = lvl.platforms[0].x;
    float lastY = lvl.platforms[0].y;

    for (int i = 1; i < lvl.platformCount; i++) {
        float newX = lastX + MIN_PLATFORM_WIDTH + rand() % MAX_GAP_X;
        float newY = lastY + (rand() % (2 * MAX_GAP_Y)) - MAX_GAP_Y;

        if (newY > GROUND_Y - 50) newY = GROUND_Y - 50;
        if (newY < 100) newY = 100;

        float newWidth = MIN_PLATFORM_WIDTH + rand() % (MAX_PLATFORM_WIDTH - MIN_PLATFORM_WIDTH);

        lvl.platforms[i] = (Platform){newX, newY, newWidth, 20};

        lastX = newX;
        lastY = newY;
    }

    lvl.finishPlatform = lvl.platformCount - 1;

    return lvl;
}

void level_destroy(Level *lvl) {
    free(lvl->platforms);
    lvl->platforms = NULL;
    lvl->platformCount = 0;
}