/*@inline resource array_perm(int* xs, int len, rational p) = xs != NULL ** 
 \pointer_length(xs) == len ** (\forall* int i; 0<=i&&i<len; Perm({:xs[i]:}, p));@*/ //~|\label{lst:many-quant}|

/*@ context_everywhere n > 0;
 context_everywhere array_perm(x0, n, write);
 context_everywhere array_perm(x1, n, 1\2);
 //~... // Similar annotations for x2..x9
 context_everywhere array_perm(x2, n, 1\2);
 context_everywhere array_perm(x3, n, 1\2);
 context_everywhere array_perm(x4, n, 1\2);
 context_everywhere array_perm(x5, n, 1\2);
 context_everywhere array_perm(x6, n, 1\2);
 context_everywhere array_perm(x7, n, 1\2);
 context_everywhere array_perm(x8, n, 1\2);
 context_everywhere array_perm(x9, n, 1\2);
 context_everywhere array_perm(x10, n, 1\2);
 ensures (\forall int j; 0<=j&&j<n; {:x0[j]:} == 
  x1[j]+ x2[j]+ x3[j]+ x4[j]+ x5[j]+ x6[j]+ x7[j]+ x8[j]+ x9[j]+ x10[j]); @*/
int main(int* x0, int* x1, int* x2, int* x3, int* x4, int* x5,//~|\label{line:arrays1}|
 int* x6, int* x7, int* x8, int* x9, int* x10, int n){//~|\label{line:arrays2}|
 /*@loop_invariant 0 <= i && i<= n;
  loop_invariant (\forall int j; 0<=j&&j<i; {:x0[j]:} == 
    x1[j]+ x2[j]+ x3[j]+ x4[j]+ x5[j]+ x6[j]+ x7[j]+ x8[j]+ x9[j]+ x10[j]);@*/
 for(int i=0; i<n;i++){
  x0[i] = 0 +
   x1[i]+ x2[i]+ x3[i]+ x4[i]+ x5[i]+ x6[i]+ x7[i]+ x8[i]+ x9[i]+ x10[i];
 }
}

int main(/*@unique<0>@*/ int* x0, /*@unique<1>@*/ int* x1, /*@unique<2>@*/ int* x2,
 /*@unique<3>@*/ int* x3, /*@unique<4>@*/ int* x4, /*@unique<5>@*/ int* x5,
 /*@unique<6>@*/ int* x6, /*@unique<7>@*/ int* x7, /*@unique<8>@*/ int* x8,
 /*@unique<9>@*/ int* x9, /*@unique<10>@*/ int* x10, int n);