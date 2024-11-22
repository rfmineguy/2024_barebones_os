#include "printf.h"
#include <printf.h>
#include "string_view.h"
#include <stdarg.h>

int string_view_test(const char* fmt, ...){
    string_view s = sv_from_cstr(fmt);
    string_view split[20] = {0};
    int x = sv_split_by(s, "{}, ", split, 20);

    for (int i = 0; i < x; i++) {
        printf("%d: %d, %p, %.*s\n", i,
                split[i].len, split[i].s,
                split[i].len, split[i].s);
    }

    return 0;
}

int main() {
    string_view_test("{l=3,d=4,t=casf}{t=s,l=10}{l=4,d=3}");
}
