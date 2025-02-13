// sudo perf stat -e cpu-clock,task-clock,L1-dcache-loads,L1-dcache-load-misses,cache-references,cache-misses,context-switches -o 1_cpu_bound_stats.txt ./1_cpu_bound 6 > /dev/null

// sudo perf stat -e cpu-clock,task-clock,L1-dcache-loads,L1-dcache-load-misses,cache-references,cache-misses,context-switches ./1_cpu_bound 6


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>

#define N 100000
#define CLOCK_MONOTONIC 1

typedef struct{
    int start;
    int end;
} ThreadData;

bool prime[N+1];

bool primeCheck(int num){
    if(num<=1) return false;
    for(int i=0;i<num;i++){
        if(num%2==0) return false;
    }
    return true;
}

void *CPU_routine(void *arg){
    ThreadData *data = (ThreadData *)arg;
    for(int i=data->start;i<=data->end;i++){
        prime[i] = primeCheck(i);
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

    for(int i=0;i<=N;i++){
        prime[i]=true;
    }
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    for(int i=0;i<NUM_THREADS;i++){
        td[i].start=i*range+1;
        td[i].end=(i+1)*range;
        pthread_create(&th[i], NULL, &CPU_routine, &td[i]);
    }

    for(int i=0;i<NUM_THREADS;i++){
        pthread_join(th[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double time_taken=(end.tv_sec-start.tv_sec)+(end.tv_nsec-start.tv_nsec)/1e9;
    printf("CPU-bound program execution time: %f seconds\n", time_taken);

    printf("Wnat to see prime numbers...(Type 1 for Yes and 0 for No) \n");
    int flag;
    scanf("%d",&flag);
    if(flag){
        for(int i=2;i<=N;i++){
            if(prime[i]) printf("%d ",i);
        }
    }
    printf("/n");
    return 0;

}