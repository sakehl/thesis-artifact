#include "Halide.h"
#include <stdio.h>

using namespace Halide;
using std::vector;

int main(int argc, char *argv[]) {
Halide::Target target = Halide::Target()
.with_feature(Halide::Target::NoAsserts)
.with_feature(Halide::Target::NoBoundsQuery)
.with_feature(Halide::Target::NoRuntime)
;

{
ImageParam inp = ImageParam(Int(32), 2);
Func blur_x, blur_y; Var x, y;
inp = ImageParam(type_of<int>(), 2, "inp");
blur_x = Func("blur_x"); blur_y = Func("blur_y");
x = Var("x"); y = Var("y");

/* Halide algorithm */
blur_x(x, y) = (inp(x, y) + inp(x+1, y) + inp(x+2, y)) / 3;
blur_x.ensures(blur_x(x, y) == (inp(x, y) + inp(x+1, y) + inp(x+2, y)) / 3); //~|\label{line:blur_x-ens}|
blur_y(x, y) = (blur_x(x, y) + blur_x(x, y+1) + blur_x(x, y+2)) / 3;
blur_y.ensures(blur_y(x, y) == ((inp(x, y) + inp(x+1, y) + inp(x+2, y)) / 3 + /*@~@*/ (inp(x, y+1) + inp(x+1, y+1) + inp(x+2, y+1)) / 3 + (inp(x, y+2) + /*@~~~~~~~~@*/ inp(x+1, y+2) + inp(x+2, y+2)) / 3) / 3); //~|\label{line:blur_y-ens}|

Param<int> nx, ny;
nx = Param<int>("nx"), ny = Param<int>("ny");
OutputImageParam out = blur_y.output_buffer();
/* Pipeline annotations */
Annotation pre = requires(inp.dim(0).min()==0 && inp.dim(0).extent()==nx+2 && /*@~~@*/inp.dim(1).min()==0 && inp.dim(1).extent()==ny+2 && out.dim(0).min()==0 && out.dim(0).extent()==nx && out.dim(1).min()==0 && out.dim(1).extent()==ny); //~|\label{line:pipeline-1}|
Annotation post = ensures(forall({x,y}, 0<=x && x<nx && 0<=y && y<ny, /*@~~~~~~~@*/ trigger(blur_y(x, y)) == ((inp(x, y) + inp(x+1, y) + inp(x+2, y)) / 3   + /*@~~~@*/(inp(x, y+1) + inp(x+1, y+1) + inp(x+2, y+1)) / 3 + (inp(x, y+2) + /*@~~~~~~~@*/ inp(x+1, y+2) + inp(x+2, y+2)) / 3) / 3)); //~|\label{line:pipeline-2}|
/* Front-end verification */
blur_y.front_end_verification("blur.pvl", {inp,nx,ny}, {pre, post}); //~|\label{line:blur-front-end}|
  

/* Schedule */
Var xi, yi, yo;
xi = Var("xi"); yi = Var("yi"); yo = Var("yo");
blur_y
 .split(y, yo, yi, 8, TailStrategy::GuardWithIf)
 .parallel(yo)
 .split(x, x, xi, 2, TailStrategy::GuardWithIf)
 .unroll(xi);
 
blur_x
 .store_at(blur_y, yo)
 .compute_at(blur_y, yi)
 .split(x, x, xi, 2, TailStrategy::GuardWithIf)
 .unroll(xi);
 

/* Bounding the dimensions */
{
 int nxx = 1920; 
 int nyy = 1024;
 out.dim(0).set_bounds(0, nxx);
 out.dim(1).set_bounds(0, nyy);
 out.dim(1).set_stride(nxx);
 inp.dim(0).set_bounds(0, nxx+2);
 inp.dim(1).set_bounds(0, nyy+2);
 inp.dim(1).set_stride(nxx+2);
/* Back-end verification */
blur_y.compile_to_c("blur.c", {inp,nx,ny}, {pre, post}//~);|\label{line:blur-back-end}|











,"haliver_blur", target);blur_y.print_loop_nest();
}
}


{
ImageParam inp; Func count; Var x;
RDom r(0,10);
count = Func("count");
inp = ImageParam(type_of<int>(), 2, "inp"); r = RDom(0, 10, "r"); x = Var("x");
int n = 1024;

count(x) = 0;//~|\label{line:count-pure-def}|
count.ensures(count(x) == 0);
count(x) = select(inp(x, r) > 0, count(x)+1, count(x));//~|\label{line:reduction}|
count.invariant(0<=count(x) && count(x)<=r);//~|\label{line:reduction-inv}|
count.ensures(0<=count(x) && count(x)<=10);//~|\label{line:reduction-ens}|

OutputImageParam count_out = count.output_buffer();
count_out.dim(0).set_bounds(0, n);
inp.dim(0).set_bounds(0, n);
inp.dim(1).set_bounds(0, n);
inp.dim(1).set_stride(n);

count.front_end_verification("count.pvl", {inp}, {}); //~|\label{line:blur-front-end}|

std::vector<Annotation> pipeline_annotations = {};
count.compile_to_c("haliver_count.c", {inp}, pipeline_annotations
    , "haliver_count", target
  );
}

{
}
}