#ifndef COLOR_H
#define COLOR_H

#include <SDL2/SDL.h>

class Color {
    public:

    float red;
    float green;
    float blue;

    Color blend(Color c, float blend_factor);

    SDL_Color to_sdl();

    private:

    unsigned char clamp(float x, float min_x, float max_x);
};

#endif
