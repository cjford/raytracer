#include "color.h"

SDL_Color Color::to_sdl() {
    return {
        clamp(red, 0, 255),
        clamp(green, 0, 255),
        clamp(blue, 0, 255),
        255
    };
}

Color Color::blend(Color c, float blend_factor) {
    return {
        red * (1 - blend_factor) + c.red * blend_factor,
        green * (1 - blend_factor) + c.green * blend_factor,
        blue * (1 - blend_factor) + c.blue * blend_factor
    };
}

unsigned char Color::clamp(float x, float min_x, float max_x) {
    if (x > max_x) {
        return max_x;
    } else if (x < min_x) {
        return min_x;
    } else {
        return x;
    };
}
