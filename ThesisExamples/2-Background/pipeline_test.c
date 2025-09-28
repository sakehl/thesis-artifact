#include <stdlib.h>
#include <stdio.h>
#include "square.h"

struct dimension { //~|\label{line:halide-dim}|
 int min, extent, stride;
};

struct buffer { //~|\label{line:halide-buf}|
 int dimensions;
 struct dimension* dim;
 void* host;
};

void pipeline(struct buffer* inp_buf, struct buffer* square_buf){
 // Input buffer dimensions
 int inp_x_min = inp_buf->dim[0].min;
 int inp_y_min = inp_buf->dim[1].min;
 int inp_y_stride = inp_buf->dim[1].stride;
 int* inp = (int*)inp_buf->host;
 // Output buffer dimensions
 int square_min = square_buf->dim[0].min; //~|\label{line:out-bounds-1}|
 int square_extent = square_buf->dim[0].extent;
 int* square = (int*)square_buf->host; //~|\label{line:out-bounds-2}|
 // Produce reflect
 // Determined by bound inferencing
 int rf_y_min = square_min; int rf_y_extent = square_extent; //~|\label{line:int-bounds-1}|
 int rf_x_min = square_min; int rf_x_extent = square_extent; //~|\label{line:int-bounds-2}|
 int* reflect = (int*) malloc(sizeof(int)*rf_x_extent*rf_y_extent); //~|\label{line:halide-alloc-square}|
 for(int y=rf_y_min;y<rf_y_min+rf_y_extent;y++)
  for(int x=rf_x_min;x<rf_x_min+rf_x_extent;x++){
  reflect[(y-rf_y_min)*rf_x_extent+x-rf_x_min] = 
   inp[(x-inp_y_min)*inp_y_stride + (y-inp_x_min)]; //~|\label{line:halide-access-input}|
 }
 // Consume reflect
 // Produce square
 for(int x=square_min;x<square_min+square_extent;x++){
  int v0 = reflect[(x-rf_y_min)*(rf_x_extent)+x-rf_x_min];
  square[x-square_min] = v0 * v0;
 }
 free(reflect);
}

void main(){
  struct buffer* inp_buf = (struct buffer*) malloc(sizeof(struct buffer));
  struct buffer* square_buf = (struct buffer*) malloc(sizeof(struct buffer));
  struct dimension d = {1, 256, 1};
  struct dimension d2[2] = {{1, 256, 1}, {1, 256, 256}};
  inp_buf->dim = d2;
  inp_buf->host = (int*) malloc(sizeof(int) * 256 * 256);
  square_buf->dim = &d;
  square_buf->host = (int*) malloc(sizeof(int) * 256);
  int* inp = (int*) inp_buf->host;
  int* square1 = (int*) square_buf->host;

  // int square(struct halide_buffer_int32_t *_inp_buffer, int32_t _inp_minx, int32_t _inp_extentx, int32_t _inp_stridex, int32_t _inp_miny, int32_t _inp_extenty, int32_t _inp_stridey, struct halide_buffer_int32_t *_square_buffer);
  struct halide_buffer_int32_t* inp_buf_h = (struct halide_buffer_int32_t*) malloc(sizeof(struct halide_buffer_int32_t));
  struct halide_buffer_int32_t* square_buf_h = (struct halide_buffer_int32_t*) malloc(sizeof(struct halide_buffer_int32_t));
  // square(inp_buf, 0, 256, 1, 0, 256, 256, square_buf);
  int xmin = 5;

  struct halide_dimension_t d_h[1] = {{xmin, 256, 1, 0}};
  struct halide_shape shape2_h = {1, d_h};
  struct halide_dimension_t d2_h[2] = {{xmin, 256, 1, 0}, {1, 256, 256, 0}};
  struct halide_shape shape_h = {2, d2_h};
  inp_buf_h->shape = shape2_h;
  square_buf_h->shape = shape_h;
  inp_buf_h->host = (int*) malloc(sizeof(int) * 256 * 256);
  square_buf_h->host = (int*) malloc(sizeof(int) * 256);
  int* inp2 = (int*) inp_buf_h->host;
  int* square2 = (int*) square_buf_h->host;

  for(int x=xmin;x<xmin+256;x++)
   for(int y=xmin;y<xmin+256;y++){
    inp[(x-xmin)*256 + (y-xmin)] = x + y; // Initialize input buffer
    inp2[(x-xmin)*256 + (y-xmin)] = x + y; // Initialize input buffer
   }

  pipeline(inp_buf, square_buf);
  square(inp_buf_h, 0, 256, 1, 0, 256, 256, square_buf_h);
  // {inp,
  // inp_minx, inp_extentx, inp_stridex, inp_miny, inp_extenty, inp_stridey}

  // Check output

  for(int x=xmin;x<xmin+256;x++){
   int expected = (x + x) * (x + x);
   if(square1[x-xmin] != expected){
    printf("Normal: Error at index %d: expected %d, got %d\n", x, expected, square1[x-xmin]);
   }
   if(square2[x-xmin] != expected){
    printf("Halide: Error at index %d: expected %d, got %d\n", x, expected, square2[x-xmin]);
   }
  }
  free(inp_buf);
  free(square_buf);
}