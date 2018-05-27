#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "omp.h"
 
int max( int a, int b ) {
    if ( a > b ) return a;
    return b;
}
 
int main( void ) {
 
    int n, m;
    scanf("%d%d", &n, &m);
 
    int w[n], v[n];
    for ( int i = 0; i < n; i++)
        scanf("%d%d", &w[i], &v[i]);
 
    int c[2][m+1];
    memset(c, 0, sizeof(c));
 
    #pragma omp parallel
    {
 
        // initialize
        #pragma omp for schedule(guided, 1)
        for ( int j = w[0]; j <= m; j++ ) {
            c[0][j] = v[0];
        }
 
        // recursion
        int now = 1, prev = 0;
        for ( int i = 1; i < n; i++ ) {
            #pragma omp for schedule(guided, 4)
            for ( int j = 0; j <= m; j++ ) {
                if( j < w[i] || c[prev][j] > (c[prev][j - w[i]] + v[i]) )
                    c[now][j] = c[prev][j];
                else c[now][j] = c[prev][j - w[i]] + v[i];
            }
            now = prev;
            prev = ( now + 1 ) % 2;
        }
 
    }
 
    // terminate
    printf("%d\n", c[(n-1)%2][m]);
 
    return 0;
}