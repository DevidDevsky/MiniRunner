#ifndef CAMERA_H
#define CAMERA_H

typedef struct {
    float x, y;
} Camera;

void camera_update(Camera *cam, float playerX, float playerY);

#endif
