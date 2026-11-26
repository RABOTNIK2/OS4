#include "contract.h"

static int is_prime_naive(int n) {
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (int i = 3; (long long)i * i <= n; i += 2)
        if (n % i == 0) return 0;
    return 1;
}

EXPORT int prime_count(int a, int b) {
    if (a > b) { int t = a; a = b; b = t; }
    if (a < 0) a = 0;
    int count = 0;
    for (int n = a; n <= b; n++)
        if (is_prime_naive(n)) count++;
    return count;
}

EXPORT float area(float a, float b) {
    return a * b;
}