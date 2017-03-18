#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <assert.h>
#include <immintrin.h>
#include <xmmintrin.h>

#define TEST_W 4096
#define TEST_H 4096

/* provide the implementations of naive_transpose,
 * sse_transpose, sse_prefetch_transpose
 */

#include "impl.c"

static long diff_in_us(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec - t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec * 1000000.0 + diff.tv_nsec / 1000.0);
}

int main(int argc, char *argv[])
{
    /* verify the result of 4x4 matrix */
    {
        int testin[64] = { 0, 1,  2,  3,  4,  5,  6,  7,
                           8, 9, 10, 11, 12, 13, 14, 15,
                           16, 17, 18, 19, 20, 21, 22, 23,
                           24, 25, 26, 27, 28, 29, 30, 31,
                           32, 33, 34, 35, 36, 37, 38, 39,
                           40, 41, 42, 43, 44, 45, 46, 47,
                           48, 49, 50, 51, 52, 53, 54, 55,
                           56, 57, 58, 59, 60, 61, 62, 63
                         };
        int testout[64];
        int expected[64] = { 0, 8, 16, 24, 32, 40, 48, 56,
                             1, 9, 17, 25, 33, 41, 49, 57,
                             2, 10, 18, 26, 34, 42, 50, 58,
                             3, 11, 19, 27, 35, 43, 51, 59,
                             4, 12, 20, 28, 36, 44, 52, 60,
                             5, 13, 21, 29, 37, 45, 53, 61,
                             6, 14, 22, 30, 38, 46, 54, 62,
                             7, 15, 23, 31, 39, 47, 55, 63
                           };
#if defined(AVX_PREFETCH)
        avx_prefetch_transpose(testin, testout, 8, 8, (argc == 2) ? atoi(argv[1]) : 8);
#endif
#if defined(AVX)
        avx_transpose(testin, testout, 8, 8);
#endif
#if defined(SSE_PREFETCH)
        sse_prefetch_transpose(testin, testout, 8, 8, (argc == 2) ? atoi(argv[1]) : 8);
#endif
#if defined(SSE)
        sse_transpose(testin, testout, 8, 8);
#endif
#if defined(NAIVE)
        naive_transpose(testin, testout, 8, 8);
#endif
        assert(0 == memcmp(testout, expected, 64 * sizeof(int)) &&
               "Verification fails");
    }

    {
        struct timespec start, end;
        int *src = (int *) malloc(sizeof(int) * TEST_W * TEST_H);
        int *out = (int *) malloc(sizeof(int) * TEST_W * TEST_H);

        srand(time(NULL));
        for (int y = 0; y < TEST_H; y++)
            for (int x = 0; x < TEST_W; x++)
                *(src + y * TEST_W + x) = rand();
#if defined(AVX_PREFETCH)
        clock_gettime(CLOCK_REALTIME, &start);
        avx_prefetch_transpose(src, out, TEST_W, TEST_H, (argc == 2) ? atoi(argv[1]) : 8);
        clock_gettime(CLOCK_REALTIME, &end);
        printf("%ld ", diff_in_us(start, end));
#endif
#if defined(AVX)
        clock_gettime(CLOCK_REALTIME, &start);
        avx_transpose(src, out, TEST_W, TEST_H);
        clock_gettime(CLOCK_REALTIME, &end);
        printf("%ld ", diff_in_us(start, end));
#endif
#if defined(SSE_PREFETCH)
        clock_gettime(CLOCK_REALTIME, &start);
        sse_prefetch_transpose(src, out, TEST_W, TEST_H, (argc == 2) ? atoi(argv[1]) : 8);
        clock_gettime(CLOCK_REALTIME, &end);
        printf("%ld ", diff_in_us(start, end));
#endif
#if defined(SSE)
        clock_gettime(CLOCK_REALTIME, &start);
        sse_transpose(src, out, TEST_W, TEST_H);
        clock_gettime(CLOCK_REALTIME, &end);
        printf("%ld ", diff_in_us(start, end));
#endif
#if defined(NAIVE)
        clock_gettime(CLOCK_REALTIME, &start);
        naive_transpose(src, out, TEST_W, TEST_H);
        clock_gettime(CLOCK_REALTIME, &end);
        printf("%ld ", diff_in_us(start, end));
#endif
        free(src);
        free(out);
    }
    return 0;
}
