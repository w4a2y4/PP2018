#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <pthread.h>
#include "utils.h"
 
#define MAXN 10000005
#define MAX_THREAD 10
uint32_t prefix_sum[MAXN];
uint32_t part_sum[MAX_THREAD];
int start[MAX_THREAD], end[MAX_THREAD];

uint32_t key;
int n;

int max( int a, int b ) {
    if ( a > b ) return a;
    else return b;
}

void *seqSum (void *thread_id) {
    long t = (long)thread_id;

    if( start[t] == end[t] ) {
        part_sum[t] = 0;
        pthread_exit(NULL);
    }

    prefix_sum[start[t]] = encrypt(start[t], key);
    for ( int i = start[t] + 1; i < end[t]; i++ )
        prefix_sum[i] = encrypt(i, key) + prefix_sum[i-1];

    part_sum[t] = prefix_sum[end[t]-1];
    pthread_exit(NULL);
}

void *addAll (void *thread_id) {
    long t = (long)thread_id;
    int pre_sum = 0;
    for ( int j = 0; j < t; j++ ) pre_sum += part_sum[j];

    for ( int i = start[t]; i < end[t]; i++ )
        prefix_sum[i] += pre_sum;

    pthread_exit(NULL);
}

int main() {

    pthread_t threads[MAX_THREAD];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    while (scanf("%d %" PRIu32, &n, &key) == 2) {

        // sequential fo each processor
        start[0] = 1;
        end[MAX_THREAD - 1] = n + 1;
        for ( int t = 0; t < MAX_THREAD; t++ ) {
            if ( t > 0 ) start[t] = max(end[t-1], t*n/MAX_THREAD + 1);
            if ( t < MAX_THREAD - 1 ) end[t] = (t+1)*n/MAX_THREAD + 1;
            pthread_create(&threads[t], &attr, seqSum, (void *)t);
        }

        // join
        for ( int t = 0; t < MAX_THREAD; t++ ) {
            pthread_join(threads[t], NULL);
        }

        for ( int t = 0; t < MAX_THREAD; t++ ) {
            pthread_create(&threads[t], &attr, addAll, (void *)t);
        }

        // join
        for ( int t = 0; t < MAX_THREAD; t++ ) {
            pthread_join(threads[t], NULL);
        }

        output(prefix_sum, n);

    }

    pthread_attr_destroy(&attr);
    return 0;
}