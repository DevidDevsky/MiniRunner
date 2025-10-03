#include "difficulty.h"
#include <math.h>

static float clampf(float v, float lo, float hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

void difficulty_compute_profile(int difficulty, DifficultyProfile* out) {
    int d = difficulty > 0 ? difficulty - 1 : 0;

    // Scaling
    out->speed_mult  = clampf(1.0f + 0.15f * d, 1.0f, 2.5f);
    float width_scale = clampf(1.0f - 0.06f * d, 0.55f, 1.0f);
    float gap_scale   = clampf(1.0f + 0.08f * d, 1.0f, 1.6f);

    out->gap_min_k = fminf(0.45f * gap_scale, 0.90f);
    out->gap_max_k = fminf(0.85f * gap_scale, 0.95f);
    out->step_up_k = 0.75f;
    out->step_down_px = 220.0f;

    // Platform sizes (scaled)
    out->float_h = 20.0f;
    out->float_w_min = 90.0f * width_scale;
    out->float_w_max = 200.0f * width_scale;
    float ground_scale = fmaxf(0.8f, width_scale);
    out->ground_w_min = 160.0f * ground_scale;
    out->ground_w_max = 300.0f * ground_scale;

    // Weights (difficulty-weighted)
    out->w_ground = (int)clampf(40 - 3 * d, 25, 100);
    out->w_far    = (int)clampf(30 + 2 * d,  0, 45);
    out->w_stairs = (int)clampf(10 + 1 * d,  0, 15);
    out->w_move   = (int)clampf(10 + 2 * d,  0, 20);
    int sum = out->w_ground + out->w_far + out->w_stairs + out->w_move;
    out->w_float = 100 - sum;
    if (out->w_float < 5) {
        out->w_float = 5;
        // reduce ground down to 25 min if needed
        int excess = (sum + out->w_float) - 100;
        if (excess > 0 && out->w_ground > 25) {
            int reduce = out->w_ground - 25;
            int take = reduce < excess ? reduce : excess;
            out->w_ground -= take;
            sum -= take;
        }
        out->w_float = 100 - (out->w_ground + out->w_far + out->w_stairs + out->w_move);
    }

    // Level length
    out->steps_base = 10;
    out->steps_per_level = 2;
    out->steps_cap = 30;

    // Moving platforms specifics
    out->moving_bonus_early = 1.25f; // wider on early levels
    out->moving_travel_k = 1.5f + 0.2f * d; // relative to width
    out->moving_speed_base = 2.5f; // multiplied by speed_mult
    out->moving_speed_cap = 6.0f;
    out->early_levels_threshold = 2;
}
