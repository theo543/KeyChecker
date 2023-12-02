#include <windows.h>
#include <stdio.h>

const wchar_t STATIC_CLASS[] = TEXT("STATIC");
const wchar_t WINDOW_CLASS[] = TEXT("Key Checker");
WNDCLASS wc = {};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT wm, WPARAM wp, LPARAM lp);

const int WINDOW_WIDTH_INIT = 400;
const int WINDOW_HEIGHT_INIT = 200;

wchar_t TXT_BUFFER[100] = {};
const int TXT_BUFFER_SIZE = sizeof(TXT_BUFFER) / sizeof(TXT_BUFFER[0]);

HWND hwndStaticText = NULL;

HINSTANCE hInst = NULL;

const int TIMER_MS = 5; // 5 ms = 200 Hz

int main() {
    hInst = GetModuleHandle(NULL);

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

            NULL,
            NULL,
            hInst,
            NULL
    );

    hwndStaticText = CreateWindowEx(
            0,
            STATIC_CLASS,
            NULL,
        WS_CHILD | WS_VISIBLE,

            0,
            0,
            WINDOW_WIDTH_INIT,
            WINDOW_HEIGHT_INIT,

            hwnd,
            NULL,
            hInst,

            NULL
    );

    ShowWindow(hwnd, SW_SHOWNORMAL);

    SetTimer(hwnd, 0, TIMER_MS, NULL);
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
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
