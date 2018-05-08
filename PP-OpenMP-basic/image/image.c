#include <stdio.h>
#include <omp.h>

#define THREAD_NUM 16

int main(void) {

    int ah, aw, bh, bw;
    while( scanf("%d %d %d %d", &ah, &aw, &bh, &bw) == 4 ) {

        int A[ah][aw], B[bh][bw];

        // input matrix
        for ( int h = 0; h < ah; h++ )
            for ( int w = 0; w < aw; w++ )
                scanf("%d", &A[h][w]);
        for ( int h = 0; h < bh; h++ )
            for ( int w = 0; w < bw; w++ )
                scanf("%d", &B[h][w]);

        int min[THREAD_NUM], minx[THREAD_NUM], miny[THREAD_NUM];
        for ( int t = 0; t < THREAD_NUM; t++ ) {
            min[t] = 2147483647;
            minx[t] = -1;
            miny[t] = -1;
        }

        omp_set_num_threads(THREAD_NUM);
        #pragma omp parallel for
        for ( int i = 0; i <= (ah-bh); i++ ) {
            for ( int j = 0; j <= (aw-bw); j++ ) {
                int diff = 0;
                for ( int m = 0; m < bh; m++ )
                    for ( int n = 0; n < bw; n++ )
                        diff += (A[i+m][j+n] - B[m][n]) * (A[i+m][j+n] - B[m][n]);
                int index = omp_get_thread_num();
                if ( diff < min[index] ) {
                    min[index] = diff;
                    minx[index] = i;
                    miny[index] = j;
                }
            }
        }

        int ans=-1, real_min = 2147483647;
        for ( int t = 0; t < THREAD_NUM; t++ ) {
            if( min[t] < real_min ) {
                real_min = min[t];
                ans = t;
            }
        }

        printf("%d %d\n", minx[ans]+1, miny[ans]+1);
    }

    return 0;
}