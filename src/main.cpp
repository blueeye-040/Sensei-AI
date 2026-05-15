// main.cpp — Entry point. The OS calls WinMain when the user opens the .exe.

#include "GhostUI.h"  // Our class that manages both windows

// WinMain: Windows GUI equivalent of main().
//
//   HINSTANCE hInstance     — Handle (numeric ID) for THIS running copy of the program.
//                             Windows uses it to track resources this process owns.
//
//   HINSTANCE hPrevInstance — Always NULL in modern Windows. Leftover from Windows 3.x.
//                             Safe to ignore.
//
//   LPSTR lpCmdLine         — Command-line arguments as a raw string. Not used here.
//
//   int nCmdShow            — Hint from Windows on how to show the first window
//                             (normal, minimized, maximized). We handle this ourselves.
//
//   WINAPI                  — Calling convention required by Windows for this function.
//                             Tells the compiler how arguments are passed on the stack.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    GhostUI app;  // Create our app object (no windows yet, just internal setup)

    // Initialize creates:
    //   1. The control window (Start / Stop buttons) — visible immediately
    //   2. The overlay box — hidden until the user clicks Start
    if (!app.Initialize(hInstance)) {
        // MB_OK | MB_ICONERROR: show a popup with an OK button and an error icon
        MessageBox(NULL, "Failed to create window.", "Error", MB_OK | MB_ICONERROR);
        return -1;  // Non-zero tells the OS something went wrong
    }

    // RunMessageLoop keeps the program alive.
    // It processes every event (clicks, paints, close) until the window is closed.
    app.RunMessageLoop();

    return 0;  // 0 = clean exit, tells the OS everything went fine
}
