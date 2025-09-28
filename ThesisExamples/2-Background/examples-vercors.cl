#include <opencl.h>

/*@ context get_work_dim() == 1 && get_local_size(0) == 32;
 context in != NULL && \pointer_length(in) == n+2;
 context out != NULL && \pointer_length(out) == n;
 context n == get_num_groups(0)*get_local_size(0);
 context Perm({:in[\gtid]:}, read) ** Perm({:in[\gtid+1]:}, read) ** 
  Perm({:in[\gtid+2]:}, read);
 context Perm({:out[\gtid]:}, write); //~|\label{ex14:write}|
 ensures {:out[\gtid]:} == (in[\gtid] + in[\gtid+1] + in[\gtid+2])/3; @*/
__kernel void blur(__global int* in, __global int* out, int n) {
 int gtid = get_global_id(0);
 out[gtid] = (in[gtid] + in[gtid+1] + in[gtid+2]) / 3;
}