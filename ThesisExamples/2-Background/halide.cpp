#include "Halide.h"
#include <stdio.h>

using namespace Halide;

int main(int argc, char *argv[]) {
Halide::Target target = Halide::Target()
.with_feature(Halide::Target::NoAsserts)
.with_feature(Halide::Target::NoBoundsQuery)
.with_feature(Halide::Target::NoRuntime)
;
{
Func gradient; //~|\label{line:halide-func}|
Var x, y; //~|\label{line:halide-var}|
gradient = Func("gradient"); x = Var("x"); y = Var("y");
gradient(x,y) = x+y; //~|\label{line:halide-def}|

Param<int> nx("nx"), ny("ny");
OutputImageParam gradient_out = gradient.output_buffer();
gradient_out.dim(0).set_bounds(1, nx);
gradient_out.dim(1).set_bounds(2, ny);
gradient_out.dim(1).set_stride(nx);
gradient.compile_to_c("haliver_gradient.c" , {nx,ny}, {}
  , "gradient", target
);
gradient.print_loop_nest();
}

{
Func reflect, square; Var x, y;
ImageParam inp = ImageParam(Int(32), 2); //~|\label{line:halide-input}|

reflect = Func("reflect"); square = Func("square");
x = Var("x"); y = Var("y");
inp = ImageParam(Int(32), 2, "inp");

reflect(x,y) = inp(y,x);
square(x) = reflect(x,x)*reflect(x,x);
reflect.compute_root(); //~|\label{line:halide-compute-root}|

Param<int> inp_minx("inp_minx"),
  inp_extentx("inp_extentx"),
  inp_stridex("inp_stridex"),
  inp_miny("inp_miny"),
  inp_extenty("inp_extenty"),
  inp_stridey("inp_stridey");
OutputImageParam square_out = square.output_buffer();
square_out.dim(0).set_bounds(inp_minx, inp_extentx);

inp.dim(0).set_bounds(inp_minx, inp_extentx);
inp.dim(1).set_bounds(inp_miny, inp_extenty);
inp.dim(1).set_stride(inp_stridey);

square.compile_to_c("pipeline.c", {inp//~});|\label{line:halide-compile}|


,inp_minx, inp_extentx, inp_stridex, inp_miny, inp_extenty, inp_stridey},
{}, "square", target);
square.compile_to_header("pipeline.h", {inp,
  inp_minx, inp_extentx, inp_stridex, inp_miny, inp_extenty, inp_stridey},
  "square", target);
square.print_loop_nest();
}

{
Func g; Var x, y;
g = Func("g"); x = Var("x"); y = Var("y");
g(x,y) = x+y;//~|\label{line:halide-def-1}|
g(7,42) = 42;//~|\label{line:halide-def-2}|
g(y+1,y) = g(y-1,y)+g(y,y);//~|\label{line:halide-def-3}|


Param<int> nx("nx"), ny("ny");
OutputImageParam g_out = g.output_buffer();
g_out.dim(0).set_bounds(0, nx);
g_out.dim(1).set_bounds(0, ny);
g_out.dim(1).set_stride(nx);
g.compile_to_c("haliver_g.c" , {nx,ny}, {}
  , "g", target
);
g.print_loop_nest();
}

{
Func prefixsum; Var x;
ImageParam inp = ImageParam(Int(32), 1);

prefixsum = Func("prefixsum"); x = Var("x");
inp = ImageParam(Int(32), 1, "inp");

RDom r(1, 9); //~|\label{line:rdom}|
prefixsum(x) = inp(x);
prefixsum(r) = prefixsum(r-1); //~|\label{line:reduction-update}|
Param<int> nx("nx"), ny("ny");
OutputImageParam prefixsum_out = prefixsum.output_buffer();
prefixsum_out.dim(0).set_bounds(0, nx);
inp.dim(0).set_bounds(0, nx);
prefixsum.compile_to_c("haliver_prefixsum.c" , {inp,nx,ny}, {}
  , "prefixsum", target
);
prefixsum.print_loop_nest();
}

{
Func gradient; //~|\label{line:halide-func}|
Var x, y; //~|\label{line:halide-var}|
gradient = Func("gradient"); x = Var("x"); y = Var("y");
gradient(x,y) = x+y; //~|\label{line:halide-def}|

Param<int> nx("nx"), ny("ny");
OutputImageParam gradient_out = gradient.output_buffer();
gradient_out.dim(0).set_bounds(1, nx);
gradient_out.dim(1).set_bounds(2, ny);
gradient_out.dim(1).set_stride(nx);
gradient.compile_to_c("haliver_gradient_fast.c" , {nx,ny}, {}
  , "gradient", target
);
Var x_out, y_out, x_in, y_in, tile_index;
Var x_in_out, y_in_out, x_vectors, y_pairs;
x_out=Var("x_out"); y_out=Var("y_out"); x_in=Var("x_in"); y_in=Var("y_in"); tile_index=Var("tile_index");
x_in_out=Var("x_in_out"); y_in_out=Var("y_in_out"); x_vectors=Var("x_vectors"); y_pairs=Var("y_pairs");
gradient
 .split(x, x_out, x_in, 64, TailStrategy::RoundUp)
 .split(y, y_out, y_in, 64, TailStrategy::RoundUp)
 .reorder(x_in, y_in, x_out, y_out)
 .fuse(x_out, y_out, tile_index)
 .parallel(tile_index)
 .split(x_in, x_in_out, x_vectors, 4, TailStrategy::RoundUp)
 .split(y_in, y_in_out, y_pairs, 2, TailStrategy::RoundUp)
 .reorder(x_vectors, y_pairs, x_in_out, y_in_out)
 .vectorize(x_vectors)
 .unroll(y_pairs);
gradient.print_loop_nest();
}

{
  Func producer, consumer;
  Var x, y;
  producer = Func("producer"); consumer = Func("consumer"); x = Var("x"); y = Var("y");
  producer(x, y) = sin(x * y);
  consumer(x, y) = (producer(x, y) +
   producer(x, y + 1) +
   producer(x + 1, y) +
   producer(x + 1, y + 1)) / 4;

  // Bounding the dimensions
 int nx = 20;
 int ny = 10;

 OutputImageParam consumer_out = consumer.output_buffer();
 consumer_out.dim(0).set_bounds(0, nx);
 consumer_out.dim(1).set_bounds(0, ny);
 consumer_out.dim(1).set_stride(nx);

/* Schedule 1 */
producer.compute_inline();
consumer.print_loop_nest();
/* Schedule 2 */
producer.compute_at(consumer, y);
consumer.print_loop_nest();
/* Schedule 3 */
producer.store_root().
 compute_at(consumer, x);
consumer.print_loop_nest();
}

{
ImageParam inp = ImageParam(Int(32), 2);
Func blur_x, blur_y; Var x, y;
inp = ImageParam(type_of<int>(), 2, "inp");
blur_x = Func("blur_x"); blur_y = Func("blur_y");
x = Var("x"); y = Var("y");

/* Halide algorithm */
blur_x(x, y) = (inp(x, y)+inp(x+1, y)+inp(x+2, y))/3;
blur_x.ensures(blur_x(x, y) ==
 (inp(x, y) + inp(x+1, y) + inp(x+2, y)) / 3);

blur_y(x, y) = (blur_x(x, y) + blur_x(x, y+1) + blur_x(x, y+2))/3;
blur_y.ensures(blur_y(x, y) ==
 ((inp(x, y)+inp(x+1, y)+inp(x+2, y))/3
  +(inp(x, y+1)+inp(x+1, y+1)+inp(x+2, y+1))/3
  +(inp(x, y+2)+inp(x+1, y+2)+inp(x+2, y+2))/3)/3);

/* Pipeline annotations */
OutputImageParam blur_y_out = blur_y.output_buffer();
{
Param<int> n;
std::vector<Annotation> pipeline_annotations = {
 requires(
  inp.dim(0).min() == blur_y_out.dim(0).min() &&
  inp.dim(1).min() == blur_y_out.dim(1).min() &&
  inp.dim(0).extent() == blur_y_out.dim(0).extent()+2 &&
  inp.dim(1).extent() == blur_y_out.dim(1).extent()+2 &&
  inp.dim(1).stride() == inp.dim(0).extent() &&
  blur_y_out.dim(1).stride() == blur_y_out.dim(0).extent()
 ),
 ensures(forall({x,y}, 0 <= x && x < n && 0 <= y && y < n,
  trigger(blur_y(x, y)) ==
   ((inp(x, y) + inp(x+1, y) + inp(x+2, y)) / 3
   + (inp(x, y+1) + inp(x+1, y+1) + inp(x+2, y+1)) / 3
   + (inp(x, y+2) + inp(x+1, y+2) + inp(x+2, y+2)) / 3) / 3
 ))};

blur_y.translate_to_pvl("haliver.pvl", {inp}, pipeline_annotations);
}

/* Schedule */
Var xi, yi, yo;
xi = Var("xi"); yi = Var("yi"); yo = Var("yo");
blur_y
 .split(y, y, yi, 8, TailStrategy::GuardWithIf)
 .parallel(y)
 .split(x, x, xi, 2, TailStrategy::GuardWithIf)
 .unroll(xi)
 ;
blur_x
 .store_at(blur_y, y)
 .compute_at(blur_y, yi)
 .split(x, x, xi, 2, TailStrategy::GuardWithIf)
 .unroll(xi)
 ;

  // Bounding the dimensions
 int n = 1024;
 
 blur_y_out.dim(0).set_bounds(0, n);
 blur_y_out.dim(1).set_bounds(0, n);
 blur_y_out.dim(1).set_stride(n);
 inp.dim(0).set_bounds(0, n+2);
 inp.dim(1).set_bounds(0, n+2);
 inp.dim(1).set_stride(n+2);

std::vector<Annotation> pipeline_annotations = {
 requires(
  inp.dim(0).min() == blur_y_out.dim(0).min() &&
  inp.dim(1).min() == blur_y_out.dim(1).min() &&
  inp.dim(0).extent() == blur_y_out.dim(0).extent()+2 &&
  inp.dim(1).extent() == blur_y_out.dim(1).extent()+2 &&
  inp.dim(1).stride() == inp.dim(0).extent() &&
  blur_y_out.dim(1).stride() == blur_y_out.dim(0).extent()
 ),
 ensures(forall({x,y}, 0 <= x && x < n && 0 <= y && y < n,
  trigger(blur_y(x, y)) ==
   ((inp(x+2, y+2) + inp(x, y+2) + inp(x+1, y+2)) / 3
   + (inp(x+2, y) + inp(x, y) + inp(x+1, y)) / 3
   + (inp(x+2, y+1) + inp(x, y+1) + inp(x+1, y+1)) / 3) / 3
 ))};

blur_y.compile_to_c("haliver.c" , {inp}, pipeline_annotations
  , "haliver_blur", target
);
}


{
ImageParam inp;
Func count;
Var x; RDom r(0,10);
count = Func("count");
inp = ImageParam(type_of<int>(), 2, "inp");
int n = 1024;

count(x) = 0;
count.ensures(count(x) == 0);
count(x) = select(inp(x, r) > 0, count(x)+1, count(x));
count.invariant(0<=count(x) && count(x)<=r);
count.ensures(0<=count(x) && count(x)<=10);

OutputImageParam count_out = count.output_buffer();
count_out.dim(0).set_bounds(0, n);
inp.dim(0).set_bounds(0, n);
inp.dim(1).set_bounds(0, n);
inp.dim(1).set_stride(n);

std::vector<Annotation> pipeline_annotations = {};
count.compile_to_c("haliver_count.c", {inp}, pipeline_annotations
    , "haliver_count", target
  );
}

{
}
}