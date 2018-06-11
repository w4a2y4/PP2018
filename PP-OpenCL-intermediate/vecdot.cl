#define CHUNK 1024
#define SIZE 256
#define uint32_t unsigned int

static inline uint32_t rotate_left(uint32_t x, uint32_t n) {
    return  (x << n) | (x >> (32-n));
}
static inline uint32_t encrypt(uint32_t m, uint32_t key) {
    return (rotate_left(m, key&31) + key)^key;
}

__kernel void add(unsigned int key1, unsigned int key2, __global unsigned int *C, unsigned int N) 
{
    __local int buff[CHUNK];

    unsigned int global_id = get_global_id(0);
    unsigned int local_id = get_local_id(0);
    unsigned int group_id = get_group_id(0);

    // generate vector in local memory
    int start = global_id * SIZE;
    buff[local_id] = 0;
    int end = (global_id+1) * SIZE < N ? (global_id+1) * SIZE : N;
    for( int idx = start; idx < end; idx ++ )
        buff[local_id] += encrypt(idx, key1) * encrypt(idx, key2);

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