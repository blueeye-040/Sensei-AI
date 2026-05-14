#include "GhostUI.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    GhostUI app;
    if (!app.Initialize(hInstance)) {
        return -1;

    }

    app.RunMessageLoop();
    return 0;
}