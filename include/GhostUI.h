#ifndef GhostUI_h
#define GhostUI_h

// Only include windows.h when compiling on a Windows system
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

class GhostUI {
    public:
        GhostUI();
        bool Initialize(HINSTANCE hInstance);
        void RunMessageLoop();

    private:
        HWND m_hwnd;
        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam);
};

#endif