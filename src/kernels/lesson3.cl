__kernel void sum_step(global float *A, unsigned N)
{
	unsigned i = get_global_id(0);
    A[i] += A[i+N];
}

// vim:filetype=c:
