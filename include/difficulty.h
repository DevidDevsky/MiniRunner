#ifndef DIFFICULTY_H
#define DIFFICULTY_H

typedef struct {
    // Scaling
    float speed_mult;      // multiplies base moving platform speed
    float width_scale;     // multiplies platform widths (smaller as difficulty grows)
    float gap_min_k;       // fraction of max jump distance
    float gap_max_k;       // fraction of max jump distance (<= 0.95)
    float step_up_k;       // fraction of max jump height
    float step_down_px;    // absolute pixels allowed per step down

    // Platform sizes (already scaled)
    float float_h;
    float float_w_min;
    float float_w_max;
    float ground_w_min;
    float ground_w_max;

    // Category weights (sum <= 100, remainder is float)
    int w_ground; // ground segments
    int w_far;    // far jump pattern (with intermediate)
    int w_stairs; // stair pattern
    int w_move;   // moving carrier
    int w_float;  // computed remainder

    // Level length
    int steps_base;
    int steps_per_level;
    int steps_cap;

    // Moving platforms specifics
    float moving_bonus_early;   // width multiplier for early levels
    float moving_travel_k;      // relative travel vs width/gap
    float moving_speed_base;    // base pixels per tick (~ per 16ms)
    float moving_speed_cap;     // maximum speed
    int   early_levels_threshold; // <= this difficulty treated as early
} DifficultyProfile;

void difficulty_compute_profile(int difficulty, DifficultyProfile* out);

#endif
