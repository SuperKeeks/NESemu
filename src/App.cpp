#include "App.h"

#include "LogUtils.h"
#include "NESemu.h"
#include "SizeOfArray.h"

#include <SDL.h>

const int SCALE = 2;
const int JOYSTICK_DEAD_ZONE = 8000;
const int AUDIO_FREQUENCY = 44100;
const int AUDIO_BUFFER_SIZE = 1024;

App::App()
{
    for (int i = 0; i < sizeofarray(_gameControllers); ++i)
    {
        _gameControllers[i] = nullptr;
    }
}

bool App::Init(const std::string& romFileName)
{
    // Initialise emulator
    _romFileName = romFileName;
    _emu.Load((romFileName + ".nes").c_str());
    _emu.GetPPU()->SetWaitToShowFrameBuffer(true);
    _emu.GetAPU()->SetOutputFrequency(AUDIO_FREQUENCY);

    // Initialise app graphics
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) < 0)
    {
        Log::Error("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    _window = SDL_CreateWindow(
        "NESemu",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        PPU::kHorizontalResolution * SCALE,
        PPU::kVerticalResolution * SCALE,
        SDL_WINDOW_SHOWN);
    if (_window == NULL)
    {
        Log::Error("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    _renderer = SDL_CreateRenderer(_window, -1, 0);
    _texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, PPU::kHorizontalResolution, PPU::kVerticalResolution);

    // Initialise app game controllers
    // Download latest DB from https://raw.github.com/gabomdq/SDL_GameControllerDB/master/gamecontrollerdb.txt
    SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");
    const int gameControllerCount = SDL_NumJoysticks();
    for (int i = 0; i < gameControllerCount && i < sizeofarray(_gameControllers); i++)
    {
        _gameControllers[i] = SDL_GameControllerOpen(i);
        if (_gameControllers[i] == nullptr)
        {
            Log::Error("Can't open game controller %d: %s", i, SDL_GetError());
        }
    }

    // Initialise app audio
    SDL_AudioSpec audioSettings;
    SDL_zero(audioSettings);
    audioSettings.freq = AUDIO_FREQUENCY;
    audioSettings.format = AUDIO_S16;
    audioSettings.channels = 1;
    audioSettings.samples = AUDIO_BUFFER_SIZE;
    audioSettings.userdata = &(_emu.GetAPU()->GetBuffer());
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

    return true;
}

bool App::Update()
{
    bool quit = false;

    // Update input
    SDL_Event inputEvent;
    while (SDL_PollEvent(&inputEvent) != 0)
    {
        if (inputEvent.type == SDL_QUIT || (inputEvent.type == SDL_KEYUP && inputEvent.key.keysym.sym == SDLK_ESCAPE))
        {
            quit = true;
        }
        else if ((inputEvent.type == SDL_KEYUP && inputEvent.key.keysym.sym == SDLK_r))
        {
            _emu.Reset();
        }
        else if ((inputEvent.type == SDL_KEYUP && inputEvent.key.keysym.sym == SDLK_d))
        {
            _enableOpcodeInfoPrinting = !_enableOpcodeInfoPrinting;
            _emu.GetCPU()->EnableOpcodeInfoPrinting(_enableOpcodeInfoPrinting);
        }
        else if ((inputEvent.type == SDL_KEYUP && inputEvent.key.keysym.sym == SDLK_i))
        {
            _invertAB = !_invertAB;
            if (_invertAB)
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
            HandleKeyboardButtonEvent(inputEvent);
            if (inputEvent.type == SDL_KEYDOWN)
            {
                if (inputEvent.key.keysym.sym == SDLK_F6)
                {
                    SaveState();
                }
                else if (inputEvent.key.keysym.sym == SDLK_F9)
                {
                    TryLoadState();
                }
            }
        }
        else if (inputEvent.type == SDL_CONTROLLERBUTTONDOWN || inputEvent.type == SDL_CONTROLLERBUTTONUP)
        {
            HandleGameControllerButtonEvent(inputEvent);
        }
        else if (inputEvent.type == SDL_CONTROLLERAXISMOTION)
        {
            HandleGameControllerAxisEvent(inputEvent);
        }
    }

    _emu.SetControllerState(1, _controller1State);
    _emu.SetControllerState(2, _controller2State);

    // Update emu
    // Timing code from https://gamedev.stackexchange.com/questions/110825/how-to-calculate-delta-time-with-sdl
    const Uint32 timeNow = SDL_GetTicks();
    const double deltaTime = (float)(timeNow - _timeLast) / 1000;
    _emu.Update(deltaTime, SDL_LockAudio, SDL_UnlockAudio);
    _timeLast = timeNow;

    // Update app graphics
    if (_emu.GetPPU()->IsWaitingToShowFrameBuffer())
    {
        SDL_UpdateTexture(_texture, NULL, _emu.GetPPU()->GetFrameBuffer(), PPU::kHorizontalResolution * sizeof(Uint32));
        SDL_RenderClear(_renderer);
        SDL_RenderCopy(_renderer, _texture, NULL, NULL);
        SDL_RenderPresent(_renderer);
    }

    return quit;
}

void App::End()
{
    for (int i = 0; i < sizeofarray(_gameControllers); ++i)
    {
        if (_gameControllers[i] != nullptr)
        {
            SDL_GameControllerClose(_gameControllers[i]);
        }
    }

    SDL_CloseAudio();
    SDL_DestroyTexture(_texture);
    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
    SDL_Quit();
}

void App::SDLAudioCallback(void* userData, Uint8* audioData, int length)
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

Input::ControllerState& App::SelectControllerState(SDL_Event event)
{
    if (event.type == SDL_CONTROLLERBUTTONDOWN ||
        event.type == SDL_CONTROLLERBUTTONUP ||
        event.type == SDL_CONTROLLERAXISMOTION)
    {
        if (event.cdevice.which == 0)
        {
            return _controller1State;
        }
        else
        {
            return _controller2State;
        }
    }
    else
    {
        // The keyboard always maps to player 1
        return _controller1State;
    }
}

void App::HandleKeyboardButtonEvent(SDL_Event event)
{
    Input::ControllerState& controllerState = SelectControllerState(event);
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

void App::HandleGameControllerButtonEvent(SDL_Event event)
{
    Input::ControllerState& controllerState = SelectControllerState(event);
    
    bool keyState = false;
    if (event.type == SDL_CONTROLLERBUTTONDOWN)
    {
        keyState = true;
    }

    switch (event.cbutton.button)
    {
        case SDL_CONTROLLER_BUTTON_A:
            if (_invertAB)
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
            if (_invertAB)
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

void App::HandleGameControllerAxisEvent(SDL_Event event)
{
    Input::ControllerState& controllerState = SelectControllerState(event);

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

void App::SaveState()
{
    _hardwareSnapShot = _emu.GetSnapshot();

    FILE* file;
    fopen_s(&file, (_romFileName + ".save").c_str(), "wb");
    fwrite(&_hardwareSnapShot, sizeof(HardwareStateSnapshot), 1, file);
    fclose(file);

    Log::Info("Saved hardware state snapshot");
}

void App::TryLoadState()
{
    Log::Info("Trying to load savestate");

    bool error = true;
    FILE* file;
    fopen_s(&file, (_romFileName + ".save").c_str(), "rb");
    if (file != NULL)
    {
        const size_t elementsRead = fread(&_hardwareSnapShot, sizeof(HardwareStateSnapshot), 1, file);
        fclose(file);
        if (elementsRead == 1)
        {
            _emu.LoadSnapshot(_hardwareSnapShot);
            Log::Info("Successfully loaded savestate");
            error = false;
        }
    }

    if (error)
    {
        Log::Error("Can't load savestate for game %s, skipping", _romFileName.c_str());
    }
}
