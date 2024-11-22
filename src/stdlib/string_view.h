#ifndef STRING_VIEW_H
#define STRING_VIEW_H

// heavily inspired by
//    https://github.com/tsoding/sv/blob/master/sv.h

typedef struct string_view {
    const char *s;
    int len;
} string_view;

#define SV_NULL (string_view) {.s=(void*)0, .len=0};

string_view sv_from_parts(const char* begin, int len);
string_view sv_from_cstr(const char* begin);
string_view sv_take_until(string_view, char);
int         sv_split_by(string_view, const char*, string_view*, int);
#endif
