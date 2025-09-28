#include <stdint.h>
#include <stdlib.h>

/*@ ghost
 requires 0 <= x && x<width && 0 <= y && y<height;
 ensures x+y*width >= x && x+y*width <= width*height - width+x;
 ensures x == x+y*width % width && y == x+y*width / width;
 ensures \result;
 decreases;
bool pure axioms_acc(int x, int y, int width, int height);@*/

 /*@requires 0 <= x && x<width && 0 <= y && y<height;
 ensures \result == x+y*width;
 ensures \result >= 0 && \result < width*height; //~|\label{line:pre-blur-axiom-1}|
 ensures \result >= x && \result <= width*height - width+x; //~|\label{line:pre-blur-axiom-2}|
 ensures x == \result % width && y == \result / width; //~|\label{line:pre-blur-axiom-3}|
 decreases;@*/
/*@opaque@*/ /*@pure@*/ int acc(int x, int y, int width, int height){ //~|\label{line:pre-blur-acc}|
 /*@assert axioms_acc(x, y, width, height); @*/ //~|\label{line:pre-blur-acc-axiom}|
 return x + y*width;}
/*@
 ghost
 requires in != NULL && \pointer_length(in) == (width+2)*(height+2);
 requires x >= 0 && x < width;
 requires y >= 0 && y < height+2;
 decreases;
opaque pure int blur_x(int x,int y, int width, int height, const uint16_t *in){//~|\label{line:pre-blur-blurx}|
 return (in[acc(x ,y, width+2, height+2)] +
  in[acc(x+1 ,y, width+2, height+2)] +
  in[acc(x+2 ,y, width+2, height+2)])/3;}

 ghost
 requires in != NULL && \pointer_length(in) == (width+2)*(height+2);
 requires x >= 0 && x < width;
 requires y >= 0 && y < height;
 decreases;
opaque pure int blur_y(int x,int y, int width, int height, const uint16_t *in){//~|\label{line:pre-blur-blury}|
 return (blur_x(x,y, width, height, in) +
  blur_x(x,y+1, width, height, in) +
  blur_x(x,y+2, width, height, in))/ 3;}@*/

typedef short __m128i __attribute__ ((__vector_size__ (sizeof(short)*8))); //~|\label{line:pre-blur-vector-def}|

static inline __m128i _mm_set_epi16(short w7, short w6, short w5, short w4, //~|\label{line:pre-blur-vector-1}|
  short w3, short w2, short w1, short w0){
 __m128i res = { w0, w1, w2, w3, w4, w5, w6, w7};
 return res;}

static inline __m128i _mm_set1_epi16 (short w){
 return _mm_set_epi16 (w, w, w, w, w, w, w, w);}

static inline __m128i _mm_load_si128(__m128i *p){ return *p; }

static inline __m128i _mm_add_epi16(__m128i a, __m128i b){ return a + b; } //~|\label{line:pre-blur-vector-2}|
/*@
 ghost 
 ensures (uint16_t)( ((uint32_t) a)*((uint32_t) b) / \pow(2,16)) == \result;
 decreases;
pure uint16_t mulhi_short(short a, short b);@*/
/*@ 
 ghost
 ensures \result[0]==mulhi_short(a[0], b[0]) &&
  \result[1]==mulhi_short(a[1], b[1]) && \result[2]==mulhi_short(a[2], b[2]) &&
  \result[3]==mulhi_short(a[3], b[3]) && \result[4]==mulhi_short(a[4], b[4]) &&
  \result[5]==mulhi_short(a[5], b[5]) && \result[6]==mulhi_short(a[6], b[6]) &&
  \result[7]==mulhi_short(a[7], b[7]);
 decreases;
pure __m128i __builtin_ia32_pmulhw128(__m128i a, __m128i b); @*/

static inline __m128i _mm_mulhi_epi16(__m128i a, __m128i b){
 return (__m128i)__builtin_ia32_pmulhw128(a, b);}

static inline void _mm_storeu_si128(__m128i *p, __m128i b){*p = b;}
/*@
 given const uint16_t * original_p;
 given int x; given int y; given int width; given int height;
 requires original_p != NULL && \pointer_length(original_p) >= width*height;
 requires x >= 0 && x <= width-8 && y >= 0 && y < height;
 requires p == original_p + acc(x, y, width, height);
 ensures (\forall int v; 0<=v && v<8;
  {:\result[v]:} == original_p[acc(x+v, y, width, height)]);
 decreases;@*/
/*@pure@*/ /*@opaque@*/ __m128i load_epi16(const uint16_t * p){ //~|\label{line:pre-blur-vector-3}|
 __m128i r = {p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]};
 return r;}
/*@ 
 given uint16_t * original_p;
 given int x; given int y; given int width; given int height;
 context original_p != NULL && \pointer_length(original_p) >= width*height;
 context x >= 0 && x <= width-8 && y >= 0 && y < height;
 context p == original_p + acc(x, y, width, height);
 context (\forall* int v; 0<=v && v<8; 
  Perm({:original_p[acc(x+v, y, width, height)]:}, write));
 ensures (\forall int v; 0<=v && v<8;
  b[v] == {:original_p[acc(x+v, y, width, height)]:} );
 decreases; @*/
void store_epi16(uint16_t * p, __m128i b){ /*@ //~|\label{line:pre-blur-vector-4}|
 assert acc(x, y, width, height)+1 == acc(x+1, y, width, height); //~|\label{line:pre-blur-assert-1}|
 assert acc(x, y, width, height)+2 == acc(x+2, y, width, height);
 assert acc(x, y, width, height)+3 == acc(x+3, y, width, height);
 assert acc(x, y, width, height)+4 == acc(x+4, y, width, height);
 assert acc(x, y, width, height)+5 == acc(x+5, y, width, height);
 assert acc(x, y, width, height)+6 == acc(x+6, y, width, height);
 assert acc(x, y, width, height)+7 == acc(x+7, y, width, height); @*/ //~|\label{line:pre-blur-assert-2}|
 p[0] = b[0];p[1] = b[1];p[2] = b[2];p[3] = b[3];
 p[4] = b[4];p[5] = b[5];p[6] = b[6];p[7] = b[7];}

/*@ context_everywhere in != NULL && \pointer_length(in) == (width+2)*(height+2) && //~|\label{line:opt-blur-pre-1}|
  bv != NULL && \pointer_length(bv) == width*height &&
  width > 0 && height > 0;
 context_everywhere height % 32 == 0 && width % 256 == 0; //~|\label{line:opt-blur-pre-2}|
 context (\forall* int i, int j; 0<=i&&i<width && 0<=j && j<height;
  Perm({:bv[acc(i,j,width,height)]:}, write)); //~|\label{line:opt-blur-acc-1}|
 context_everywhere (\forall int i, int k; 0<=i&&i<width+2 && 0<=k && k<height+2;
  0 <= {:in[acc(i, k, width+2, height+2)]:} &&
  in[acc(i, k, width+2, height+2)] < \pow(2,15)/3); //~|\label{line:opt-blur-pre-3}|
 ensures (\forall int i, int j;
  0<=i&&i<width && 0<=j && j<height;
  {:bv[acc(i,j,width,height)]:} == blur_y(i, j, width, height, in)); //~|\label{line:opt-blur-quant-trigger-1}|
 decreases; @*/ //~|\label{line:opt-blur-decreases-1}|
void fast_blur(const uint16_t *in, uint16_t* bv, int width, int height) {
 __m128i one_third = _mm_set1_epi16(21846);
 #pragma omp parallel for
 for (int yo = 0; yo < height/32; yo++)/*@ 
  context 0 <= yo && yo < height/32;
  context (\forall* int i, int j; 0<=i&&i<width && 0<=j&&j<32;
   Perm({:bv[acc(i ,j+yo*32,width,height)]:}, write));
  ensures (\forall int xTilef, int yf, int xf, int v; 0<=v && v<8 &&
   0<= yf && yf<32 && 0<= xf && xf<256/8 && 0 <= xTilef && xTilef < width/256;
   {:bv[acc(256*xTilef+8*xf+v ,yo*32+yf,width,height)]:} == //~|\label{line:opt-blur-quant-trigger-2}|
   blur_y(256*xTilef+8*xf+v, yo*32+yf, width, height, in));  
   @*/ {
  __m128i a, b, c, sum, avg;
  __m128i bh[(256/8)*(32+2)];/*@
   loop_invariant xTile % 256 == 0 && 0 <= xTile && xTile <= width;
   loop_invariant bh != NULL && \pointer_length(bh) == (256/8)*(32+2);
   loop_invariant (\forall* int i, int j; 0<=i&&i<(256/8) && 0<=j&&j<(32+2);
    Perm({:bh[acc(i, j, 256/8, 32+2)]:}, write));
   loop_invariant (\forall* int i, int j; 0<=i&&i<width && 0<=j&&j<32;
    Perm({:bv[acc(i ,j+yo*32,width,height)]:}, write));
   loop_invariant (\forall int xTilef, int yf, int xf, int v; 0<=v && v<8 &&
    0<= yf && yf<32 && 0<= xf && xf<256/8 && 0 <= xTilef && xTilef < xTile/256;
    {:bv[acc(256*xTilef+8*xf+v ,yf+yo*32,width,height)]:} == //~|\label{line:opt-blur-quant-trigger-3}|
    blur_y(256*xTilef+8*xf+v, yo*32+yf, width, height, in));
   decreases width-xTile; @*/ //~|\label{line:opt-blur-decreases-2}|
  for (int xTile = 0; xTile < width; xTile += 256) {
   __m128i *bhPtr = bh;/*@
    loop_invariant -1 <= y && y <= 32+1;
    loop_invariant bh != NULL && \pointer_length(bh) == (256/8)*(32+2);
    loop_invariant y < 33 ==> bhPtr == bh + acc(0, y+1, 256/8, 32+2);
    loop_invariant (\forall* int i, int j; 0<=i&&i<(256/8) && 0<=j&&j<(32+2);
     Perm({:bh[acc(i, j, 256/8, 32+2)]:}, write));
    loop_invariant (\forall int yf, int xf, int v;
     0<=v && v<8 && -1<= yf && yf<y && 0<= xf && xf<32;
     {:bh[acc(xf, yf+1, 256/8, 32+2)][v]:} == 
    blur_x(xTile+8*xf+v, yo*32+yf+1, width, height, in) &&
     0 <= bh[acc(xf, yf+1, 256/8, 32+2)][v] &&
     bh[acc(xf, yf+1, 256/8, 32+2)][v] < \pow(2,15)/3);
    decreases 33-y; @*/
   for (int y = -1; y < 32+1; y++) {
    const uint16_t *inPtr = &in[acc(xTile+1, yo*32+y+1, width+2, height+2)];
     /*@loop_invariant x%8==0 && 0 <= x && x <= 256;
     loop_invariant bh != NULL && \pointer_length(bh) == (256/8)*(32+2);
     loop_invariant x < 256 ==> bhPtr == bh + acc(x/8, y+1, 256/8, 32+2);
     loop_invariant y != 32 && x == 256 ==> bhPtr == bh+acc(0, y+2, 256/8, 32+2);
     loop_invariant inPtr == in + acc(xTile+x+1, yo*32+y+1, width+2, height+2);
     loop_invariant (\forall* int xf; 0<= xf && xf<256/8;
      Perm({:bh[acc(xf, y+1, 256/8, 32+2)]:}, write));
     loop_invariant (\forall int xf, int v; 
      0<=v && v<8 && 0<= xf && xf<x/8;
      {:bh[acc(xf, y+1, 256/8, 32+2)][v]:} == 
      blur_x(xTile+8*xf+v, yo*32+y+1, width, height, in) &&
      0 <= bh[acc(xf, y+1, 256/8, 32+2)][v] &&
      bh[acc(xf, y+1, 256/8, 32+2)][v] < \pow(2,15)/3);
     decreases 256-x; @*/
    for (int x = 0; x < 256; x += 8) {
     a = load_epi16(inPtr - 1) /*@ given {original_p=in, x=xTile+x, y=yo*32+y+1,
      width=width+2, height=height+2}@*/;
     b = load_epi16(inPtr + 1) /*@ given {original_p=in, x=xTile+x+2, y=yo*32+y+1,
      width=width+2, height=height+2}@*/;
     c = load_epi16(inPtr) /*@ given {original_p=in, x=xTile+x+1, y=yo*32+y+1,
      width=width+2, height=height+2}@*/;
     sum = _mm_add_epi16(_mm_add_epi16(a, b), c);
     avg = _mm_mulhi_epi16(sum, one_third); //~|\label{line:opt-blur-mul-hi}|
     _mm_storeu_si128(bhPtr, avg);/*@
     assert (\forall int v; (0<=v && v<8); {:bh[acc(x/8, y+1, 256/8, 32+2)][v]:} == //~|\label{line:opt-blur-reveal-1}|
       reveal blur_x(xTile+x+v, yo*32+y+1, width, height, in));@*/ 
     if(!(y == 32 && x == 256-8)) {
      bhPtr++;
     }
     inPtr += 8;
    }}
   bhPtr = bh;/*@
    loop_invariant 0 <= y && y <= 32;
    loop_invariant bh != NULL && \pointer_length(bh) == (256/8)*(32+2);
    loop_invariant bhPtr == bh + acc(0, y, 256/8, 32+2);
    loop_invariant (\forall* int xf, int yf; 0<= xf && xf<256/8 && -1<= yf && yf<33;
     Perm({:bh[acc(xf, yf+1, 256/8, 32+2)]:}, write));
    loop_invariant (\forall int yf, int xf, int v;
     0<=v && v<8 && -1<= yf && yf<33 && 0<= xf && xf<32;
     {:bh[acc(xf, yf+1, 256/8, 32+2)][v]:} == 
     blur_x(xTile+8*xf+v, yo*32+yf+1, width, height, in) &&
     0 <= bh[acc(xf, yf+1, 256/8, 32+2)][v] &&
     bh[acc(xf, yf+1, 256/8, 32+2)][v] < \pow(2,15)/3);
    loop_invariant (\forall* int yf, int xf, int v;
     0<=v && v<8 && 0<= yf && yf<32 && 0<= xf && xf<256/8;
     Perm({:bv[acc(xTile+8*xf+v, yf+yo*32, width, height)]:}, write));
    loop_invariant (\forall int yf, int xf, int v;
     0<=v && v<8 && 0<= yf && yf<y && 0<= xf && xf<256/8;
     {:bv[acc(xTile+8*xf+v, yf+yo*32, width, height)]:} == 
     blur_y(xTile+8*xf+v, yo*32+yf, width, height, in));
    decreases 32-y; @*/
   for (int y = 0; y < 32; y++) {
    uint16_t *outPtr = &bv[acc(xTile, yo*32+y, width, height)];
     /*@loop_invariant x%8==0 && 0 <= x && x <= 256;
     loop_invariant bh != NULL && \pointer_length(bh) == (256/8)*(32+2);
     loop_invariant x < 256 ==> bhPtr == bh + acc(x/8, y, 256/8, 32+2);
     loop_invariant x == 256 ==> bhPtr == bh + acc(0, y+1, 256/8, 32+2);
     loop_invariant x<256 ==> outPtr == bv+acc(xTile+x, yo*32+y, width, height);
     loop_invariant (\forall* int xf, int yf; 0<= xf && xf<256/8 && -1<= yf && yf<33;
      Perm({:bh[acc(xf, yf+1, 256/8, 32+2)]:}, write));
     loop_invariant (\forall int yf, int xf, int v;
      0<=v && v<8 && -1<= yf && yf<33 && 0<= xf && xf<32;
      {:bh[acc(xf, yf+1, 256/8, 32+2)][v]:} == 
      blur_x(xTile+8*xf+v, yo*32+yf+1, width, height, in) &&
      0 <= bh[acc(xf, yf+1, 256/8, 32+2)][v] &&
      bh[acc(xf, yf+1, 256/8, 32+2)][v] < \pow(2,15)/3);
     loop_invariant (\forall* int xf, int v;
      0<=v && v<8 && 0<= xf && xf<256/8;
      Perm({:bv[acc(xTile+8*xf+v, y+yo*32, width, height)]:}, write)); //~|\label{line:opt-blur-trigger}|
     loop_invariant (\forall int xf, int v;
      0<=v && v<8 && 0<= xf && xf<x/8;
      {:bv[acc(xTile+8*xf+v, y+yo*32, width, height)]:} == 
      blur_y(xTile+8*xf+v, y+yo*32, width, height, in));
     decreases 256-x; @*/
    for (int x = 0; x < 256; x += 8) {/*@
     assert acc(x/8, y, 256/8, 32+2) + (256*2)/8 == acc(x/8, y+2, 256/8, 32+2);@*/
     a = _mm_load_si128(bhPtr + (256 * 2) / 8);/*@
     assert acc(x/8, y, 256/8, 32+2) + (256)/8 == acc(x/8, y+1, 256/8, 32+2);@*/
     b = _mm_load_si128(bhPtr + 256 / 8);
     c = _mm_load_si128(bhPtr);
     bhPtr++;
     sum = _mm_add_epi16(_mm_add_epi16(a, b), c);
     avg = _mm_mulhi_epi16(sum, one_third);
     store_epi16(outPtr, avg) /*@ given {original_p=bv, x=xTile+x, y=yo*32+y, 
      width=width, height=height}@*/;/*@
     assert (\forall int v; 0<=v&&v<8; {:bv[acc(xTile+x+v, y+yo*32, width, height)]:} //~|\label{line:opt-blur-reveal-2}|
      == reveal blur_y(xTile+x+v, yo*32+y, width, height, in)); @*/
     if(!(x == 256-8)) {
      outPtr += 8;
}}}}}}