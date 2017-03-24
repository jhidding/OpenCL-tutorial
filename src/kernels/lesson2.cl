__kernel void initVectorA(global int *A)
{
    int i = get_global_id(0);
    A[i] = i;
}

__kernel void initVectorB(global int *B, int numElements)
{
    int i = get_global_id(0);
    B[i] = numElements - i;
}

__kernel void sum(global int *C, global int *A, global int *B)
{
    int i = get_global_id(0);
    C[i] = A[i] + B[i];
}

