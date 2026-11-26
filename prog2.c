#include <windows.h>

typedef int  (*prime_fn)(int, int);
typedef float (*area_fn)(float, float);

static HANDLE hIn, hOut;

static int read_line(char *buf, DWORD cap) {
    DWORD read = 0;
    if (!ReadFile(hIn, buf, cap - 1, &read, NULL)) return -1;
    buf[read] = '\0';
    while (read > 0 && (buf[read-1] == '\n' || buf[read-1] == '\r'))
        buf[--read] = '\0';
    return (int)read;
}

static void print(const char *s) {
    DWORD w; WriteFile(hOut, s, (DWORD)lstrlenA(s), &w, NULL);
}

static void print_int(int v) {
    char tmp[32]; int i = 0, neg = 0;
    if (v < 0) { neg = 1; v = -v; }
    if (v == 0) tmp[i++] = '0';
    while (v > 0) { tmp[i++] = (char)('0' + v % 10); v /= 10; }
    if (neg) tmp[i++] = '-';
    char out[33]; int j = 0;
    while (i > 0) out[j++] = tmp[--i];
    out[j] = '\0'; print(out);
}

static int tokenize(char *s, char **tokens, int max_tokens) {
    int n = 0; char *p = s;
    while (*p && n < max_tokens) {
        while (*p == ' ' || *p == '\t') p++;
        if (!*p) break;
        tokens[n++] = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) { *p = '\0'; p++; }
    }
    return n;
}
static int parse_int(const char *s, int *out) {
    if (!s || !*s) return 0;
    int sign = 1, i = 0;
    if (s[0] == '-') { sign = -1; i = 1; }
    else if (s[0] == '+') i = 1;
    if (!s[i]) return 0;
    int v = 0;
    for (; s[i]; i++) {
        if (s[i] < '0' || s[i] > '9') return 0;
        v = v * 10 + (s[i] - '0');
    }
    *out = v * sign; return 1;
}
static int parse_float(const char *s, float *out) {
    if (!s || !*s) return 0;
    int sign = 1, i = 0;
    if (s[0] == '-') { sign = -1; i = 1; }
    else if (s[0] == '+') i = 1;
    double v = 0.0; int digits = 0;
    while (s[i] >= '0' && s[i] <= '9') { v = v * 10 + (s[i] - '0'); i++; digits++; }
    if (s[i] == '.') {
        i++; double frac = 0.1;
        while (s[i] >= '0' && s[i] <= '9') { v += (s[i]-'0')*frac; frac *= 0.1; i++; digits++; }
    }
    if (digits == 0 || s[i] != '\0') return 0;
    *out = (float)(sign * v); return 1;
}

static HMODULE g_lib = NULL;
static prime_fn g_prime = NULL;
static area_fn g_area  = NULL;
static int g_impl  = 1; 

static int load_impl(int which) {
    if (g_lib) { FreeLibrary(g_lib); g_lib = NULL; g_prime = NULL; g_area = NULL; }

    const char *path = (which == 1) ? ".\\impl1.dll" : ".\\impl2.dll";
    HMODULE h = LoadLibraryA(path);
    if (!h) {
        print("LoadLibrary failed: ");
        print(path);
        print("\r\n");
        return 0;
    }
    prime_fn pf = (prime_fn)GetProcAddress(h, "prime_count");
    area_fn  af = (area_fn)GetProcAddress(h, "area");
    if (!pf || !af) {
        print("GetProcAddress failed\r\n");
        FreeLibrary(h);
        return 0;
    }
    g_lib = h; g_prime = pf; g_area = af; g_impl = which;
    print("loaded impl");
    print_int(which);
    print("\r\n");
    return 1;
}

int main(void) {
    hIn  = GetStdHandle(STD_INPUT_HANDLE);
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    if (!load_impl(1)) return 1;

    char line[512];
    for (;;) {
        print("> ");
        int len = read_line(line, sizeof(line));
        if (len < 0 || len == 0) break;

        char *tok[16];
        int n = tokenize(line, tok, 16);
        if (n == 0) continue;

        if (tok[0][0] == '0' && tok[0][1] == '\0') {
            int next = (g_impl == 1) ? 2 : 1;
            load_impl(next);
        } else if (tok[0][0] == '1' && tok[0][1] == '\0') {
            int a, b;
            if (n >= 3 && parse_int(tok[1], &a) && parse_int(tok[2], &b)) {
                int r = g_prime(a, b);
                print("prime_count = ");
                print_int(r);
                print("\r\n");
            } else print("usage: 1 <a> <b>\r\n");
        } else if (tok[0][0] == '2' && tok[0][1] == '\0') {
            float a, b;
            if (n >= 3 && parse_float(tok[1], &a) && parse_float(tok[2], &b)) {
                float r = g_area(a, b);
                int ip = (int)r;
                int frac = (int)((r - ip) * 100000 + 0.5f);
                if (frac < 0) frac = -frac;
                print("area = ");
                print_int(ip);
                print(".");
                char fb[8]; int k = 0, tmp = frac;
                do { fb[k++] = (char)('0' + tmp % 10); tmp /= 10; } while (tmp > 0 && k < 5);
                while (k < 5) fb[k++] = '0';
                char out[8]; int j = 0;
                while (k > 0) out[j++] = fb[--k];
                out[j] = '\0';
                print(out);
                print("\r\n");
            } else print("usage: 2 <a> <b>\r\n");
        } else {
            print("unknown command\r\n");
        }
    }

    if (g_lib) FreeLibrary(g_lib);
    return 0;
}