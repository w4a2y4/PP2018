#include "matrix.h"
#include <omp.h >

#define MAX_THREAD 4

typedef struct {
    int start;
    int end;
} bound;

bound x[MAX_THREAD];
int n;
unsigned long (*AA)[2048];
unsigned long (*CC)[2048];
unsigned long D[2048][2048];

void mult (int t) {
    bound bd = x[t];

    for (int i = bd.start; i < bd.end; i++) {
        for (int j = 0; j < n; j++) {
            unsigned long sum = 0;    // overflow, let it go.
            for (int k = 0; k < n; k++)
                sum += (*(AA+i))[k] * D[j][k];
            (*(CC+i))[j] = sum;
        }
    }
}

void multiply(int N, unsigned long A[][2048], unsigned long B[][2048], unsigned long C[][2048]) {

    // transpose B = D
    for ( int i = 0; i < N; i++ )
        for ( int j = 0; j < N; j++ )
            D[i][j] = B[j][i];

    AA = A;
    CC = C;
    n = N;

    int remain = N % MAX_THREAD;
    int quotient = N / MAX_THREAD;

    x[0].start = 0;
    x[0].end = quotient + ( remain > 0 );

    for ( int t = 1; t < MAX_THREAD; t++ ) {
        x[t].start = x[t-1].end;
        x[t].end = quotient + ( remain > t );
    }

    x[MAX_THREAD - 1].end = N;

    omp_set_num_threads(MAX_THREAD);
    #pragma omp parallel for
    for ( int t = 0; t < MAX_THREAD; t++ ) {
        mult(omp_get_thread_num());
    }

    return;
}
