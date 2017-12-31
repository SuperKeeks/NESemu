#include "LogUtils.h"

#include <cstdarg>
#include <cstdio>
#include <string>
#include <Windows.h>

void Log::Debug(const char* fmt, ...)
{
#if _DEBUG
    va_list args;
    va_start(args, fmt);
    const int bufferSize = 1024;
    char buff[bufferSize];
    vsnprintf(buff, bufferSize, fmt, args);
    printf("\n");
    printf(buff);
    va_end(args);
#endif
}
