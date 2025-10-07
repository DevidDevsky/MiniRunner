#ifndef CONFIG_H
#define CONFIG_H

// ---------- Окно ----------
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600
#define FULLSCREEN    0   // 1 = полный экран, 0 = окно

// ---------- Игрок ----------
#define GRAVITY      0.5f
#define JUMP_FORCE  -12.0f
#define PLAYER_SPEED 5.0f
#define PLAYER_SIZE  72

// ---------- Земля ----------
#define GROUND_HEIGHT 50
#define GROUND_Y (SCREEN_HEIGHT - GROUND_HEIGHT)

#endif