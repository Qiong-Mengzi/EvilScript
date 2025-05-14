// #include "chaos_a.h"
#include "../src/chaos_a.c"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
char arr[16777216];
#define sect_size 4096
const unsigned sect_n = sizeof(arr) / sect_size;

int main(void)
{
    init_chaosA(arr, sect_n, sect_size);
    void *m = arr;

    LARGE_INTEGER ts, te, tq, f;
    QueryPerformanceFrequency(&f);

    QueryPerformanceCounter(&ts);
    for (unsigned long long i = 0; i < 100000000; ++i)
    {
        void *p = chaos_allocA(arr, (i * 18) % 16777216);
        chaos_freeA(arr, p);
    }
    QueryPerformanceCounter(&te);

    printf("%.12lf sec (Chaos@evilrt)\n", (double)(te.QuadPart - ts.QuadPart) / (double)f.QuadPart);

    return 0;
}
