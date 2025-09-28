#include <immintrin.h>

/*@ context_everywhere n >= 0 && n % 4 == 0;
 context_everywhere a != NULL && \pointer_length(a) == n;
 context_everywhere b != NULL && \pointer_length(b) == n;
 context_everywhere c != NULL && \pointer_length(c) == n;
 context_everywhere (\forall* int i; 0<=i && i<n; Perm({:a[i]:}, read));
 context_everywhere (\forall* int i; 0<=i && i<n; Perm({:b[i]:}, read));
 context_everywhere (\forall* int i; 0<=i && i<n; Perm({:c[i]:}, write));
 ensures (\forall int j; 0 <= j && j<n; {:c[j]:} == a[j] + b[j]); @*/
void vector_add(float *a, float *b, float *c, int n){
/*@  loop_invariant 0<=i && i<=n && i%4 == 0;
  loop_invariant (\forall int j; 0 <= j && j<i; {:c[j]:} == a[j] + b[j]);@*/
 for(int i = 0; i < n; i+=4){
  __m128 a4 = _mm_loadu_ps(a + i);
  __m128 b4 = _mm_loadu_ps(b + i);
  __m128 c4 = a4 + b4;
  _mm_store_ps(c+i, c4);
 }
}

// Example of vector header file used in VerCors
// Definition of float vector of size 4
typedef float __m128 __attribute__ ((__vector_size__ (sizeof(float)*4)));

// Loads 4 floats into a vector of length 4
/*@ requires p != NULL ** \pointer_length(p) >= 4 ** 
  (\forall* int i; 0<=i && i<4; Perm(p[i], read));
 ensures p[0] == \result[0] && p[1] == \result[1] &&
  p[2] == \result[2] && p[3] == \result[3];@*/
/*@pure@*/ __m128 _mm_loadu_ps (float *p);

// Stores a vector of length 4 into a float pointer
/*@ context p != NULL && \pointer_length(p) >= 4 **
  (\forall* int i; 0<=i && i<4; Perm(p[i], write));
 ensures p[0] == a[0] && p[1] == a[1] && p[2] == a[2] && p[3] == a[3];@*/
void _mm_store_ps (float *p, __m128 a);

// Adds two float vectors
/*@pure@*/__m128 _mm_add_ps (__m128 a, __m128 b){ return a + b;}

struct s {int x; int y;};

/*@ requires Perm(v, write);
 ensures Perm(\result, write) ** \result.x == x ** \result.y == \old(v.y);@*/
struct s set_x(struct s v, int x) {
 v.x = x;
 return v;
}

void f(){
 struct s p[1];
 p->x = 5;
 p->y = 10;
 struct s v2 = set_x(*p, 3); 
 /*@assert(p->x == 5 && p->y == 10);@*/ // Since *p is passed by value, it is not changed
 /*@assert(v2.x == 3 && v2.y == 10);@*/ // But the returned valued is changed
}