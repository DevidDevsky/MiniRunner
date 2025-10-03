#include "physics.h"

void platforms_update_all(Platform *platforms, int count, float dt) {
    for (int i = 0; i < count; i++) {
        Platform *p = &platforms[i];
        if (p->type == PLATFORM_MOVING) {
            p->x += p->vx * dt;
            if (p->x < p->rangeL) {
                p->x = p->rangeL;
                p->vx = -p->vx;
            }
            if (p->x + p->w > p->rangeR) {
                p->x = p->rangeR - p->w;
                p->vx = -p->vx;
            }
        }
    }
}
