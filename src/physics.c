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
        // crumble timers
        if (p->type == PLATFORM_CRUMBLE && p->crumbles && p->crumbleTimer > 0.0f) {
            p->crumbleTimer -= dt / 60.0f; // dt ~ frames, convert to seconds
            if (p->crumbleTimer <= 0.0f) {
                // collapse: hide by setting height to 0
                p->h = 0;
            }
        }
    }
}
