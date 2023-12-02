#include <cstring>
#include <cstdio>

const int VK_MAX = 0xFF;
const int VK_ASCII_LETTER_MIN = 0x41;
const int VK_ASCII_LETTER_MAX = 0x5A;
const int VK_ASCII_NUMBER_MIN = 0x30;
const int VK_ASCII_NUMBER_MAX = 0x39;
extern const char *keycode_names[];

int main() {
    size_t total = 0;
    for(int x = 0;x < VK_MAX;x++) {
        if((x >= VK_ASCII_LETTER_MIN && x <= VK_ASCII_LETTER_MAX) || (x >= VK_ASCII_NUMBER_MIN && x <= VK_ASCII_NUMBER_MAX)) {
            printf("%c ", (char)x);
            // 1 char + 1 space
            total += 2;
        } else if (keycode_names[x] != nullptr) {
            // strlen + 1 space
            printf("%s ", keycode_names[x]);
            total += strlen(keycode_names[x]) + 1;
        } else {
            printf("0x%X ", x);
            // 0x + 2 hex digits + 1 space
            total += 5;
        }
    }
    printf("\n%zu\n", total);
}
