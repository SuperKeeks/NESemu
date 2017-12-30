#include "NESemu.h"

#include <SDL.h>
#include <stdio.h>

const int SCALE = 2;

int main(int argc, char* args[])
{
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Texture* texture = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }
    else
    {
        window = SDL_CreateWindow(
            "NESemu",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            PPU::kHorizontalResolution * SCALE,
            PPU::kVerticalResolution * SCALE,
            SDL_WINDOW_SHOWN);
        if (window == NULL)
        {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        }
        else
        {
            renderer = SDL_CreateRenderer(window, -1, 0);
            texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, PPU::kHorizontalResolution, PPU::kVerticalResolution);

            bool quit = false;
            SDL_Event e;

            NESemu emu;
            emu.Load("demo1.nes");

            // Timing code from https://gamedev.stackexchange.com/questions/110825/how-to-calculate-delta-time-with-sdl
            Uint64 now = SDL_GetPerformanceCounter();
            Uint64 last = 0;
            double deltaTime = 0; // In seconds

            while (!quit)
            {
                last = now;
                now = SDL_GetPerformanceCounter();
                deltaTime = ((now - last) * 1000/(double)SDL_GetPerformanceFrequency()) * 0.001;

                while (SDL_PollEvent(&e) != 0)
                {
                    if (e.type == SDL_QUIT)
                    {
                        quit = true;
                    }
                }

                emu.Update(deltaTime);
                
                SDL_UpdateTexture(texture, NULL, emu.GetPPU()->GetFrameBuffer(), PPU::kHorizontalResolution * sizeof(Uint32));
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, texture, NULL, NULL);
                SDL_RenderPresent(renderer);
            }
        }
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}