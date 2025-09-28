#include <math.h>

/*@
  requires 0 <= x_1 && x_1<n && 0 <= x_2 && x_2<m;
  ensures \result;
  ensures (x_1+x_2*n) >= 0 && (x_1+x_2*n) < n*m;
  ensures x_1 == (x_1+x_2*n) % n;
  ensures x_2 == (x_1+x_2*n) / n; 
pure bool nonlinear_lemma(int x_1, int x_2 , int n, int m);
*/

/*@
  requires 0 <= x_1 && x_1<n && 0 <= x_2 && x_2<m;
  requires nonlinear_lemma(x_1, x_2, n, m);
  ensures \result == x_1+x_2*n;
  ensures \result >= 0 && \result < n*m;
  ensures x_1 == \result % n;
  ensures x_2 == \result / n; 
  */
/*@ pure @*/ int idx(int x_1, int x_2 , int n, int m){
  return x_1+x_2*n;
}

/*@
  context_everywhere a != NULL && \pointer_length(a) == n*m;
  context_everywhere n > 0 && m > 0;
  context (\forall* int i , int j; 0 <= i && i<n && 0 <= j && j<m;
      Perm( {: &a[idx(i,j,n,m)] :}, write)
    );
  ensures (\forall int x_1, int x_2; 
        0 <= x_1 && x_1<n && 0 <= x_2 && x_2<m && x_1+x_2<10 
          ==> {:a[idx(x_1,x_2,n,m)]:} == x_1+x_2);*/
void f(int n, int m, int* a){
  //@ assert(1.0/0.0 == INFINITY);

    /*@
    loop_invariant 0 <= j && j<=m;
    loop_invariant (\forall* int x_1, int x_2; 
        0 <= x_1 && x_1<n && 0 <= x_2 && x_2<m && x_1+x_2<10 
          ==>  Perm( {: &a[idx(x_1,x_2,n,m)] :}, write));

    loop_invariant (\forall int x_1, int x_2; 
        0 <= x_1 && x_1<n && 0 <= x_2 && x_2<j && x_1+x_2<10 
          ==> {:a[idx(x_1,x_2,n,m)]:} == x_1+x_2);*/
  for(int j=0; j<m; j++){
    /*@
    loop_invariant 0 <= i && i<=n;
    loop_invariant (\forall* int x_1; 
        0 <= x_1 && x_1<n && x_1+j<10 
          ==> Perm( {: &a[idx(x_1,j,n,m)] :}, write));

    loop_invariant (\forall int x_1; 
        0 <= x_1 && x_1<i && x_1+j<10 
          ==> {:a[idx(x_1,j,n,m)]:} == x_1+j);*/
    for(int i=0; i<n; i++){
      if(i+j<10){
        a[idx(i, j, n, m)] = i+j;
      }
    }
  }
}