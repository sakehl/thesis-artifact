#include <opencl.h>

/*@ context get_work_dim() == 1;
 context get_local_size(0) == 32;
 context get_num_groups(0) > 0;
 context a != NULL && b != NULL && c != NULL;
 context \pointer_length(a) == size && \pointer_length(b) == size &&
  \pointer_length(c) == size;
 context size == get_num_groups(0)*get_local_size(0)*4;
 context (\forall* int i; 0<=i && i<4; Perm({:a[\gtid*4+i]:}, read));
 context (\forall* int i; 0<=i && i<4; Perm({:b[\gtid*4+i]:}, read));
 context (\forall* int i; 0<=i && i<4; Perm({:c[\gtid*4+i]:}, write));
 ensures (\forall int i; 0<=i && i<4; {:c[\gtid*4+i]:} == a[\gtid*4+i]+b[\gtid*4+i]);@*/
__kernel void addArrays(__global int* a, __global int* b, __global int* c,
  int size) {
 int tid = get_global_id(0);
 int4 a2 = vload4(tid, a); // Load 4 elements in a vector
 int4 b2 = vload4(tid, b);
 int4 c2 = a2 + b2;
 vstore4(c2, tid, c); // Store the vector at pointer c with offset 4*tid
}