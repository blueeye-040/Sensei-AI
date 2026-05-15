#ifndef GHOSTUI_H   // Include guard: prevents this file from being included twice
#define GHOSTUI_H   // If already included, skip everything below

// windows.h: the main Windows API header.
// Pulls in everything we need: CreateWindow, ShowWindow, HWND, HINSTANCE, etc.
#include <windows.h>

// Button IDs — numeric labels we assign to each button.
// Windows uses these in WM_COMMAND messages to tell us WHICH button was clicked.
// The actual numbers (101, 102) are arbitrary; they just need to be unique.
#define ID_BTN_START  101   // Identifies the "START" button
#define ID_BTN_STOP   102   // Identifies the "STOP"  button

class GhostUI {
public:
    GhostUI();                          // Constructor: sets up internal state
    bool Initialize(HINSTANCE hInstance); // Creates the control window + overlay box
    void RunMessageLoop();               // Keeps the app alive, processing events

private:
    HWND m_hwndControl;  // Handle to the control window (Start/Stop UI)
    HWND m_hwndOverlay;  // Handle to the invisible overlay box

    // Window procedures MUST be static — Windows doesn't know about C++ objects,
    // so it calls these as plain C-style function pointers.
    // CALLBACK = the calling convention Windows requires for these functions.
    // LRESULT  = the return type Windows expects (a long integer result code).
    static LRESULT CALLBACK ControlProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK OverlayProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif // GHOSTUI_H
