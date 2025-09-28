#include "Halide.h"
#include <stdio.h>

using namespace Halide;

int main(int argc, char *argv[]) {
Halide::Target target = Halide::Target()
.with_feature(Halide::Target::NoAsserts)
.with_feature(Halide::Target::NoBoundsQuery)
;
{
Func f; //~|\label{line:halide-func}
Var x, y; //~|\label{line:halide-var}
f(x,y) = x+y; //~|\label{line:halide-def}

Param<int> nx, ny;
OutputImageParam f_out = f.output_buffer();
f_out.dim(0).set_bounds(0, nx);
f_out.dim(1).set_bounds(0, ny);
f_out.dim(1).set_stride(nx);
f.compile_to_c("haliver_f.c" , {nx,ny}, {}
  , "f", target
);
f.print_loop_nest();
}
{
Func g; //~|\label{line:halide-func}
Var x, y; //~|\label{line:halide-var}
g = Func("g"); x = Var("x"); y = Var("y");
// Buffer<int> inp;
ImageParam inp = ImageParam(Int(32), 2);
Buffer<int> buf = inp.get();
g(x,y) = inp(y,x);
g(x,y) = select(g(x,y) < 0, -g(x,y), g(x,y));


Param<int> nx('nx'), ny('ny');
OutputImageParam g_out = g.output_buffer();
g_out.dim(0).set_bounds(0, nx);
g_out.dim(1).set_bounds(0, ny);
g_out.dim(1).set_stride(nx);
inp.dim(0).set_bounds(0, nx);
inp.dim(1).set_bounds(0, ny);
inp.dim(1).set_stride(nx);
g.compile_to_c("haliver_g.c" , {inp, nx,ny}, {}
  , "g", target
);
g.print_loop_nest();
}

{
Func g, h; Var x, y;
ImageParam inp = 
 ImageParam(Int(32), 2); //~|\label{line:halide-input}|
g(x,y) = inp(y,x);
h(x) = g(x,x)*g(x,x);
g.compute_root();

Param<int> inp_minx('inp_minx'),
  inp_extentx('inp_extentx'),
  inp_stridex('inp_stridex'),
  inp_miny('inp_miny'),
  inp_extenty('inp_extenty'),
  inp_stridey('inp_stridey');
OutputImageParam h_out = g.output_buffer();
h_out.dim(0).set_bounds(0, nx);

inp.dim(0).set_bounds(inp_minx, inp_extentx);
inp.dim(1).set_bounds(inp_miny, inp_extenty);
inp.dim(1).set_stride(inp_stridey);

h.compile_to_c("pipeline", {inp}, {});
}

{
ImageParam inp;
Func blur_x, blur_y;
Var x, y;
inp = ImageParam(type_of<int>(), 2, "inp");
blur_x = Func("blur_x"); blur_y = Func("blur_y");
x = Var("x"); y = Var("y");

/* Halide algorithm */
blur_x(x, y) = (inp(x+2, y)+inp(x, y)+inp(x+1, y))/3; 
blur_x.ensures(blur_x(x, y) ==
 (inp(x+2, y) + inp(x, y) + inp(x+1, y)) / 3); 

blur_y(x, y) = (blur_x(x, y+2) + blur_x(x, y) + blur_x(x, y+1))/3; 
blur_y.ensures(blur_y(x, y) ==
 ((inp(x+2, y+2)+inp(x, y+2)+inp(x+1, y+2))/3
  +(inp(x+2, y)+inp(x, y)+inp(x+1, y))/3
  +(inp(x+2, y+1)+inp(x, y+1)+inp(x+1, y+1))/3)/3);

/* Pipeline annotations */
OutputImageParam blur_y_out = blur_y.output_buffer();
{
Param<int> n;
std::vector<Annotation> pipeline_annotations = {
//  requires(
//   inp.dim(0).min() == blur_y_out.dim(0).min() &&
//   inp.dim(1).min() == blur_y_out.dim(1).min() &&
//   inp.dim(0).extent() == blur_y_out.dim(0).extent()+2 &&
//   inp.dim(1).extent() == blur_y_out.dim(1).extent()+2 &&
//   inp.dim(1).stride() == inp.dim(0).extent() &&
//   blur_y_out.dim(1).stride() == blur_y_out.dim(0).extent()
//  ),
 ensures(forall({x,y}, 0 <= x && x < n && 0 <= y && y < n,
  trigger(blur_y(x, y)) ==
   ((inp(x+2, y+2) + inp(x, y+2) + inp(x+1, y+2)) / 3
   + (inp(x+2, y) + inp(x, y) + inp(x+1, y)) / 3
   + (inp(x+2, y+1) + inp(x, y+1) + inp(x+1, y+1)) / 3) / 3
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