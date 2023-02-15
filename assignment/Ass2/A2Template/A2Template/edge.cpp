#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
//#include <tmmintrin.h>
#include <immintrin.h> ///AVX
#include "bmp.h"
#include "measure.h"
//#include <x86intrin.h>
#include <iostream>
#include <iomanip>
#ifdef _WIN32
#include "Windows.h"
#include "malloc.h"
#define posix_memalign(address, alignment, size) *(address)=_aligned_malloc((size), (alignment))
#define sleep(s) Sleep(1000*s)
#else
#include <cstring>
#include "unistd.h"
#define memcpy_s(d, n, s, c) memcpy(d, s, c)
#endif
/* just used for time measurements */
#define REP 10
#define MIN(X, Y) (((X)<(Y))?X:Y)


void kirsch_operator_avx(unsigned char *data_out,
                         unsigned char *data_in, unsigned height,
                         unsigned width)
{
    /*
       Fill in your code here.
     */
}

