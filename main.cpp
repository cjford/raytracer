#include <SDL2/SDL.h>

#include "trace_ray.h"

const float SCREEN_WIDTH      = 640;
const float SCREEN_HEIGHT     = 640;
const float CANVAS_WIDTH      = SCREEN_WIDTH;
const float CANVAS_HEIGHT     = SCREEN_HEIGHT;
const float VIEW_WIDTH        = 1.0;
const float VIEW_HEIGHT       = 1.0;
const int   MAX_REFLECT_DEPTH = 3;
const char* WINDOW_TITLE      = "Raytracer";


void put_pixel(SDL_Renderer* renderer, float canvas_x, float canvas_y)  {
    float screen_x = (CANVAS_WIDTH / 2) + canvas_x;
    float screen_y = (CANVAS_HEIGHT / 2) - canvas_y;

    SDL_RenderDrawPoint(renderer, screen_x, screen_y);
}

int main(int argc, char* args[]) {
    SDL_Window* window = NULL;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL_Init failed. Error: %s\n", SDL_GetError());
        exit(-1);
    }

    window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if(window == NULL) {
        printf("SDL_CreateWindow failed. Error: %s\n", SDL_GetError() );
        exit(-1);
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    int last_ms = SDL_GetTicks();
    int curr_ms;
    int frame_ms;

    bool quit = false;
    SDL_Event e;

    while(!quit) {
        for (int x = -(CANVAS_WIDTH/2); x < (CANVAS_WIDTH / 2); x++) {
            for (int y = -(CANVAS_HEIGHT/2); y < (CANVAS_HEIGHT / 2); y++) {
                V3 ray = {
                    x * (VIEW_WIDTH / CANVAS_WIDTH),
                    y * (VIEW_HEIGHT / CANVAS_HEIGHT),
                    1
                };

                // For each canvas pixel, trace a ray from camera into the scene.
                // Calculate what color it should be and render it here.
                Color c = trace_ray({0, 0, 0}, ray, 0, 10000, MAX_REFLECT_DEPTH);
                SDL_SetRenderDrawColor(renderer, c.red, c.green, c.blue, 255);
                put_pixel(renderer, x, y);
            }
        }

        SDL_RenderPresent(renderer);

        while(SDL_PollEvent(&e) != 0) {
            if(e.type == SDL_QUIT || e.type == SDL_MOUSEBUTTONDOWN) {
                quit = true;
            }

        }

        curr_ms = SDL_GetTicks();
        frame_ms = curr_ms - last_ms;
        last_ms = curr_ms;

        printf("Rendered frame in %i ms\n", frame_ms);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
