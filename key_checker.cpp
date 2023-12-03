#include <windows.h>
#include <climits>
#include <cstdlib>
#include <cstdio>

#ifdef DEBUG_DISPLAY_ALL_KEYCODES
    // don't let compiler know that this is always true
    volatile bool always_true = true;
    #define GetAsyncKeyState(x) (always_true ? true : GetAsyncKeyState(x))
#endif

const wchar_t EDIT_CONTROL_CLASS[] = TEXT("EDIT");
const wchar_t WINDOW_CLASS[] = TEXT("Key Checker");
const wchar_t BUTTON_CLASS[] = TEXT("BUTTON");
HMENU BUTTON_ID = HMENU(1);
WNDCLASSEX wc = {};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT wm, WPARAM wp, LPARAM lp);

const int WINDOW_WIDTH_INIT = 400;
const int WINDOW_HEIGHT_INIT = 200;
const int MARGIN = 10;
const int COPY_BUTTON_HEIGHT = 30;
const int COPY_BUTTON_WIDTH = 200;

const wchar_t COPY_BUTTON_TEXT[] = TEXT("Copy to clipboard");
const wchar_t COPY_BUTTON_ERROR[] = TEXT("Couldn't copy to clipboard");
const char TXT_DEFAULT[] = "Press a key...";
const int VK_MAX = 0xFF;
const int TXT_MAX_SIZE = 2119 + 1; // see total_keycode_name_size.cpp for calculation
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

HWND hwndWindow = nullptr;
HWND hwndText = nullptr;
HWND hwndButton = nullptr;

HINSTANCE hInst = nullptr;

const int TIMER_MS = 5; // 5 ms = 200 Hz

void autosize(const int WINDOW_WIDTH, const int WINDOW_HEIGHT) {
    const int TXT_WIDTH = WINDOW_WIDTH - 2 * MARGIN;
    const int TXT_HEIGHT = WINDOW_HEIGHT - 2 * MARGIN - COPY_BUTTON_HEIGHT - 2 * MARGIN;
    const int COPY_BUTTON_X = MARGIN;
    const int COPY_BUTTON_Y = WINDOW_HEIGHT - MARGIN - COPY_BUTTON_HEIGHT;
    SetWindowPos(
            hwndText,
            nullptr,

            MARGIN,
            MARGIN,
            TXT_WIDTH,
            TXT_HEIGHT,

            SWP_NOZORDER | SWP_NOCOPYBITS
    );

    SetWindowPos(
            hwndButton,
            nullptr,

            COPY_BUTTON_X,
            COPY_BUTTON_Y,
            COPY_BUTTON_WIDTH,
            COPY_BUTTON_HEIGHT,

            SWP_NOZORDER | SWP_NOCOPYBITS
    );
}

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
    wc.hbrBackground = CreateSolidBrush(RGB(240, 240, 240));
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = WINDOW_CLASS;
    wc.hIconSm = nullptr;

    RegisterClassEx(&wc);

    hwndWindow = CreateWindowEx(
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

    hwndText = CreateWindowEx(
            0,
            EDIT_CONTROL_CLASS,
            nullptr,
            WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_READONLY | ES_MULTILINE | ES_LEFT,

            MARGIN,
            MARGIN,
            0,
            0,

            hwndWindow,
            nullptr,
            hInst,

            nullptr
    );

    hwndButton = CreateWindowEx(
            0,
            BUTTON_CLASS,
            COPY_BUTTON_TEXT,
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,

            0,
            0,
            COPY_BUTTON_WIDTH,
            COPY_BUTTON_HEIGHT,

            hwndWindow,
            BUTTON_ID,
            hInst,

            nullptr
    );

    autosize(WINDOW_WIDTH_INIT, WINDOW_HEIGHT_INIT);

    ShowWindow(hwndWindow, SW_SHOWNORMAL);

    SetTimer(hwndWindow, 0, TIMER_MS, nullptr);
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
                SetWindowText(hwndText, txt_current_w);
            }
            return 0;
        }

        case WM_SIZE: {
            int width = LOWORD(lp), height = HIWORD(lp);
            autosize(width, height);
            return 0;
        }

        case WM_COMMAND: {
            if(reinterpret_cast<HMENU>(wp) == BUTTON_ID) {
                if(OpenClipboard(hwndWindow)) {
                    EmptyClipboard();
                    HGLOBAL txtGlobalCopy = GlobalAlloc(GMEM_MOVEABLE, sizeof(txt_current_w));
                    if (txtGlobalCopy != nullptr) {
                        memcpy(GlobalLock(txtGlobalCopy), txt_current_w, sizeof(txt_current_w));
                        GlobalUnlock(txtGlobalCopy);
                        SetClipboardData(CF_UNICODETEXT, txtGlobalCopy);
                        SetWindowText(hwndButton, COPY_BUTTON_TEXT);
                    } else {
                        SetWindowText(hwndButton, COPY_BUTTON_ERROR);
                    }
                    CloseClipboard();
                } else {
                    SetWindowText(hwndButton, COPY_BUTTON_ERROR);
                }
                return 0;
            }
            return 1;
        }

        default: {
            return DefWindowProc(hwnd, wm, wp, lp);
        }
    }
}
