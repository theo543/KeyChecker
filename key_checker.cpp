#include <windows.h>
#include <cstdio>

const wchar_t STATIC_CLASS[] = TEXT("STATIC");
const wchar_t WINDOW_CLASS[] = TEXT("Key Checker");
WNDCLASS wc = {};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT wm, WPARAM wp, LPARAM lp);

const int WINDOW_WIDTH_INIT = 400;
const int WINDOW_HEIGHT_INIT = 200;

const int TXT_BUFFER_SIZE = 100;
wchar_t TXT_BUFFER[TXT_BUFFER_SIZE] = {};

HWND hwndStaticText = nullptr;

HINSTANCE hInst = nullptr;

const int TIMER_MS = 5; // 5 ms = 200 Hz

int main() {
    hInst = GetModuleHandle(nullptr);

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInst;
    wc.lpszClassName = WINDOW_CLASS;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
            0,
            WINDOW_CLASS,
            TEXT("Key Checker"),
            WS_OVERLAPPEDWINDOW,

            CW_USEDEFAULT,
            CW_USEDEFAULT,
            WINDOW_WIDTH_INIT,
            WINDOW_HEIGHT_INIT,

            nullptr,
            nullptr,
            hInst,
            nullptr
    );

    hwndStaticText = CreateWindowEx(
            0,
            STATIC_CLASS,
            nullptr,
        WS_CHILD | WS_VISIBLE,

            0,
            0,
            WINDOW_WIDTH_INIT,
            WINDOW_HEIGHT_INIT,

            hwnd,
            nullptr,
            hInst,

            nullptr
    );

    ShowWindow(hwnd, SW_SHOWNORMAL);

    SetTimer(hwnd, 0, TIMER_MS, nullptr);
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT wm, WPARAM wp, LPARAM lp) {
    switch (wm) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_TIMER:
            swprintf(TXT_BUFFER, TXT_BUFFER_SIZE, TEXT("Hello"));
            SetWindowText(hwndStaticText, TXT_BUFFER);

        default:
            return DefWindowProc(hwnd, wm, wp, lp);
    }
}
