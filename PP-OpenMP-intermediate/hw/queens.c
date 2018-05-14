#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "omp.h"
#define N 17

int n;
char board[N][N];

int ok(int position[], int c, int r) {
    if ( board[r][c] == '*' ) return 0;
    for (int i = 0; i < c; i++)
        if (position[i] == r || (abs(r - position[i]) == c - i))
            return 0;
    return 1;
}

int setQueen( int positions[n], int c ) {

    // base case
    if ( c >= n ) return 1;

    int ans = 0;
    for ( int r = 0; r < n; r++ ) {
        if ( ok(positions, c, r)) {
            positions[c] = r;
            ans += setQueen( positions, c+1);
        }
    }

    return ans;
}

int main(void) {

    int cnt = 1;
    while( scanf("%d", &n) == 1 ) {

        // read input
        for ( int i = 0; i < n; i++ )
            scanf("%s", board[i]);

        int positions[n], ans = 0;
        #pragma omp parallel
        {

        #pragma omp for
        for ( int i = 0; i < n; i++ )
            positions[i] = 0;

        #pragma omp for private(positions) schedule(guided, 16) collapse(4) reduction(+:ans)
        for ( int i = 0; i < n; i++ ) {
            for ( int j = 0; j < n; j++ ) {
                for ( int k = 0; k < n; k++ ) {
                    for ( int l = 0; l < n; l++ ) {
                        positions[0] = i; 
                        positions[1] = j;
                        positions[2] = k;
                        positions[3] = l;
                        if ( ok(positions, 0, i) && ok(positions, 1, j) && ok(positions, 2, k) && ok(positions, 3, l)) {
                            ans += setQueen( positions, 4);
                        }
                    }
                }
            }
        }
        }

        printf("Case %d: %d\n", cnt++, ans);
    }

    return 0;
}