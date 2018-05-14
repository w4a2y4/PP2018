/* header */
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
 
#define SIDE 2010
 
#define nLiveNeighbor(A, i, j) \
    A[i + 1][j] + A[i - 1][j] + A[i][j + 1] + \
    A[i][j - 1] + A[i + 1][j + 1] + A[i + 1][j - 1] + \
    A[i - 1][j + 1] + A[i - 1][j - 1]
 
/* print */
void print(char A[SIDE][SIDE], int n) {
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) 
            A[i][j] += 48;
        A[i][n+1] = '\0';
        puts(A[i]+1);
    }
}
// char A[SIDE][SIDE], B[SIDE][SIDE], c;
/* main */
int main()
{
    char A[SIDE][SIDE]={0}, B[SIDE][SIDE]={0};
 
    int n, generation;
    scanf("%d%d\n", &n, &generation);
    for (int i = 1; i <= n; i++){
        scanf("%s", A[i]+1);
        for (int j = 1; j <= n; j++)
            A[i][j] -= 48;
    }
 
    /* generation */
    #pragma omp parallel
    for (int g = 0; g < generation; g+=2 ) {
 
        #pragma omp for  /*  from A to B */
        for (int i = 1; i <= n; i++)
            for (int j = 1; j <= n; j++) {
                int nln = nLiveNeighbor(A, i, j);
                B[i][j] = (nln == 3) || ( A[i][j] && nln == 2 );
            }
 
        if ( g >= generation ) break;
 
        #pragma omp for  /*  from B to A */
        for (int i = 1; i <= n; i++)
            for (int j = 1; j <= n; j++) {
                int nln = nLiveNeighbor(B, i, j);
                A[i][j] = (nln == 3) || ( B[i][j] && nln == 2 );
            }
    }
 
    /* printcell */
    if (generation % 2) print(B, n);
    else print(A, n);
 
    return 0;
}