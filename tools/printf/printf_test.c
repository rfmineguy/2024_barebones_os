#include "printf.h"
#include <printf.h>
#include "string_view.h"
#include <stdarg.h>

// int string_view_test(const char* fmt, ...){
//     string_view s = sv_from_cstr(fmt);
//     string_view split[20] = {0};
//     int x = sv_split_by(s, "{}, ", split, 20);
// 
//     for (int i = 0; i < x; i++) {
//         printf("%d: %d, %p, %.*s\n", i,
//                 split[i].len, split[i].s,
//                 split[i].len, split[i].s);
//     }
// 
//     return 0;
// }

void normal_printf_example() {
    int x = 0;
    printf("%*.c|%9c|%*.*s|%7d|%p\n", 2, 'a', 'p', 5, 3, "adkfhsjd", 443431, &x);
    k_printf2("%*.c|%9c|%*.*s|%7d|%5x|%p\n", 2, 'a', 'p', 5, 3, "adkfhsjd", 443431, 24, &x);
}

int main() {
    normal_printf_example();

    // fmt_spec s = {0};
    // fmt_spec_parse("%-015s", &s);
    // debug_fmt_spec(s);
    // fmt_spec_parse("%+3c", &s);
    // debug_fmt_spec(s);

    // string_view_test("{l=3,d=4,t=casf}{t=s,l=10}{l=4,d=3}");
}
