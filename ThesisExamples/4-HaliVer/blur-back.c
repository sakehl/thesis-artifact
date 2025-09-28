#include <stdint.h>
#include <stdlib.h>
/* Euclidean division is defined internally in VerCors*/
/*@pure int hdiv(int x, int y) = y == 0 ? 0 : \euclidean_div(x, y); @*/
/*@ requires y != 0;
 ensures \result == \euclidean_div(x, y); @*/
inline /*@pure@*/ int div_eucl(int x, int y){
 int q = x/y; int r = x%y;
 return r < 0 ? q + (y > 0 ? -1 : 1) : q;
}
/* Definitions for buffers */
struct halide_dimension_t {
 int min, extent, stride;
};
struct halide_buffer {
 int dimensions;
 struct halide_dimension_t *dim;
 int *host;
};
struct halide_buffer_const {
 int dimensions;
 struct halide_dimension_t *dim;
 const int *host;
};

/*@ context inp_buffer != NULL ** \pointer_length(inp_buffer) == 1 ** 
  Perm(inp_buffer->host, 1\2) ** inp_buffer->host != NULL;
 context \pointer_length(inp_buffer->host) == 1026 * 1922;
 context blur_y_buffer != NULL ** \pointer_length(blur_y_buffer) == 1 ** 
  Perm(blur_y_buffer->host, 1\2) ** blur_y_buffer->host != NULL; 
 context \pointer_length(blur_y_buffer->host) == 1024 * 1920;
 context (\forall* int x, int y; 0 <= x && x < 0 + 1920 && 
  0 <= y && y < 0 + 1024; 
  Perm({:blur_y_buffer->host[y*1920 + x]:}, 1\1));
 ensures (\forall int x, int y; 0 <= x && x < 0 + 1920 && 
  0 <= y && y < 0 + 1024; 
  {:blur_y_buffer->host[y*1920 + x]:} == 
   hdiv(hdiv(inp_buffer->host[y*1922 + x + 3846] + 
    inp_buffer->host[y*1922 + x + 3844] + 
    inp_buffer->host[y*1922 + x + 3845], 3) + 
   hdiv(inp_buffer->host[y*1922 + x + 2] + 
    inp_buffer->host[y*1922 + x] + 
    inp_buffer->host[y*1922 + x + 1], 3) + 
   hdiv(inp_buffer->host[y*1922 + x + 1924] + 
    inp_buffer->host[y*1922 + x + 1922] + 
    inp_buffer->host[y*1922 + x + 1923], 3), 3));@*/
int haliver_blur(struct halide_buffer_const *inp_buffer,
      /*@unique_pointer_field<host,1>@*/ struct halide_buffer *blur_y_buffer) {
 const int* inp = inp_buffer->host;
 /*@unique<1>@*/int* blur_y = blur_y_buffer->host;
 // produce blur_y
 #pragma omp parallel for
 for (int yo = 0; yo < 0 + 128; yo++)
/*@  context 0 <= yo && yo < 0 + 128;
  context (\forall* int yif, int xof, int xif; 
   0 <= yif && yif < 8 && 0 <= xof && xof < 960 && 0 <= xif && xif < 2;
   Perm({:blur_y[(yo*8 + yif)*1920 + xof*2 + xif]:}, 1\1));
  ensures (\forall int yif, int xof, int xif; 0 <= yif && yif < 8 &&
   0 <= xof && xof < 960 && 0 <= xif && xif < 2; 
   {:blur_y[(yo*8 + yif)*1920 + xof*2 + xif]:} == 
    hdiv(hdiv(inp[(yo*8 + yif)*1922 + xof*2 + xif + 1923] + 
     inp[(yo*8 + yif)*1922 + xof*2 + xif + 1924] + 
     inp[(yo*8 + yif)*1922 + xof*2 + xif + 1922], 3) + 
    hdiv(inp[(yo*8 + yif)*1922 + xof*2 + xif + 3845] + 
     inp[(yo*8 + yif)*1922 + xof*2 + xif + 3846] + 
     inp[(yo*8 + yif)*1922 + xof*2 + xif + 3844], 3) + 
    hdiv(inp[(yo*8 + yif)*1922 + xof*2 + xif + 1] + 
     inp[(yo*8 + yif)*1922 + xof*2 + xif + 2] + 
     inp[(yo*8 + yif)*1922 + xof*2 + xif], 3), 3));@*/
 {
  {
   int64_t _2 = 19200;
   int *blur_x = (int*)malloc(sizeof(int)*_2);
   /*@assume blur_x != NULL;@*/ //~|\label{line:assume_blur_x_not_null}|
   int _t12 = yo * 8;
/*@    loop_invariant 0 <= yi && yi <= 0 + 8;
    loop_invariant (\forall* int x, int y; 0 <= x && x < 1920 && 
     yo*8 <= y && y < yo*8 + 10; 
     Perm({:blur_x[(y - yo*8)*1920 + x]:}, 1\1));
    loop_invariant (\forall* int yif, int xof, int xif; 0 <= yif && yif < 8 &&
     0 <= xof && xof < 960 && 0 <= xif && xif < 2; 
     Perm({:blur_y[(yo*8 + yif)*1920 + xof*2 + xif]:}, 1\1));
    loop_invariant (\forall int yif, int xof, int xif; 0 <= yif && yif < yi && 
     0 <= xof && xof < 960 && 0 <= xif && xif < 2; 
     {:blur_y[(yo*8 + yif)*1920 + xof*2 + xif]:} == 
     hdiv(hdiv(inp[(yo*8 + yif)*1922 + xof*2 + xif + 1923] + 
      inp[(yo*8 + yif)*1922 + xof*2 + xif + 1924] + 
      inp[(yo*8 + yif)*1922 + xof*2 + xif + 1922], 3) + 
     hdiv(inp[(yo*8 + yif)*1922 + xof*2 + xif + 3845] + 
      inp[(yo*8 + yif)*1922 + xof*2 + xif + 3846] + 
      inp[(yo*8 + yif)*1922 + xof*2 + xif + 3844], 3) + 
     hdiv(inp[(yo*8 + yif)*1922 + xof*2 + xif + 1] + 
     inp[(yo*8 + yif)*1922 + xof*2 + xif + 2] + 
     inp[(yo*8 + yif)*1922 + xof*2 + xif], 3), 3));@*/
   for (int yi = 0; yi < 0 + 8; yi++) {
    // produce blur_x
    int _t13 = yi + _t12;
/*@     loop_invariant _t13 <= y && y <= _t13 + 3;
     loop_invariant (\forall* int yf, int xof, int xif; 
      yo*8 + yi <= yf && yf < yo*8 + yi + 3 && 
      0 <= xof && xof < 960 && 0 <= xif && xif < 2; 
      Perm({:blur_x[(yf - yo*8)*1920 + xof*2 + xif]:}, 1\1));
     loop_invariant (\forall int yf, int xof, int xif; 
      yo*8 + yi <= yf && yf < y && 0 <= xof && xof < 960 && 
      0 <= xif && xif < 2; {:blur_x[(yf - yo*8)*1920 + xof*2 + xif]:} == 
      hdiv(inp[(yf*961 + xof)*2 + xif + 1] + 
       inp[(yf*961 + xof)*2 + xif + 2] + 
       inp[(yf*961 + xof)*2 + xif], 3)); @*/
    for (int y = _t13; y < _t13 + 3; y++) {
     int _t15 = ((y - _t12) * 960);
     int _t14 = (y * 961);
/*@      loop_invariant 0 <= xo && xo <= 0 + 960;
      loop_invariant (\forall* int xof, int xif; 
        0 <= xof && xof < 960 && 0 <= xif && xif < 2; 
        Perm({:blur_x[(y - yo*8)*1920 + xof*2 + xif]:}, 1\1));
      loop_invariant (\forall int xof, int xif; 
       0 <= xof && xof < xo && 0 <= xif && xif < 2; 
       {:blur_x[(y - yo*8)*1920 + xof*2 + xif]:} == 
       hdiv(inp[y*1922 + xof*2 + xif + 1] + 
        inp[y*1922 + xof*2 + xif + 2] + 
        inp[y*1922 + xof*2 + xif], 3));@*/
     for (int xo = 0; xo < 0 + 960; xo++) {
      int _t8 = xo + _t14;
      int _3 = inp[_t8 * 2 + 1];
      int _4 = inp[_t8 * 2 + 2];
      int _5 = inp[_t8 * 2];
      blur_x[(xo + _t15) * 2] = div_eucl(_3 + _4 + _5, 3);
      int _t9 = xo + _t14;
      int _6 = inp[_t9 * 2 + 2];
      int _7 = inp[_t9 * 2 + 3];
      int _8 = inp[_t9 * 2 + 1];
      blur_x[(xo + _t15) * 2 + 1] = div_eucl(_6 + _7 + _8, 3);
     } // for xo
    } // for y
    // consume blur_x
    int _t17 = (yi + _t12) * 960;
    int _t16 = yi * 960;
/*@     loop_invariant 0 <= xo && xo <= 0 + 960;
     loop_invariant (\forall* int xo, int y;
      0 <= xo && xo < 1920 && yo*8 <= y && y < yo*8 + 10;
      Perm({:blur_x[(y - yo*8)*1920 + xo]:}, 1\(2*2*2)));
     loop_invariant (\forall int xo, int y; 
      0 <= xo && xo <= 1919 && yo*8 + yi <= y && y <= yo*8 + yi + 2; 
      {:blur_x[(y - yo*8)*1920 + xo]:} == 
       hdiv(inp[y*1922 + xo + 1] + inp[y*1922 + xo + 2] + inp[y*1922 + xo], 3));
     loop_invariant (\forall* int xof, int xif; 
      0 <= xof && xof < 960 && 0 <= xif && xif < 2;
      Perm({:blur_y[(yo*8 + yi)*1920 + xof*2 + xif]:}, 1\1));
     loop_invariant (\forall int xof, int xif; 
      0 <= xof && xof < xo && 0 <= xif && xif < 2;
      {:blur_y[(yo*8 + yi)*1920 + xof*2 + xif]:} == 
      hdiv(hdiv(inp[(yo*8 + yi)*1922 + xof*2 + xif + 1923] + 
       inp[(yo*8 + yi)*1922 + xof*2 + xif + 1924] + 
       inp[(yo*8 + yi)*1922 + xof*2 + xif + 1922], 3) + 
      hdiv(inp[(yo*8 + yi)*1922 + xof*2 + xif + 3845] + 
      inp[(yo*8 + yi)*1922 + xof*2 + xif + 3846] + 
      inp[(yo*8 + yi)*1922 + xof*2 + xif + 3844], 3) + 
     hdiv(inp[(yo*8 + yi)*1922 + xof*2 + xif + 1] + 
      inp[(yo*8 + yi)*1922 + xof*2 + xif + 2] + 
      inp[(yo*8 + yi)*1922 + xof*2 + xif], 3), 3)); 
@*/    for (int xo = 0; xo < 0 + 960; xo++) {
     int _t10 = (xo + _t16);
     int _9 = blur_x[_t10 * 2 + 1920];
     int _10 = blur_x[_t10 * 2 + 3840];
     int _11 = blur_x[_t10 * 2];
     blur_y[(xo + _t17) * 2] = div_eucl(_9 + _10 + _11, 3);
     int _t11 = (xo + _t16);
     int _12 = blur_x[_t11 * 2 + 1921];
     int _13 = blur_x[_t11 * 2 + 3841];
     int _14 = blur_x[_t11 * 2 + 1];
     blur_y[(xo + _t17) * 2 + 1] = div_eucl(_12 + _13 + _14, 3);
    } // for xo
   } // for yi
   free(blur_x);
  } // alloc blur_x
 } // for yo
 return 0;
}