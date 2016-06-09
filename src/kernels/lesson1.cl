__kernel void square(global float *buffer)
{
	int i = get_global_id(0);
	buffer[i]= buffer[i]*buffer[i];
}

