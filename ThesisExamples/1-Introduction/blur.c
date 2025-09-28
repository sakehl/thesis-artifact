#include <immintrin.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <omp.h>

static inline int acc(int x, int y, int width) {
 return x+y*width;
}

void blur(const uint16_t *in, uint16_t *bv, int width, int height) {
 uint16_t *bh = (uint16_t*) malloc((height+2)*(width)*sizeof(uint16_t));
 for (int y = 0; y < height+2; y++)
  for (int x = 0; x < width; x++)
   bh[acc(x, y, width)] = (in[acc(x, y, width+2)] + 
    in[acc(x+1,y,width+2)] + in[acc(x+2, y, width+2)])/3;
 for (int y = 0; y < height; y++)
  for (int x = 0; x < width; x++)
   bv[acc(x, y, width)] = (bh[acc(x, y, width)] + 
    bh[acc(x, y+1, width)] + bh[acc(x, y+2, width)])/3;
 free(bh);
}

void fast_blur(const uint16_t *in, uint16_t *bv, int width, int height) {
 __m128i one_third = _mm_set1_epi16(21846);
 #pragma omp parallel for
 for (int yTile= 0; yTile < height; yTile += 32) { //~|\label{line:opt-blur-par}|
    __m128i a, b, c, sum, avg;
  __m128i bh[(256 / 8) * (32 + 2)];
  for (int xTile = 0; xTile < width; xTile += 256) {
   __m128i *bhPtr = bh;
   for (int y = 0; y < 32 + 2; y++) {
    const uint16_t *inPtr = &in[acc(xTile, yTile+y, width+2)];
    for (int x = 0; x < 256; x += 8) {
     a = _mm_loadu_si128((__m128i *)(inPtr)); //~|\label{line:opt-blur-1}|
     b = _mm_loadu_si128((__m128i *)(inPtr + 1)); //~|\label{line:opt-blur-2}|
     c = _mm_loadu_si128((__m128i *)(inPtr + 2)); //~|\label{line:opt-blur-3}|
     sum = _mm_add_epi16(_mm_add_epi16(a, b), c);
     avg = _mm_mulhi_epi16(sum, one_third);
     _mm_store_si128(bhPtr++, avg); //~|\label{line:opt-blur-pointer-1}|
     inPtr += 8;
   }}
   bhPtr = bh;
   for (int y = 0; y < 32; y++) {
    __m128i *outPtr = (__m128i *)&bv[acc(xTile, yTile + y, width)];
    for (int x = 0; x < 256; x += 8) {
     a = _mm_load_si128(bhPtr + (256 * 2) / 8);
     b = _mm_load_si128(bhPtr + 256 / 8);
     c = _mm_load_si128(bhPtr++); //~|\label{line:opt-blur-pointer-2}|
     sum = _mm_add_epi16(_mm_add_epi16(a, b), c);
     avg = _mm_mulhi_epi16(sum, one_third); 
     _mm_store_si128(outPtr++, avg); //~|\label{line:opt-blur-4}|
}}}}}

int main() {
    int height = 32 * 32*2;
    int width = 4 * 4 * 512;

    uint16_t *in = (uint16_t*)aligned_alloc(16, ((height+2) * (width+2)) * sizeof(uint16_t));
    uint16_t *bv = (uint16_t*)aligned_alloc(16, (height * width) * sizeof(uint16_t));
    uint16_t *bv2 = (uint16_t*)aligned_alloc(16, (height * width) * sizeof(uint16_t));
    if (in == NULL || bv == NULL || bv2 == NULL) {
        printf("Error allocating memory\n");
        return 1;
    }

    for (int j = 0; j < height+2; j++) {
        for (int i = 0; i < width+2; i++) {
            in[acc(i, j, width+2)] = (i+j) % 1024;
            if (i < width && j < height) {
                bv[acc(i, j, width)] = 0;
                bv2[acc(i, j, width)] = 0;
            }
        }
    }

    long start, end;
    struct timeval timecheck;

    // Warm up
    blur(in, bv, width, height);

    // Time the blur function
    gettimeofday(&timecheck, NULL);
    start = (long)timecheck.tv_sec * 1000 * 1000+(long)timecheck.tv_usec;
    int repetitions = 50;

    for (int i = 0; i < repetitions; i++) {
        blur(in, bv, width, height);
    }

    gettimeofday(&timecheck, NULL);
    end = (long)timecheck.tv_sec * 1000 * 1000+(long)timecheck.tv_usec;

    double time_taken_b = (double)(end-start) / (1000.0 * repetitions);
    printf("blur (avg %d times) in %f ms.\n", repetitions, time_taken_b);

    // Warm up
    fast_blur(in, bv2, width, height);

    // Time the fast_blur function
    gettimeofday(&timecheck, NULL);
    start = (long)timecheck.tv_sec * 1000 * 1000+(long)timecheck.tv_usec;

    for (int i = 0; i < repetitions; i++) {
        fast_blur(in, bv2, width, height);
    }

    gettimeofday(&timecheck, NULL);
    end = (long)timecheck.tv_sec * 1000 * 1000+(long)timecheck.tv_usec;

    double time_taken = (double)(end-start) / (1000.0 * repetitions);
    printf("fast_blur (avg %d times) in %f ms.\n", repetitions, time_taken);
    printf("Speedup: %f\n", time_taken_b / time_taken);

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            if (bv[acc(i, j, width)] != bv2[acc(i, j, width)]) {
                printf("Error at %d %d: %hu %hu\n", i, j,
                       bv[acc(i, j, width)],
                       bv2[acc(i, j, width)]);
                return 1;
            }
        }
    }
    printf("Succes!\n");
    free(in);
    free(bv);
    free(bv2);

    return 0;
}