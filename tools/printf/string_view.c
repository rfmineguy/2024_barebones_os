#include "string_view.h"
#include <string.h>
#include <printf.h>

string_view sv_from_parts(const char* begin, int len){
    return (string_view) {.s=begin, .len=len};
}
string_view sv_from_cstr(const char* begin){
    return (string_view) {.s=begin, .len=strlen(begin)};
}
string_view sv_take_until(string_view sv, char c) {
    for (int i = 0; i < sv.len; i++) {
        if (sv.s[i] == c)
            return (string_view) {.s = sv.s + i, sv.len - i};
    }
    return SV_NULL;
}
int sv_split_by(string_view sv, const char* c, string_view* out, int out_len) {
    int delim_len = strlen(c);
    int counter = 0;
    const char* token_begin = c;
    for (int i = 0; i < sv.len; i++) {
        if (sv.s[i] == ' ') continue;
        if (counter >= out_len) return counter;
        for (int j = 0; j < delim_len; j++) {
            if (sv.s[i] == c[j]) {
                out[counter++] = sv_from_parts(token_begin + 1, (sv.s + i) - token_begin - 1);
                token_begin += (sv.s + i) - token_begin;
            }
        }
    }
    return counter;
}
