#define CHUNK 1024

__kernel void add(unsigned int key1, unsigned int key2, __global unsigned int *C, unsigned int N) 
{
    __local int buff[CHUNK];

    unsigned int idx = get_global_id(0);
    unsigned int local_id = get_local_id(0);
    unsigned int group_id = get_group_id(0);

    // generate vector in local memory
    if ( idx >= N ) buff[local_id] = 0;
    else {
        unsigned int rta = (idx << (key1&31)) | (idx >> (32-(key1&31)));
        unsigned int rtb = (idx << (key2&31)) | (idx >> (32-(key2&31)));
        buff[local_id] = ( ( rta + key1 ) ^ key1 ) * ( ( rtb + key2 ) ^ key2 );
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    // tree
    int i = CHUNK/2;
    while( i >= 1 && local_id < i ) {
        buff[local_id] += buff[local_id+i];
        barrier(CLK_LOCAL_MEM_FENCE);
        i /= 2;
    }

    if( local_id == 0 )
        C[group_id] = buff[0];
}