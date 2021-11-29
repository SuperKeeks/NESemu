#include "App.h"
#include "LogUtils.h"

int main(int argc, char* args[])
{
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

    App app;
    app.Init(romFileName);
    while (!app.Update())
    {
    }
    app.End();

    return 0;
}
