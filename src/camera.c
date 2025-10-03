#include "camera.h"
#include "config.h"

void camera_update(Camera *cam, float playerX, float playerY) {
    cam->x = playerX - SCREEN_WIDTH / 2;
    cam->y = 0; // Камера не двигается по Y

    // Ограничиваем камеру, чтобы она не выходила за пределы уровня (если нужно)
    if (cam->x < 0) {
        cam->x = 0;
    }
}
