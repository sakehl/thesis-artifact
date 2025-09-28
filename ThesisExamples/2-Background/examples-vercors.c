/*@ yields int idx; //~|\label{ex14:contract-1}|
 requires n > 0;
 context_everywhere xs != NULL && \pointer_length(xs) == n; //~|\label{ex14:context-1}|
 context_everywhere (\forall* int j; 0<=j && j<n; Perm(xs[j], read)); //~|\label{ex14:context-2}|
 ensures idx >= 0 && idx < n && xs[idx] == \result;
 ensures (\forall int j; 0<=j && j<n; xs[j] <= \result); //~|\label{ex14:forall}|
 decreases; @*/ //~|\label{ex14:contract-2}|
int max_element(int* xs, int n) {
  int max = xs[0];
  /*@ ghost idx = 0; @*/ //~|\label{ex14:ghost-1}|
  
 /*@ loop_invariant 1 <= i && i<=n; //~|\label{ex14:loop-contract-1}|
  loop_invariant idx >= 0 && idx < n && max == xs[idx];
  loop_invariant (\forall int j; 0<=j && j<i; xs[j] <= max); //~|\label{ex14:invariant}|
  decreases n-i; @*/ //~|\label{ex14:loop-contract-2}|
 for(int i=1; i<n; i++){
  if(xs[i] > max) {
    max = xs[i];
    /*@ ghost idx = i; @*/ //~|\label{ex14:ghost-2}|
  }
 }
 return max;
}

/*@ requires n > 0;
 context xs != NULL ** \pointer_length(xs) == n **
  (\forall* int j; 0<=j && j<n; Perm(xs[j], read)); @*/
void call(int* xs, int n) {
 //@ ghost int result_idx;
 int max = max_element(xs, n) /*@ yields {result_idx=idx} @*/;
}

/*@ context_everywhere xs != NULL && \pointer_length(xs) == n;
 context (\forall* int j; 0<=j && j<n; Perm(xs[j], write)); 
 ensures (\forall int j; 0<=j && j<n; xs[j] == \old(xs[j])+1); @*/
void incr_one(int* xs, int n) {
 #pragma omp parallel for
 for(int i=0; i<n; i++)
 /*@ context Perm(xs[i], write);
  ensures xs[i] == \old(xs[i]) + 1; @*/ {
  xs[i] += 1;
 }
}