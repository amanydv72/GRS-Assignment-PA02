// sudo perf stat -e cpu-clock,task-clock,L1-dcache-loads,L1-dcache-load-misses,cache-references,cache-misses,context-switches -o 2_memory_bound_stats.txt ./2_memory_bound 6 > /dev/null

// sudo perf stat -e cpu-clock,task-clock,L1-dcache-loads,L1-dcache-load-misses,cache-references,cache-misses,context-switches ./2_memory_bound

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define N 100000
#define CLOCK_MONOTONIC 1

int* array;

typedef struct{
    int start;
    int end;
} ThreadData;

void *MEMORY_Routine(void *arg){
    ThreadData *data=(ThreadData *)arg;
    for(int i=data->start;i<=data->end;i++){
        array[i]=array[i]+1; // Initialize array with random numbers
    }
    pthread_exit(NULL);
}


int main(int argc, char *args[]){
    if(argc!=2){
        printf("Required: %s <number_of_threads>\n", args[0]);
        return 1;
    }
    int NUM_THREADS = atoi(args[1]);
    if(NUM_THREADS<=0){
        printf("Error: Number of threads must be a positive integer.\n");
        return 1;
    }

    pthread_t th[NUM_THREADS];
    ThreadData td[NUM_THREADS];
    int range=N/NUM_THREADS;

    array=(int*)malloc(N*sizeof(int));
    if(!array){
        fprintf(stderr,"Memory allocation failed.\n");
        return 1;
    }

    for(int i=0;i<N;i++){
        array[i]=rand()%100;
    }

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    for(int i=0;i<NUM_THREADS;i++){
        td[i].start=i*range+1;
        td[i].end=(i==NUM_THREADS-1)?N:(i+1)*range;
        pthread_create(&th[i], NULL, &MEMORY_Routine, &td[i]);
    }

    for(int i=0;i<NUM_THREADS;i++){
        pthread_join(th[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double time_taken=(end.tv_sec-start.tv_sec)+(end.tv_nsec-start.tv_nsec)/1e9;
    printf("Memory-bound program execution time: %f seconds\n", time_taken);

    // Free allocated memory
    free(array);
    return 0;

}