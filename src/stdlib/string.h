#ifndef STRING_H
#define STRING_H

void* memcpy(void* dest, const void* src, int n);
int   memcmp(const void* ptr1, const void* ptr2, int n);

int   strcmp(const char* str1, const char* str2);
int   strlen(const char* str);

#endif
