/*
 * This is not a real benchmark, just some very early attempt at comparing
 * various re2clib algorithms and re2.
 */

#include "config.h"
#include "lib/regex.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef HAVE_RE2_RE2_H
#include "re2/re2.h"
#endif

static inline void show(const char *prefix1, const char *prefix2
    , clock_t t1, clock_t t2, bool ok)
{
    fprintf(stderr, "%25s%10s:%10ld ticks, %lfs\n"
        , prefix1, prefix2, t2 - t1, double(t2 - t1) / CLOCKS_PER_SEC);

    if (!ok) {
        fprintf(stderr, "*** %s %s failed\n", prefix1, prefix2);
        exit(1);
    }
}

static void bench_re2c(const char *regexp, const char *string, size_t ntimes
    , int flags, const char *prefix)
{
    regex_t re;
    int result;
    clock_t t1, t2;

    t1 = clock();
    result = regcomp(&re, regexp, flags);
    t2 = clock();
    show(prefix, "compile", t1, t2, result == 0);

    const size_t nmatch = re.re_nsub;
    regmatch_t *pmatch = new regmatch_t[nmatch];

    t1 = clock();
    for (size_t i = 0; i < ntimes; ++i) {
        result = regexec(&re, string, nmatch, pmatch, 0);
    }
    t2 = clock();
    show("", "run", t1, t2, result == 0);

    delete[] pmatch;
    regfree(&re);
}

#ifdef HAVE_RE2_RE2_H
static void bench_re2(const char *regexp, const char *string, size_t ntimes
    , const char *prefix)
{
    RE2 *re2;
    clock_t t1, t2;
    bool err;

    t1 = clock();
    re2 = new RE2(regexp, RE2::POSIX);
    t2 = clock();
    show(prefix, "compile", t1, t2, re2->ok());

    const int argc = re2->NumberOfCapturingGroups();
    RE2::Arg *args = new RE2::Arg[argc];
    RE2::Arg **argps = new RE2::Arg*[argc];
    std::string *results = new std::string[argc];
    for (int i = 0; i < argc; ++i) {
        args[i] = &results[i];
        argps[i] = &args[i];
    }

    t1 = clock();
    for (size_t i = 0; i < ntimes; ++i) {
        err = RE2::FullMatchN(string, *re2, argps, argc);
    }
    t2 = clock();
    show("", "run", t1, t2, err);

    delete[] results;
    delete[] argps;
    delete[] args;
    delete re2;
}
#endif

static void bench(const char *r, const char *s, size_t n, bool notrie)
{
    fprintf(stderr, "\nr: %.*s..., s: %.*s..., n: %lu\n", 30, r, 30, s, n);

    bench_re2c(r, s, n, 0,                      "re2c-tdfa");
    bench_re2c(r, s, n, REG_NFA,                "re2c-tnfa-posix-gor1");
    bench_re2c(r, s, n, REG_NFA | REG_GTOP,     "re2c-tnfa-posix-gtop");
    bench_re2c(r, s, n, REG_NFA | REG_LEFTMOST, "re2c-tnfa-leftmost");
    if (!notrie) {
        bench_re2c(r, s, n, REG_NFA | REG_TRIE,                "re2c-tnfa-posix-gtop-trie");
        bench_re2c(r, s, n, REG_NFA | REG_LEFTMOST | REG_TRIE, "re2c-tnfa-leftmost-trie");
    }

#ifdef HAVE_RE2_RE2_H
    bench_re2(r, s, n, "re2");
#endif
}

int main()
{
    static const size_t VERY_LONG = 1 << 16;
    const char *regexp, *string;
    char *longstring = new char[VERY_LONG + 1];
    longstring[VERY_LONG] = 0;

    regexp = "([a-zA-Z]([a-zA-Z0-9+.-])*)[:]([/][/](((([a-zA-Z0-9._~-]|[%][0-9a-fA-F]{2}|[!$&'()*+,;=:])*)[@])?(([[]((([0-9a-fA-F]{1,4}[:]){6}([0-9a-fA-F]{1,4}[:][0-9a-fA-F]{1,4}|(([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])[.]([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])[.]([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])[.]([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])))|[:][:]([0-9a-fA-F]{1,4}[:]){5}([0-9a-fA-F]{1,4}[:][0-9a-fA-F]{1,4}|(([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])[.]([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])[.]([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])[.]([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])))|([0-9a-fA-F]{1,4})?[:][:]([0-9a-fA-F]{1,4}[:]){4}([0-9a-fA-F]{1,4}[:][0-9a-fA-F]{1,4}|(([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])[.]([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])[.]([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])[.]([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])))|(([0-9a-fA-F]{1,4}[:]){0,1} [0-9a-fA-F]{1,4})?[:][:]([0-9a-fA-F]{1,4}[:]){3}([0-9a-fA-F]{1,4}[:][0-9a-fA-F]{1,4}|(([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])[.]([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])[.]([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])[.]([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])))|(([0-9a-fA-F]{1,4}[:]){0,2} [0-9a-fA-F]{1,4})?[:][:]([0-9a-fA-F]{1,4}[:]){2}([0-9a-fA-F]{1,4}[:][0-9a-fA-F]{1,4}|(([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])[.]([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])[.]([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])[.]([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])))|(([0-9a-fA-F]{1,4}[:]){0,3} [0-9a-fA-F]{1,4})?[:][:][0-9a-fA-F]{1,4}[:]([0-9a-fA-F]{1,4}[:][0-9a-fA-F]{1,4}|(([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])[.]([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])[.]([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])[.]([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])))|(([0-9a-fA-F]{1,4}[:]){0,4} [0-9a-fA-F]{1,4})?[:][:]([0-9a-fA-F]{1,4}[:][0-9a-fA-F]{1,4}|(([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])[.]([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])[.]([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])[.]([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])))|(([0-9a-fA-F]{1,4}[:]){0,5} [0-9a-fA-F]{1,4})?[:][:][0-9a-fA-F]{1,4}|(([0-9a-fA-F]{1,4}[:]){0,6} [0-9a-fA-F]{1,4})?[:][:])|([v][0-9a-fA-F]+[.]([a-zA-Z0-9._~-]|[!$&'()*+,;=]|[:])+))[\\]])|(([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])[.]([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])[.]([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35])[.]([0-9]|[\x31-\x39][0-9]|[1][0-9]{2}|[2][\x30-\x34][0-9]|[2][5][\x30-\x35]))|(([a-zA-Z0-9._~-]|[%][0-9a-fA-F]{2}|[!$&'()*+,;=])*))([:][0-9]*)?)(([/]([a-zA-Z0-9._~-]|[%][0-9a-fA-F]{2}|[!$&'()*+,;=:@])*)*)|([/](([a-zA-Z0-9._~-]|[%][0-9a-fA-F]{2}|[!$&'()*+,;=:@])+([/]([a-zA-Z0-9._~-]|[%][0-9a-fA-F]{2}|[!$&'()*+,;=:@])*)*)?)|(([a-zA-Z0-9._~-]|[%][0-9a-fA-F]{2}|[!$&'()*+,;=:@])+([/]([a-zA-Z0-9._~-]|[%][0-9a-fA-F]{2}|[!$&'()*+,;=:@])*)*)|([/]?))([?](([a-zA-Z0-9._~-]|[%][0-9a-fA-F]{2}|[!$&'()*+,;=:@])|[/?])*)?([#](([a-zA-Z0-9._~-]|[%][0-9a-fA-F]{2}|[!$&'()*+,;=:@])|[/?])*)?";
    string = "http://240.147.163.34:2124/z/dibws/hkwfcdsvlghmmyd/ltiyxrfuam?fshxiix3zNzrBJNnuLDXnOruEmp4fX";
    bench(regexp, string, 1000, false);

    regexp = "(a|aa)*";
    memset(longstring, 'a', VERY_LONG);
    longstring[VERY_LONG - 1] = 0;
    bench(regexp, longstring, 1, true);

    regexp = "((((a)*))*|(((((a)))*))+)*";
    memset(longstring, 'a', VERY_LONG);
    bench(regexp, longstring, 1, true);

    regexp = "(X|Xa|Xab|Xaba|abab|baba|bY|Y)*";
    longstring[0] = 'X';
    for (size_t i = 1; i < VERY_LONG - 1; ) {
        longstring[i++] = 'a';
        longstring[i++] = 'b';
    }
    longstring[VERY_LONG - 1] = 'Y';
    bench(regexp, longstring, 1, true);

    regexp = "(((..)|(.))((..)|(.))((..)|(.)))*";
    memset(longstring, 'a', VERY_LONG);
    bench(regexp, longstring, 1, true);

    // regexp = "(((){0,100}){0,100}){0,100}";
    // regexp = "(){0,10000}";
    regexp = "(((){0,10}){0,10}){0,10}";
    string = "";
    bench(regexp, string, 1, false);

    // Pathological case for constant-memory POSIX algorithms (includes TDFA).
    // Takes quibic time in the size of counter. This is caused by quadratic-
    // time computation of precedence matrix on each step (the number of TNFA
    // states in the closure approaches TNFA size), multiplied by the length
    // of compared histores (which also approaches TNFA size). Trie-based
    // algorithms are not affected, but they consume memory proportional to
    // the length of the input string, and so are also not practical.
    regexp = "((a?){1,200})*";
    bench(regexp, "a", 1, false);

    delete[] longstring;
    return 0;
}

