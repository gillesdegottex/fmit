#ifdef _WIN32

#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <iostream>

void initConsoleIfAttached() {
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
        _setmode(_fileno(stdout), _O_TEXT);
        _setmode(_fileno(stderr), _O_TEXT);
        setvbuf(stdout, NULL, _IONBF, 0);
        setvbuf(stderr, NULL, _IONBF, 0);
        std::cout.clear();
        std::cerr.clear();
    }
}

#else

void initConsoleIfAttached() {
}

#endif
