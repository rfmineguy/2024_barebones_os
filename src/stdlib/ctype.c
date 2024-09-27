#include "ctype.h"

int toupper(int ch) {
    if (ch >= 'a' && ch <= 'z') return ch + 'A' - 'a';
    return ch;
}
