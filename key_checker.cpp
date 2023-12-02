#include <windows.h>
#include <climits>
#include <cstdlib>

const wchar_t STATIC_CLASS[] = TEXT("STATIC");
const wchar_t WINDOW_CLASS[] = TEXT("Key Checker");
WNDCLASSEX wc = {};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT wm, WPARAM wp, LPARAM lp);

const int WINDOW_WIDTH_INIT = 400;
const int WINDOW_HEIGHT_INIT = 200;

const int TXT_BUFFER_SIZE = CHAR_MAX;
char txtBuffer[TXT_BUFFER_SIZE] = {};
wchar_t txtBufferW[TXT_BUFFER_SIZE] = {};
const wchar_t TXT_DEFAULT[] = TEXT("Press a key...");

HWND hwndStaticText = nullptr;

HINSTANCE hInst = nullptr;

const int TIMER_MS = 5; // 5 ms = 200 Hz

int main() {
    hInst = GetModuleHandle(nullptr);

    wc.cbSize = sizeof(wc);
    wc.style = 0;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInst;
    wc.hIcon = nullptr;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = WINDOW_CLASS;
    wc.hIconSm = nullptr;

    RegisterClassEx(&wc);

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
    // no resize
    SetWindowLongPtrA(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX);

    SetTimer(hwnd, 0, TIMER_MS, nullptr);
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT wm, WPARAM wp, LPARAM lp) {
    switch (wm) {
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }

        case WM_TIMER: {
            bool already_checked[TXT_BUFFER_SIZE] = {};
            wchar_t txtNewBufferW[TXT_BUFFER_SIZE] = {};

            // remove no longer pressed letters in-place, keeping order of pressed ones
            int new_string_size = 0;
            for (char c : txtBuffer) {
                if (c == '\0') {
                    break;
                }
                if (GetAsyncKeyState(c)) {
                    already_checked[c] = true;
                    txtBuffer[new_string_size++] = c;
                }
            }

            for(int x = 1;x < TXT_BUFFER_SIZE;x++) {
                if(already_checked[x]) {
                    continue;
                }
                char c = static_cast<char>(x);
                if(GetAsyncKeyState(c)) {
                    txtBuffer[new_string_size++] = c;
                }
            }

            if(new_string_size == 0) {
                wcscpy(txtNewBufferW, TXT_DEFAULT);
            } else {
                txtBuffer[new_string_size++] = '\0';
                mbstowcs(txtNewBufferW, txtBuffer, TXT_BUFFER_SIZE);
            }

            if(wcscmp(txtNewBufferW, txtBufferW) != 0) {
                wcscpy(txtBufferW, txtNewBufferW);
                SetWindowText(hwndStaticText, txtBufferW);
            }
            return 0;
        }

        default: {
            return DefWindowProc(hwnd, wm, wp, lp);
        }
    }
}
