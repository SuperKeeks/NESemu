#include "LogUtils.h"
#include "NESemu.h"
#include "SizeOfArray.h"

#include <SDL.h>
#include <stdio.h>

const int SCALE = 2;
const int JOYSTICK_DEAD_ZONE = 8000;

void HandleKeyboardButtonEvent(Input::ControllerState& controllerState, SDL_Event event);
void HandleGameControllerButtonEvent(Input::ControllerState& controllerState, SDL_Event event);
void HandleGameControllerAxisEvent(Input::ControllerState& controllerState, SDL_Event event);

int main(int argc, char* args[])
{
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Texture* texture = NULL;
    SDL_GameController* gameControllers[2];
    for (int i = 0; i < sizeofarray(gameControllers); ++i)
    {
        gameControllers[i] = nullptr;
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0)
    {
        Log::Error("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
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
            Log::Error("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        }
        else
        {
            // Initialise game controllers
            const int numJoysticks = SDL_NumJoysticks();
            Log::Info("%d joysticks were found.\n", numJoysticks);
            for (int i = 0; i < numJoysticks && i < sizeofarray(gameControllers); i++)
            {
                gameControllers[i] = SDL_GameControllerOpen(i);
            }

            renderer = SDL_CreateRenderer(window, -1, 0);
            texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, PPU::kHorizontalResolution, PPU::kVerticalResolution);

            bool quit = false;
            SDL_Event e;

            NESemu emu;
            //emu.Load("color_test.nes");
            //emu.Load("demo1.nes");
            emu.Load("smb.nes");
            //emu.Load("mariobros.nes");
            //emu.Load("zelda_title.nes");
            //emu.Load("popeye.nes");
            //emu.Load("galaga.nes");
            //emu.Load("pyramid.nes");
            //emu.Load("donkey.nes");
            //emu.Load("tetris.nes");
            emu.GetPPU()->SetWaitToShowFrameBuffer(true);

            // Timing code from https://gamedev.stackexchange.com/questions/110825/how-to-calculate-delta-time-with-sdl
            Uint32 now = SDL_GetTicks();
            Uint32 last = 0;
            double deltaTime = 0; // In seconds
            Input::ControllerState controller1State;
            Input::ControllerState controller2State;
            bool enableOpcodeInfoPrinting = false;

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
                    else if ((e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_r))
                    {
                        emu.Reset();
                    }
                    else if ((e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_d))
                    {
                        enableOpcodeInfoPrinting = !enableOpcodeInfoPrinting;
                        emu.GetCPU()->EnableOpcodeInfoPrinting(enableOpcodeInfoPrinting);
                    }
                    else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
                    {
                        HandleKeyboardButtonEvent(controller1State, e);
                    }
                    else if (e.type == SDL_CONTROLLERBUTTONDOWN || e.type == SDL_CONTROLLERBUTTONUP)
                    {
                        HandleGameControllerButtonEvent(controller2State, e);
                    }
                    else if (e.type == SDL_CONTROLLERAXISMOTION)
                    {
                        HandleGameControllerAxisEvent(controller2State, e);
                    }
                }

                emu.SetControllerState(1, controller1State);
                emu.SetControllerState(2, controller2State);
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

    for (int i = 0; i < sizeofarray(gameControllers); ++i)
    {
        if (gameControllers[i] != nullptr)
        {
            SDL_GameControllerClose(gameControllers[i]);
        }
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void HandleKeyboardButtonEvent(Input::ControllerState& controllerState, SDL_Event event)
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
        case SDLK_RSHIFT:
            controllerState.Select = keyState;
            break;
        case SDLK_RETURN:
            controllerState.Start = keyState;
            break;
    }
}

void HandleGameControllerButtonEvent(Input::ControllerState& controllerState, SDL_Event event)
{
    bool keyState = false;
    if (event.type == SDL_CONTROLLERBUTTONDOWN)
    {
        keyState = true;
    }

    switch (event.cbutton.button)
    {
        case SDL_CONTROLLER_BUTTON_A:
            controllerState.A = keyState;
            break;
        case SDL_CONTROLLER_BUTTON_X:
            controllerState.B = keyState;
            break;
        case SDL_CONTROLLER_BUTTON_BACK:
            controllerState.Select = keyState;
            break;
        case SDL_CONTROLLER_BUTTON_START:
            controllerState.Start = keyState;
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            controllerState.Up = keyState;
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            controllerState.Down = keyState;
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            controllerState.Left = keyState;
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            controllerState.Right = keyState;
            break;
    }
}

void HandleGameControllerAxisEvent(Input::ControllerState& controllerState, SDL_Event event)
{
    if (event.jaxis.axis == 0)
    {
        if (event.jaxis.value < -JOYSTICK_DEAD_ZONE)
        {
            controllerState.Left = true;
            controllerState.Right = false;
        }
        else if (event.jaxis.value > JOYSTICK_DEAD_ZONE)
        {
            controllerState.Left = false;
            controllerState.Right = true;
        }
        else
        {
            controllerState.Left = false;
            controllerState.Right = false;
        }
    }
    else if (event.jaxis.axis == 1)
    {
        if (event.jaxis.value < -JOYSTICK_DEAD_ZONE)
        {
            controllerState.Up = false;
            controllerState.Down = true;
        }
        else if (event.jaxis.value > JOYSTICK_DEAD_ZONE)
        {
            controllerState.Up = true;
            controllerState.Down = false;
        }
        else
        {
            controllerState.Up = false;
            controllerState.Down = false;
        }
    }
}
