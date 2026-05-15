// GhostUI.cpp — Manages the control window (Start/Stop) and the invisible overlay box.

#include "GhostUI.h"

// Global pointer to the one GhostUI instance.
// Window procedures (ControlProc, OverlayProc) must be static C-style functions,
// so they can't access 'this'. This global lets them reach the instance.
static GhostUI* g_app = nullptr;

// ─────────────────────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────────────────────
GhostUI::GhostUI()
    : m_hwndControl(NULL),  // NULL = no window handle yet
      m_hwndOverlay(NULL)
{
    g_app = this;  // Register this instance so static callbacks can access it
}

// ─────────────────────────────────────────────────────────────
// Initialize — creates both windows
// ─────────────────────────────────────────────────────────────
bool GhostUI::Initialize(HINSTANCE hInstance) {

    // ── 1. Register the CONTROL window class ─────────────────
    // WNDCLASS tells Windows the properties of a type of window before we create one.
    // We must register a class first, then create windows of that class.

    WNDCLASS wcControl      = {};   // Zero-initialize every field
    wcControl.lpfnWndProc   = GhostUI::ControlProc;          // Function that handles events for this window
    wcControl.hInstance     = hInstance;                      // Which program owns this class
    wcControl.lpszClassName = "ControlClass";                 // Name we invented to identify this class
    wcControl.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);   // Background color: standard Windows grey
    wcControl.hCursor       = LoadCursor(NULL, IDC_ARROW);    // Mouse cursor: standard arrow

    RegisterClass(&wcControl);  // Hand the class definition to Windows

    // ── 2. Register the OVERLAY window class ─────────────────
    WNDCLASS wcOverlay      = {};
    wcOverlay.lpfnWndProc   = GhostUI::OverlayProc;
    wcOverlay.hInstance     = hInstance;
    wcOverlay.lpszClassName = "OverlayClass";
    wcOverlay.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);  // Black background for HUD look
    wcOverlay.hCursor       = LoadCursor(NULL, IDC_SIZEALL);        // Move cursor — we allow dragging

    RegisterClass(&wcOverlay);

    // ── 3. Create the CONTROL window ─────────────────────────
    // CreateWindowEx creates an actual window using the class we registered.
    //
    //   dwExStyle   (1st param) = Extended style flags
    //   lpClassName             = Must match what we registered above
    //   lpWindowName            = Title bar text
    //   dwStyle                 = Style flags controlling window shape/behavior
    //   X, Y                    = Position on screen (pixels from top-left corner)
    //   nWidth, nHeight         = Size in pixels
    //   hWndParent              = NULL = this is a top-level window (no parent)
    //   hMenu                   = NULL = no menu bar
    //   hInstance               = our program handle
    //   lpParam                 = NULL = no extra creation data

    m_hwndControl = CreateWindowEx(
        0,                                           // No extended styles
        "ControlClass",                              // Class we registered
        "Invincible Assistant",                      // Title bar text
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,    // Style:
        //   WS_OVERLAPPED = basic window shape
        //   WS_CAPTION    = shows a title bar
        //   WS_SYSMENU    = shows the X (close) button
        //   (No WS_THICKFRAME — prevents the user from resizing)
        500, 280,    // X, Y: where the window appears on screen
        300, 170,    // Width, Height
        NULL, NULL, hInstance, NULL
    );

    if (m_hwndControl == NULL) return false;  // CreateWindowEx failed (very rare)

    // ── 4. Add START and STOP buttons inside the control window ──
    // Buttons are child windows using the built-in "BUTTON" class.
    //
    //   WS_CHILD      = this window lives inside a parent window
    //   WS_VISIBLE    = show it immediately (children aren't visible by default)
    //   BS_PUSHBUTTON = standard clickable button look
    //
    // The hMenu parameter is reused for child windows to hold a numeric ID.
    // That ID appears in WM_COMMAND so we know which button was clicked.

    CreateWindowEx(
        0, "BUTTON", "START",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        40, 65,         // X, Y inside the parent window (relative to parent)
        90, 35,         // Width, Height
        m_hwndControl,          // Parent window
        (HMENU)ID_BTN_START,    // ID = 101, used in WM_COMMAND to identify this button
        hInstance, NULL
    );

    CreateWindowEx(
        0, "BUTTON", "STOP",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        160, 65,        // Positioned to the right of START
        90, 35,
        m_hwndControl,
        (HMENU)ID_BTN_STOP,     // ID = 102
        hInstance, NULL
    );

    // ── 5. Create the OVERLAY box (hidden until Start is pressed) ──

    m_hwndOverlay = CreateWindowEx(
        WS_EX_TOPMOST    |  // Always appears above all other windows
        WS_EX_LAYERED    |  // Required to use SetLayeredWindowAttributes (transparency)
        WS_EX_TOOLWINDOW |  // Hides this window from the Windows taskbar
        WS_EX_NOACTIVATE,   // Clicking the box does NOT steal keyboard focus from your meeting app
        "OverlayClass",
        "Overlay",
        WS_POPUP,           // WS_POPUP = no title bar, no border — a plain frameless box
        100, 100,           // Starting position on screen
        440, 120,           // Width, Height of the box
        NULL, NULL, hInstance, NULL
    );

    if (m_hwndOverlay == NULL) return false;

    // ── 6. Apply screen-share invisibility to the overlay ────
    //
    // SetWindowDisplayAffinity controls how the OS shares this window's pixels.
    //
    //   WDA_EXCLUDEFROMCAPTURE: the OS display driver removes this window from:
    //     - Screen recordings (OBS, Xbox Game Bar, QuickTime)
    //     - Screen shares (Zoom, Teams, Google Meet, WebEx)
    //     - Screenshots (PrintScreen, Snipping Tool, Win+Shift+S)
    //   The window is still fully visible on YOUR physical monitor.
    //
    //   Requires Windows 10 version 2004 (May 2020 update) or later.

    SetWindowDisplayAffinity(m_hwndOverlay, WDA_EXCLUDEFROMCAPTURE);

    // SetLayeredWindowAttributes sets overall window transparency.
    //   Param 2 (0)     = color key — not used here
    //   Param 3 (225)   = alpha: 0 = fully invisible, 255 = fully solid
    //   Param 4 (LWA_ALPHA) = flag: use the alpha value, ignore the color key
    SetLayeredWindowAttributes(m_hwndOverlay, 0, 225, LWA_ALPHA);

    // Overlay stays HIDDEN here. ShowWindow is called only when Start is clicked.

    // ── 7. Show the control window ───────────────────────────
    ShowWindow(m_hwndControl, SW_SHOW);   // SW_SHOW = make the window visible
    UpdateWindow(m_hwndControl);          // Force an immediate WM_PAINT so it draws right away

    return true;
}

// ─────────────────────────────────────────────────────────────
// ControlProc — handles events for the Start/Stop control window
// ─────────────────────────────────────────────────────────────
// Windows calls this every time something happens to the control window:
// a button click, a paint request, the user closing it, etc.
//
//   hwnd   = which window this message is for
//   uMsg   = event type (WM_COMMAND, WM_PAINT, WM_DESTROY, ...)
//   wParam = extra data (meaning changes per message type)
//   lParam = more extra data (meaning changes per message type)

LRESULT CALLBACK GhostUI::ControlProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {

        case WM_COMMAND:
            // WM_COMMAND fires when a button (child window) is clicked.
            // LOWORD(wParam) extracts the lower 16 bits of wParam — that holds the button's ID.
            if (LOWORD(wParam) == ID_BTN_START) {
                // Show the overlay box
                // SW_SHOW makes a hidden window visible without changing its size or position
                ShowWindow(g_app->m_hwndOverlay, SW_SHOW);
                UpdateWindow(g_app->m_hwndOverlay);  // Paint it immediately
            }
            else if (LOWORD(wParam) == ID_BTN_STOP) {
                // Hide the overlay box
                // SW_HIDE makes the window invisible but keeps it in memory
                ShowWindow(g_app->m_hwndOverlay, SW_HIDE);
            }
            return 0;

        case WM_PAINT: {
            // WM_PAINT fires when the window (or part of it) needs to be redrawn.
            PAINTSTRUCT ps;
            // BeginPaint prepares the window for drawing.
            // Returns an HDC (Handle to Device Context) — the "canvas" we draw on.
            HDC hdc = BeginPaint(hwnd, &ps);

            SetBkMode(hdc, TRANSPARENT);      // Text background is transparent (no white box behind letters)
            SetTextColor(hdc, RGB(60, 60, 60)); // Dark grey text color

            // DrawText draws a string inside a rectangle. -1 = auto-calculate string length.
            // DT_CENTER | DT_VCENTER | DT_SINGLELINE = centered text
            RECT rcText = {20, 18, 280, 55};
            DrawText(hdc, "Press START to show the invisible overlay.", -1, &rcText, DT_LEFT | DT_WORDBREAK);

            EndPaint(hwnd, &ps);  // Done drawing — releases the HDC
            return 0;
        }

        case WM_DESTROY:
            // WM_DESTROY fires when the window is being closed (user clicked X).
            // PostQuitMessage puts WM_QUIT into the message queue.
            // GetMessage() in RunMessageLoop sees WM_QUIT and returns FALSE, ending the loop.
            // Param 0 = exit code (0 = normal exit)
            PostQuitMessage(0);
            return 0;
    }

    // DefWindowProc handles any messages we didn't process:
    // resizing, minimizing, mouse movement defaults, etc.
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// ─────────────────────────────────────────────────────────────
// OverlayProc — handles events for the invisible overlay box
// ─────────────────────────────────────────────────────────────
LRESULT CALLBACK GhostUI::OverlayProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {

        case WM_LBUTTONDOWN:
            // WM_LBUTTONDOWN fires when the user clicks the left mouse button on this window.
            // We use it to let the user drag and reposition the frameless box.

            // ReleaseCapture: stops the window from holding onto mouse input.
            // Must be called before the SendMessage below, otherwise the drag gets stuck.
            ReleaseCapture();

            // WM_NCLBUTTONDOWN tells Windows to start a drag as if the user clicked
            // the title bar (NC = Non-Client area).
            // HTCAPTION = the "caption / title bar" hit-test code.
            // This tricks Windows into dragging the window even though WS_POPUP has no title bar.
            SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // GetClientRect fills a RECT with the drawable area of the window
            // (excludes any border/title bar — WS_POPUP has none, so it's the full size)
            RECT rc;
            GetClientRect(hwnd, &rc);

            // Fill the entire window with solid black
            // GetStockObject(BLACK_BRUSH) returns a pre-made black brush (no need to create/delete)
            FillRect(hdc, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));

            // TRANSPARENT background mode: when drawing text, the pixels behind each
            // character stay as-is (our black fill), instead of being overwritten with white
            SetBkMode(hdc, TRANSPARENT);

            // ── Title line — bright green ──────────────────────────────
            SetTextColor(hdc, RGB(0, 255, 80));  // RGB: red=0, green=255, blue=80 → vivid green

            // CreateFont creates a custom font for drawing.
            // Params in order:
            //   nHeight        = 20  — character height in pixels
            //   nWidth         = 0   — 0 = auto width based on height
            //   nEscapement    = 0   — text angle in tenths of a degree (0 = horizontal)
            //   nOrientation   = 0   — character angle (0 = normal)
            //   fnWeight       = FW_BOLD — font weight (bold)
            //   fdwItalic      = FALSE
            //   fdwUnderline   = FALSE
            //   fdwStrikeOut   = FALSE
            //   fdwCharSet     = DEFAULT_CHARSET — use system default character set
            //   fdwOutputPrec  = OUT_DEFAULT_PRECIS — default output precision
            //   fdwClipPrec    = CLIP_DEFAULT_PRECIS — default clip precision
            //   fdwQuality     = CLEARTYPE_QUALITY — smoothest rendering (sub-pixel antialiasing)
            //   fdwPitchAndFamily = DEFAULT_PITCH
            //   lpszFace       = "Consolas" — monospace font, clean HUD look
            HFONT hFontBig = CreateFont(
                20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH, "Consolas"
            );

            // SelectObject swaps our font into the drawing context.
            // Returns the previously selected font — we save it to restore later.
            HFONT hOldFont = (HFONT)SelectObject(hdc, hFontBig);

            // TextOut draws text at pixel position (x=12, y=12).
            // Last param is character count (must match the string exactly).
            TextOut(hdc, 12, 12, "[ AI ASSISTANT - INVISIBLE MODE ACTIVE ]", 40);

            // ── Hint line — grey ──────────────────────────────────────
            SetTextColor(hdc, RGB(160, 160, 160));  // Grey

            HFONT hFontSmall = CreateFont(
                15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH, "Consolas"
            );
            SelectObject(hdc, hFontSmall);

            TextOut(hdc, 12, 44, "Drag to move.   This box is invisible to screen share.", 53);
            TextOut(hdc, 12, 68, "Click STOP in the control panel to hide it.", 43);

            // Restore original font, then delete the ones we created.
            // GDI objects (fonts, brushes, pens) must be deleted manually to avoid memory leaks.
            SelectObject(hdc, hOldFont);
            DeleteObject(hFontBig);
            DeleteObject(hFontSmall);

            EndPaint(hwnd, &ps);
            return 0;
        }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// ─────────────────────────────────────────────────────────────
// RunMessageLoop — keeps the app alive
// ─────────────────────────────────────────────────────────────
void GhostUI::RunMessageLoop() {
    MSG msg = {};

    // GetMessage: blocks (waits) until a message is ready, then fills the MSG struct.
    // Returns FALSE only when WM_QUIT arrives (posted by PostQuitMessage in WM_DESTROY).
    //   &msg  = where to store the message
    //   NULL  = receive messages from ALL windows of this thread
    //   0, 0  = no filter — receive all message types
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);  // Converts raw key events into WM_CHAR character messages
        DispatchMessage(&msg);   // Routes the message to the correct WindowProc (ControlProc or OverlayProc)
    }
}
