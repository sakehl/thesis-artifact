#include <stdlib.h>
void test(){
/*@unique<1>@*/ int*  xs1; // Integers stored in this pointer have the unique number 1. |\label{lst:unique-xs1}|
int /*@unique<1>@*/*  xs2; // This is the same type as above |\label{lst:unique-xs2}|
xs1 = xs2; // Allowed: both have the same unique number for its integers |\label{lst:unique-assignx1x2}|
int /*@unique<2>@*/*  xs3; // This int pointer has a different unique number |\label{lst:unique-xs3}|
//~xs1 = xs3; // Not Allowed: the pointers have different uniquness numbers |\label{lst:unique-assignx1x3}|
/*@unique<1>@*/ int a; // Unique is not used here, since plain integers are no heap location|\label{lst:unique-a}|
// We take the adress of the variable `a`, meaning it is now tracked as a heap variable
// Thus now the uniqueness (unique<1>) number does matter.
xs1 = &a; //~|\label{lst:querry-a}|
int* /*@unique<1>@*/ ys; // This uniqueness information is also not used |\label{lst:unique-ys}|
int* /*@unique<1>@*/ * zs; // This specifies that the content of zs has a unique number 1 |\label{lst:unique-zs}|
//~xs1 = *zs; // Not allowed: *zs values do not have an unique number |\label{lst:unique-assignx1z}|
}

struct s1 { //~|\label{lst:unique-struct-1}|
 /*@unique<1>@*/ int x; // This member has unique number 1
 /*@unique<1>@*/ int* xs1; // The values of x1 have unique number 1
 int* /*@unique<1>@*/ xs2; // xs2 has unique number 1, but not its values
}; //~|\label{lst:unique-struct-2}|


void test2(){
/*@unique<1>@*/ int*  xs1 = (/*@unique<1>@*/ int*) malloc(sizeof(int)); //@assume xs1 != NULL;
int /*@unique<1>@*/*  xs2;
int /*@unique<2>@*/*  xs3 = (/*@unique<2>@*/ int*) malloc(sizeof(int)); //@assume xs3 != NULL;
int* /*@unique<1>@*/ * zs;
struct s1 b; //~|\label{lst:unique-b}|
xs1[0] = xs3[0]; // Allowed:  these are setting the concrete integers values
b.xs1 = xs1; // Allowed: the pointers have same uniqueness
//~b.xs2 = xs1; // Not allowed: the pointers do not have the same uniqueness
zs = &(b.xs2); // Allowed: After taking the address, the uniqueness is now tracked
}

struct v {
 int n;
 int* xs;
};
void test3(){
  /*@unique<1>@*/ int*  xs1;
struct v a; // A normal struct stored in variable a
//~unique_field<n,1>  struct v b; // This struct has its member n stored uniquely
/*@unique_pointer_field<xs,1>@*/  struct v c; // The field xs is now unique<1> int *
//~a.xs = xs1; // Not allowed: since the uniqueness types differ
c.xs = xs1; // Allowed: c has altered the uniqueness of field xs
//~xs1 = &(b.n); // Allowed
}

// This is some previously defined sorting function
void sort(int* xs, int len){
  //~... // Some implementation
}
//~
/*@ context xs != NULL && ys != NULL && \pointer_length(xs)>1 && \pointer_length(ys)>1;
context Perm(xs[0], 1\1) ** Perm(ys[0], 1\1);@*/
//~ ... // Some contract
int f(/*@unique<1>@*/int* xs, int* ys, int n){
  sort(xs, n);//~|\label{lst:modulo-uniqueness}|
  sort(ys, n);
  return xs[0] - ys[0];
}

/*@ context_everywhere xs!=NULL && ys!=NULL && res!=NULL;
 context_everywhere \pointer_length(xs) == n && \pointer_length(ys) == n &&
  \pointer_length(res) == n;
 context_everywhere (\forall* int i; 0<=i&&i<n; Perm(xs[i], read) ** Perm(ys[i], read)
  ** Perm(res[i], write));@*/
void sub(/*@unique<1>@*/ int* xs, /*@unique<2>@*/ int* ys, int* res, int n){
/*@  loop_invariant 0<=i&&i<=n;@*/
 for(int i=0;i<n;i++){
  res[i] = xs[i] - ys[i];
}}

/*@ context a!=NULL && b!=NULL && c!=NULL;
 context \pointer_length(a)==n && \pointer_length(b)==n && \pointer_length(c)==n;
 context (\forall* int i; 0<=i&&i<n; Perm(a[i], read) ** Perm(b[i], read) **
  Perm(c[i], write));@*/
void f(int* a, int* b, /*@unique<1>@*/ int* c, int n){
 // This is fine; sub treats a and b as separate memory locations,
 // even if they can potentially overlap in the called context
 sub(a, b, c, n);//~|\label{lst:unique-call-1}|
 // Here the first and second argument explicitly overlap
 // However sub cannot use the information that they are the same.
 sub(a, a, c, n);//~|\label{lst:unique-call-2}|
}

/*@ context_everywhere xs!=NULL && ys!=NULL && res!=NULL;
 context_everywhere \pointer_length(xs) == n && \pointer_length(ys) == n &&
  \pointer_length(res) == n;
 context_everywhere (\forall* int i; 0<=i&&i<n; Perm(xs[i], read) ** Perm(ys[i], read)
  ** Perm(res[i], write));@*/
void sub(int* xs, int* ys, /*@unique<1>@*/ int* res, int n){
  /*@loop_invariant 0<=i&&i<=n;@*/
 for(int i=0;i<n;i++){
  res[i] = xs[i] - ys[i];
 }
}

/*@ context xs!=NULL && \pointer_length(xs) > 0 ** Perm(*xs, write);
 context ys!=NULL && \pointer_length(ys) > 0 ** Perm(*ys, write);
 ensures *ys == \old(*xs) && *xs == \old(*ys); @*/
void swap(int** xs, int** ys){
  int* temp = *xs;
  *xs = *ys;
  *ys = temp;
}

void g(/*@ unique<1>@*/ int* a, int* b, int* c, int n){
    // This is not allowed; this violates that a and b could
    // never point to the same heap location
    //~swap(&a, &b);//~|\label{lst:unique-call-4}|
}

/*@ requires a != NULL && b != NULL;
 requires \pointer_length(a) == 1 && \pointer_length(b) == 1;
 requires Perm(a[0], 1\2) ** Perm(b[0], 1\2);
 requires \polarity_dependent(perm(a[0])== 1\1, perm(a[0])== 1\1 );
 ensures a == b; @*/
void f(int* a, int* b){
}