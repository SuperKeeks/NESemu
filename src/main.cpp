#include "NESemu.h"

#include <SDL.h>
#include <stdio.h>

const int SCALE = 2;

void HandleKeyboardInput(Input::ControllerState& controllerState, SDL_Event event);

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
            emu.Load("demo4.nes");
            emu.GetPPU()->SetWaitToShowFrameBuffer(true);

            // Timing code from https://gamedev.stackexchange.com/questions/110825/how-to-calculate-delta-time-with-sdl
            Uint32 now = SDL_GetTicks();
            Uint32 last = 0;
            double deltaTime = 0; // In seconds
            Input::ControllerState controller1State;

            while (!quit)
            {
                last = now;
                now = SDL_GetTicks();
                deltaTime = (float)(now - last) / 1000;

                while (SDL_PollEvent(&e) != 0)
                {
                    if (e.type == SDL_QUIT || (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE))
                    {
                        quit = true;
                    }
                    else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
                    {
                        HandleKeyboardInput(controller1State, e);
                    }
                }

                emu.SetControllerState(1, controller1State);
                emu.Update(deltaTime);
                
                if (emu.GetPPU()->IsWaitingToShowFrameBuffer())
                {
                    SDL_UpdateTexture(texture, NULL, emu.GetPPU()->GetFrameBuffer(), PPU::kHorizontalResolution * sizeof(Uint32));
                    SDL_RenderClear(renderer);
                    SDL_RenderCopy(renderer, texture, NULL, NULL);
                    SDL_RenderPresent(renderer);
                }
            }
        }
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void HandleKeyboardInput(Input::ControllerState& controllerState, SDL_Event event)
{
    bool keyState = false;
    if (event.type == SDL_KEYDOWN)
    {
        keyState = true;
    }

    switch (event.key.keysym.sym)
    {
        case SDLK_LEFT:
            controllerState.Left = keyState;
            break;
        case SDLK_RIGHT:
            controllerState.Right = keyState;
            break;
        case SDLK_UP:
            controllerState.Up = keyState;
            break;
        case SDLK_DOWN:
            controllerState.Down = keyState;
            break;
        case SDLK_z:
            controllerState.B = keyState;
            break;
        case SDLK_x:
            controllerState.A = keyState;
            break;
        case SDLK_n:
            controllerState.Select = keyState;
            break;
        case SDLK_m:
            controllerState.Start = keyState;
            break;
    }
}
