__kernel void add(unsigned int key1, unsigned int key2, __global unsigned int *C, unsigned int N) 
{
    unsigned int id = get_global_id(0);
    unsigned int sum = 0;
    for( int i = 0; i < 32; i++ ) {
        unsigned int idx = id * 32 + i;
        if( idx >= N ) break;
        unsigned int rta = (idx << (key1&31)) | (idx >> (32-(key1&31)));
        unsigned int A = ( rta + key1 ) ^ key1;
        unsigned int rtb = (idx << (key2&31)) | (idx >> (32-(key2&31)));
        unsigned int B = ( rtb + key2 ) ^ key2;
        sum += A * B;
    }
 
    C[id] = sum;
}