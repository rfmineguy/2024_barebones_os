#include "ctype.h"

int toupper(int ch) {
    if (ch >= 'a' && ch <= 'z') return ch + 'A' - 'a';
    return ch;
}

int tolower(int ch) {
    if (ch >= 'A' && ch <= 'Z') return ch + 'a' - 'A';
    return ch;
}

int isprint(int ch) {
    return ch > 0x1f && ch < 0x7f;
}

int isdigit(int ch) {
    return ch >= '0' && ch <= '9';
}

int isalpha(int ch) {
    ch = tolower(ch);
    return (ch >= 'a' && ch <= 'z');
}
