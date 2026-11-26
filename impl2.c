#include "contract.h"
#include <windows.h>

EXPORT int prime_count(int a, int b) {
    if (a > b) { int t = a; a = b; b = t; }
    if (a < 0) a = 0;
    if (b < 2) return 0;

    int size = b + 1;
    HANDLE heap = GetProcessHeap();
    unsigned char *sieve = (unsigned char *)HeapAlloc(heap, HEAP_ZERO_MEMORY, (SIZE_T)size);
    if (!sieve) return -1;

    for (int i = 2; i <= b; i++) sieve[i] = 1;

    for (int i = 2; (long long)i * i <= b; i++) {
        if (sieve[i]) {
            for (long long j = (long long)i * i; j <= b; j += i)
                sieve[(int)j] = 0;
        }
    }

    int count = 0;
    for (int i = a; i <= b; i++) count += sieve[i];

    HeapFree(heap, 0, sieve);
    return count;
}

EXPORT float area(float a, float b) {
    return 0.5f * a * b;
}