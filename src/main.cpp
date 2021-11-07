#include "LogUtils.h"
#include "NESemu.h"
#include "SizeOfArray.h"

#include <d3d11.h>

#include <SDL.h>
#include <SDL_syswm.h>

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_dx11.h>

#include <stdio.h>

static ID3D11Device*  g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

const int SCALE = 2;
const int JOYSTICK_DEAD_ZONE = 8000;
const int AUDIO_FREQUENCY = 44100;
const int AUDIO_BUFFER_SIZE = 1024;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();

void InitImGui(SDL_Window* window);

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
            SDL_SysWMinfo wmInfo;
            SDL_VERSION(&wmInfo.version);
            SDL_GetWindowWMInfo(window, &wmInfo);
            HWND hwnd = (HWND)wmInfo.info.win.window;
            // Initialize Direct3D
            if (!CreateDeviceD3D(hwnd))
            {
                CleanupDeviceD3D();
                return 1;
            }

            // Initialise game controllers
            const int gameControllerCount = SDL_NumJoysticks();
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

            // Check if ROM name has been passed as an argument
            if (argc == 1)
            {
                Log::Error("No ROM name specified. Usage: NESemu <ROM name> (e.g. \"NESemu smb\")");
                return 0;
            }

            // Check if ROM actually exists
            std::string romFileName(args[1]);
            romFileName.append(".nes");
            FILE* file;
            fopen_s(&file, romFileName.c_str(), "rb");
            if (file == NULL)
            {
                Log::Error("ROM %s not found!", romFileName.c_str());
                return 0;
            }                
            fclose(file);

            NESemu emu;
            emu.Load(romFileName.c_str());
            
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

            InitImGui(window);

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

                // Start the Dear ImGui frame
                ImGui_ImplDX11_NewFrame();
                ImGui_ImplSDL2_NewFrame();
                ImGui::NewFrame();

                // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
                //bool show_demo_window = true;
                //ImGui::ShowDemoWindow(&show_demo_window);

                ImGui::Begin("Hello, world!");
                ImGui::Text("This is some useful text.");
                ImGui::End();

                // Rendering
                ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
                ImGui::Render();
                /*const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
                g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
                g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
                ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
                g_pSwapChain->Present(1, 0); // Present with vsync*/
                
                if (emu.GetPPU()->IsWaitingToShowFrameBuffer())
                {
                    SDL_UpdateTexture(texture, NULL, emu.GetPPU()->GetFrameBuffer(), PPU::kHorizontalResolution * sizeof(Uint32));
                    SDL_RenderClear(renderer);
                    SDL_RenderCopy(renderer, texture, NULL, NULL);

                    const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
                    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
                    g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
                    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
                    g_pSwapChain->Present(1, 0); // Present with vsync

                    //SDL_RenderPresent(renderer);
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
    CleanupDeviceD3D();
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

void InitImGui(SDL_Window* window)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForD3D(window);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
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
        output[i] = (int16_t)(sample * 0x7FFF /*std::numeric_limits<int16_t>::max()*/);
    }

    // If the emulator buffer is not long enough, fill the SDL one with the last sample
    for (int i = 0; i < samplesToWrite - emuBufferLength; ++i)
    {
        output[i] = output[emuBufferLength - 1];
    }
}
