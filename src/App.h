#pragma once

#include "NESemu.h"

#include <SDL.h>

#include <string>

class App
{
public:
    App();
    ~App() {}

    bool Init(const std::string& romFileName);
    bool Update();
    void End();

private:
    NESemu _emu;
    Input::ControllerState _controller1State;
    Input::ControllerState _controller2State;
    HardwareStateSnapshot _hardwareSnapShot;
    bool _invertAB = true; // Used for NES Mini controllers
    Uint32 _timeLast = 0;
    bool _enableOpcodeInfoPrinting = false;
    
    // SDL
    SDL_Window* _window = NULL;
    SDL_Renderer* _renderer = NULL;
    SDL_Texture* _texture = NULL;
    SDL_GameController* _gameControllers[2];

    static void SDLAudioCallback(void* userData, Uint8* audioData, int length);
    Input::ControllerState& SelectControllerState(SDL_Event event);
    void HandleKeyboardButtonEvent(SDL_Event event);
    void HandleGameControllerButtonEvent(SDL_Event event);
    void HandleGameControllerAxisEvent(SDL_Event event);
};
