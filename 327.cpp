#include <stdio.h>
#include <math.h>

double log2(double x) {
    return log(x)/log(2.0);
}

int main() {

    long long int N;
    long long int num[1000000];
    int P = 10;
    long long int critical[P][P];

    scanf("%lld", &N);
    for ( int i = 0; i < N; i++ ) scanf("%lld", &num[i]);

    // sequential fo each processor
    for ( int p = 0; p < P; p++ ) {
        for ( int i = p*N/P + 1; i < (p+1)*N/P; i++ )
            num[i] += num[i-1];
        critical[0][p] = num[ (p+1) * N/P - 1 ];
    }

    int base = 1, j;
    for ( j = 0; j < log2(P); j++ ) {
        for ( int p = 0; p < P; p++ ) {
            critical[j+1][p] = critical[j][p];
            if ( p - base >= 0 )
                critical[j+1][p] += critical[j][p - base];
        }
        base *= 2;
    }

    for ( int p = 0; p < P; p++ ) {
        critical[j][p] -= num[ (p+1) * N/P - 1 ];
        for ( int i = p*N/P; i < (p+1)*N/P; i++ ) {
            num[i] += critical[j][p];
        }
    }

    // print
    printf("%lld", num[0]);
    for ( int i = 1; i < N; i++ )
        printf(" %lld", num[i]);
    printf("\n");

    return 0;
}