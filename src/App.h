#pragma once

#include "NESemu.h"

#include <SDL.h>

#include <string>

class App
{
public:
    App() {}
    ~App() {}

    void Init(const std::string& romFileName);
    bool Update();
    void End();

private:
    NESemu _emu;
    Input::ControllerState _controller1State;
    Input::ControllerState _controller2State;
    HardwareStateSnapshot _hardwareSnapShot;
    bool _invertAB = true; // Used for NES Mini controllers
    Uint32 _last = 0;
    
    // SDL
    SDL_Window* _window = NULL;
    SDL_Renderer* _renderer = NULL;
    SDL_Texture* _texture = NULL;
    SDL_GameController* _gameControllers[2];

    static void SDLAudioCallback(void* userData, Uint8* audioData, int length);
    static Input::ControllerState& SelectControllerState(
        SDL_Event event,
        Input::ControllerState& controllerState1,
        Input::ControllerState& controllerState2);
    static void HandleKeyboardButtonEvent(Input::ControllerState& controllerState, SDL_Event event);
    static void HandleGameControllerButtonEvent(Input::ControllerState& controllerState, SDL_Event event, bool invertAB);
    static void HandleGameControllerAxisEvent(Input::ControllerState& controllerState, SDL_Event event);
};
