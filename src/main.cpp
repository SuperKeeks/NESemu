#include "LogUtils.h"
#include "NESemu.h"
#include "SizeOfArray.h"

#include <SDL.h>
#include <stdio.h>

const int SCALE = 2;
const int JOYSTICK_DEAD_ZONE = 8000;
const int AUDIO_FREQUENCY = 44100;
const int AUDIO_BUFFER_SIZE = 1024;

Input::ControllerState& SelectControllerState(SDL_Event event, Input::ControllerState& controllerState1, Input::ControllerState& controllerState2);
void HandleKeyboardButtonEvent(Input::ControllerState& controllerState, SDL_Event event);
void HandleGameControllerButtonEvent(Input::ControllerState& controllerState, SDL_Event event, bool invertAB);
void HandleGameControllerAxisEvent(Input::ControllerState& controllerState, SDL_Event event);
void SDLAudioCallback(void* userData, Uint8* audioData, int length);

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
    bool invertAB = true; // Used for NES Mini controllers

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) < 0)
    {
        Log::Error("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }
    else
    {
        // Download latest DB from https://raw.github.com/gabomdq/SDL_GameControllerDB/master/gamecontrollerdb.txt
        SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");

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
            const int gameControllerCount = SDL_NumJoysticks();
            Log::Info("%d joysticks were found.\n", gameControllerCount);
            for (int i = 0; i < gameControllerCount && i < sizeofarray(gameControllers); i++)
            {
                gameControllers[i] = SDL_GameControllerOpen(i);
                if (gameControllers[i] == nullptr)
                {
                    Log::Error("Can't open game controller %d: %s", i, SDL_GetError());
                }
            }

            // Rendering initialisation
            renderer = SDL_CreateRenderer(window, -1, 0);
            texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, PPU::kHorizontalResolution, PPU::kVerticalResolution);

            NESemu emu;
            //emu.Load("arkanoid.nes");
            //emu.Load("color_test.nes");
            //emu.Load("donkey.nes");
            //emu.Load("galaga.nes");
            //emu.Load("mariobros.nes");
            //emu.Load("popeye.nes");
            //emu.Load("smb.nes");
            emu.Load("smb3.nes");
            //emu.Load("dmc.nes");
            //emu.Load("gb.nes");
            //emu.Load("zelda_title.nes");
            
            emu.GetPPU()->SetWaitToShowFrameBuffer(true);
            emu.GetAPU()->SetOutputFrequency(AUDIO_FREQUENCY);

            // Audio initialisation
            SDL_AudioSpec audioSettings;
            SDL_zero(audioSettings);
            audioSettings.freq = AUDIO_FREQUENCY;
            audioSettings.format = AUDIO_S16;
            audioSettings.channels = 1;
            audioSettings.samples = AUDIO_BUFFER_SIZE;
            audioSettings.userdata = &(emu.GetAPU()->GetBuffer());
            audioSettings.callback = &SDLAudioCallback;
            SDL_OpenAudio(&audioSettings, 0);
            if (audioSettings.format != AUDIO_S16)
            {
                Log::Error("Got an unexpected audio format");
                SDL_CloseAudio();
            }
            else
            {
                Log::Info("Initialised an Audio device at frequency %d Hz, %d Channels\n", audioSettings.freq, audioSettings.channels);
                SDL_PauseAudio(0);
            }

            // Timing code from https://gamedev.stackexchange.com/questions/110825/how-to-calculate-delta-time-with-sdl
            Uint32 now = SDL_GetTicks();
            Uint32 last = 0;
            double deltaTime = 0; // In seconds
            Input::ControllerState controller1State;
            Input::ControllerState controller2State;
            bool enableOpcodeInfoPrinting = false;

            bool quit = false;
            SDL_Event inputEvent;
            while (!quit)
            {
                last = now;
                now = SDL_GetTicks();
                deltaTime = (float)(now - last) / 1000;

                while (SDL_PollEvent(&inputEvent) != 0)
                {
                    if (inputEvent.type == SDL_QUIT || (inputEvent.type == SDL_KEYUP && inputEvent.key.keysym.sym == SDLK_ESCAPE))
                    {
                        quit = true;
                    }
                    else if ((inputEvent.type == SDL_KEYUP && inputEvent.key.keysym.sym == SDLK_r))
                    {
                        emu.Reset();
                    }
                    else if ((inputEvent.type == SDL_KEYUP && inputEvent.key.keysym.sym == SDLK_d))
                    {
                        enableOpcodeInfoPrinting = !enableOpcodeInfoPrinting;
                        emu.GetCPU()->EnableOpcodeInfoPrinting(enableOpcodeInfoPrinting);
                    }
                    else if ((inputEvent.type == SDL_KEYUP && inputEvent.key.keysym.sym == SDLK_i))
                    {
                        invertAB = !invertAB;
                        if (invertAB)
                        {
                            Log::Info("Inverted A and B buttons mappings");
                        }
                        else
                        {
                            Log::Info("Restored A and B buttons mappings");
                        }
                    }
                    else if (inputEvent.type == SDL_KEYDOWN || inputEvent.type == SDL_KEYUP)
                    {
                        HandleKeyboardButtonEvent(SelectControllerState(inputEvent, controller1State, controller2State), inputEvent);
                    }
                    else if (inputEvent.type == SDL_CONTROLLERBUTTONDOWN || inputEvent.type == SDL_CONTROLLERBUTTONUP)
                    {
                        HandleGameControllerButtonEvent(SelectControllerState(inputEvent, controller1State, controller2State), inputEvent, invertAB);
                    }
                    else if (inputEvent.type == SDL_CONTROLLERAXISMOTION)
                    {
                        HandleGameControllerAxisEvent(SelectControllerState(inputEvent, controller1State, controller2State), inputEvent);
                    }
                }

                emu.SetControllerState(1, controller1State);
                emu.SetControllerState(2, controller2State);

                emu.Update(deltaTime, SDL_LockAudio, SDL_UnlockAudio);
                
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

    SDL_CloseAudio();
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

Input::ControllerState& SelectControllerState(
    SDL_Event event,
    Input::ControllerState& controllerState1, 
    Input::ControllerState& controllerState2)
{
    if (event.type == SDL_CONTROLLERBUTTONDOWN || 
        event.type == SDL_CONTROLLERBUTTONUP || 
        event.type == SDL_CONTROLLERAXISMOTION)
    {
        if (event.cdevice.which == 0)
        {
            return controllerState1;
        }
        else
        {
            return controllerState2;
        }
    }
    else
    {
        // The keyboard always maps to player 1
        return controllerState1;
    }
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

void HandleGameControllerButtonEvent(Input::ControllerState& controllerState, SDL_Event event, bool invertAB)
{
    bool keyState = false;
    if (event.type == SDL_CONTROLLERBUTTONDOWN)
    {
        keyState = true;
    }

    switch (event.cbutton.button)
    {
        case SDL_CONTROLLER_BUTTON_A:
            if (invertAB)
            {
                controllerState.B = keyState;
            }
            else
            {
                controllerState.A = keyState;
            }
            break;
        case SDL_CONTROLLER_BUTTON_B:
        case SDL_CONTROLLER_BUTTON_X:
            if (invertAB)
            {
                controllerState.A = keyState;
            }
            else
            {
                controllerState.B = keyState;
            }
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

void SDLAudioCallback(void* userData, Uint8* audioData, int length)
{
    int16_t* output = reinterpret_cast<int16_t*>(audioData);
    const int samplesToWrite = length / sizeof(int16_t);
    APU::OutputBuffer& emuBuffer = *((APU::OutputBuffer*)userData);
    const int emuBufferLength = (int)emuBuffer.GetLength();
    for (int i = 0; i < samplesToWrite && i < emuBufferLength; ++i)
    {
        const double sample = emuBuffer.Read();
        OMBAssert(sample >= 0 && sample <= 1.0, "Wrong output value");
        output[i] = (int16_t)(sample * std::numeric_limits<int16_t>::max());
    }

    // If the emulator buffer is not long enough, fill the SDL one with the last sample
    for (int i = 0; i < samplesToWrite - emuBufferLength; ++i)
    {
        output[i] = output[emuBufferLength - 1];
    }
}
