#include "GhostUI.h"

GhostUI::GhostUI() : m_hwnd(NULL) {}

bool GhostUI::Initialize(HINSTANCE hInstance) {
    const char CLASS_NAME[] = "InvincibleOverlayClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = GhostUI::WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    // Create the window with TopMost and layered archictecture
    m_hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_LAYERED,
        CLASS_NAME, "AI Assistant",
        WS_POPUP, // Frameless for clean and ghost look,
        100, 100, 400, 200,
        NULL, NULL, hInstance, NULL

    );

    if (m_hwnd == NULL) {
        return false;
    }

    // --- THE STARTUP SECRET SAUCE ---
    // This makes the window disappear from screen shares and recordings

    SetWindowDisplayAffinity(m_hwnd, WDA_EXCLUDEFROMCAPTURE);

    // Set transparency (opacity 255 = solid, 0 = invisible)
    SetLayeredWindowAttributes(m_hwnd, 0, 255, LWA_ALPHA);
    return true;
}

LRESULT CALLBACK GhostUI::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            TextOut(hdc, 10, 10, "SYSTEM: INVICIBLE STATE ACTIVE", 31);
            EndPaint(hwnd, &ps);

        }
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void GhostUI::RunMessageLoop() {
    MSG msg = {};
    while(GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}