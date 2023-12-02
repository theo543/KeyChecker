#include <windows.h>
#include <climits>
#include <cstdlib>
#include <cstdio>

#ifdef DEBUG_DISPLAY_ALL_KEYCODES
    // don't let compiler know that this is always true
    volatile bool always_true = true;
    #define GetAsyncKeyState(x) (always_true ? true : GetAsyncKeyState(x))
#endif

const wchar_t STATIC_CLASS[] = TEXT("STATIC");
const wchar_t WINDOW_CLASS[] = TEXT("Key Checker");
WNDCLASSEX wc = {};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT wm, WPARAM wp, LPARAM lp);

const int WINDOW_WIDTH_INIT = 400;
const int WINDOW_HEIGHT_INIT = 200;

const char TXT_DEFAULT[] = "Press a key...";
const int VK_MAX = 0xFF;
const int TXT_MAX_SIZE = VK_MAX * 30 + 1;
const int VK_ASCII_LETTER_MIN = 0x41;
const int VK_ASCII_LETTER_MAX = 0x5A;
const int VK_ASCII_NUMBER_MIN = 0x30;
const int VK_ASCII_NUMBER_MAX = 0x39;

int nr_pressed = 0;
int pressed_indexes[TXT_MAX_SIZE] = {};
bool is_pressed[TXT_MAX_SIZE] = {};
char txt_current[TXT_MAX_SIZE] = {};
wchar_t txt_current_w[TXT_MAX_SIZE] = {};
char txt_tmp[TXT_MAX_SIZE] = {};

const char HEX_CHARS[] = "0123456789ABCDEF";
extern const char *keycode_names[];

void append_keycode(char **dest, int keycode) {
    if((keycode >= VK_ASCII_LETTER_MIN && keycode <= VK_ASCII_LETTER_MAX) || (keycode >= VK_ASCII_NUMBER_MIN && keycode <= VK_ASCII_NUMBER_MAX)) {
        sprintf(*dest, "%c ", static_cast<char>(keycode));
        (*dest) += 2;
    } else if (keycode_names[keycode] != nullptr) {
        sprintf(*dest, "%s ", keycode_names[keycode]);
        (*dest) += strlen(keycode_names[keycode]) + 1;
    } else {
        char hex_first_digit = HEX_CHARS[keycode >> 4], hex_second_digit = HEX_CHARS[keycode & 0xF];
        sprintf(*dest, "0x%c%c ", hex_first_digit, hex_second_digit);
        *dest += 5;
    }
}

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
            char *new_displayed = txt_tmp;

            memset(is_pressed, 0, sizeof(is_pressed));
            int new_nr_pressed = 0;
            for (int x = 0;x < nr_pressed;x++) {
                int code = pressed_indexes[x];
                if(GetAsyncKeyState(code)) {
                    append_keycode(&new_displayed, code);
                    pressed_indexes[new_nr_pressed++] = pressed_indexes[x];
                    is_pressed[code] = true;
                }
            }
            nr_pressed = new_nr_pressed;

            for(int x = 1;x < VK_MAX;x++) {
                if(is_pressed[x]) {
                    continue;
                }
                if(GetAsyncKeyState(x)) {
                    append_keycode(&new_displayed, x);
                    pressed_indexes[nr_pressed++] = x;
                }
            }

            if(new_displayed == txt_tmp) {
                memcpy(new_displayed, TXT_DEFAULT, sizeof(TXT_DEFAULT));
            } else {
                *(new_displayed - 1) = '\0';
            }

            if(strcmp(txt_current, txt_tmp) != 0) {
                memcpy(txt_current, txt_tmp, TXT_MAX_SIZE);
                mbstowcs(txt_current_w, txt_current, TXT_MAX_SIZE);
                SetWindowText(hwndStaticText, txt_current_w);
            }
            return 0;
        }

        default: {
            return DefWindowProc(hwnd, wm, wp, lp);
        }
    }
}
