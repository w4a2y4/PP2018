#include "matrix.h"

#define MAX_THREAD 6

typedef struct {
    int start;
    int end;
    int n;
} bound;

bound x[MAX_THREAD];

unsigned long (*AA)[2048];
unsigned long (*CC)[2048];
unsigned long D[][2048];

void *mult (void *thread_id) {
    long t = (long)thread_id;
    bound bd = x[t];

    for (int i = bd.start; i < bd.end; i++) {
        for (int j = 0; j < bd.n; j++) {
            unsigned long sum = 0;    // overflow, let it go.
            for (int k = 0; k < bd.n; k++)
                sum += (*(AA+i))[k] * D[j][k];
            (*(CC+i))[j] = sum;
        }
    }

    pthread_exit(NULL);
}

void multiply(int N, unsigned long A[][2048], unsigned long B[][2048], unsigned long C[][2048]) {

    // transpose B = D
    AA = A;
    CC = C;

    for ( int i = 0; i < N; i++ )
        for ( int j = 0; j < N; j++ )
            D[i][j] = B[j][i];

    // thread
    pthread_t threads[MAX_THREAD];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int remain = N % MAX_THREAD;
    int quotient = N / MAX_THREAD;

    x[0].n = N;
    x[0].start = 0;
    x[0].end = quotient + ( remain > 0 );

    for ( int t = 1; t < MAX_THREAD; t++ ) {
        x[t].n = N;
        x[t].start = x[t-1].end;
        x[t].end = quotient + ( remain > t );
    }

    x[MAX_THREAD].end = N;

    for ( int t = 0; t < MAX_THREAD; t++ )
        pthread_create(&threads[t], &attr, mult, (void *)t);

    for ( int t = 0; t < MAX_THREAD; t++ ) pthread_join(threads[t], NULL);
}