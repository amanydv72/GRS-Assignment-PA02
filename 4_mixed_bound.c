// sudo perf stat -e cpu-clock,task-clock,L1-dcache-loads,L1-dcache-load-misses,cache-references,cache-misses,context-switches -o 4_mixed_bound_stats.txt ./4_mixed_bound 6 > /dev/null

// sudo perf stat -e cpu-clock,task-clock,L1-dcache-loads,L1-dcache-load-misses,cache-references,cache-misses,context-switches ./4_mixed_bound 6


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define N 100000 
#define ARRAY_SIZE 100000 
#define NUM_OPERATIONS 100000
#define CLOCK_MONOTONIC 1

bool primeCheck(int num){
    if(num<=1) return false;
    for(int i=0;i<num;i++){
        if(num%2==0) return false;
    }
    return true;
}

void *CPU_routine(){
    for(int i=2;i<=N;i++){
        if(primeCheck(i)){
            //....
        }
    }
    printf("CPU-bound task (prime computation) completed.\n");
    pthread_exit(NULL);
}

void *MEMORY_Routine(void *arg){
    int *array=(int *)arg;
    for(int j=0;j<ARRAY_SIZE;j++){
        array[j]=array[j]+1;
    }
    printf("Memory-bound task (array modification) completed.\n");
    pthread_exit(NULL);
}

void *IO_Routine(void *arg){
    int thread_id=*(int *)arg;
    char filename[20];
    sprintf(filename, "Mixed_file_%d.txt", thread_id);

    // Perform file write operations
    for(int i=0;i<NUM_OPERATIONS;i++){
        FILE *file=fopen(filename, "a"); // Open in append mode
        if(file==NULL){
            perror("Failed to open file for writing");
            exit(EXIT_FAILURE);
        }
        fprintf(file, "Thread %d, Operation %d\n", thread_id, i);
        fclose(file);
    }
    char output_filename[50];
    sprintf(output_filename, "Mixed_output_%d.txt", thread_id);

    // Perform file read and write operations
    FILE *file=fopen(filename, "r");
    if(file==NULL){
        perror("Failed to open file for reading");
        exit(EXIT_FAILURE);
    }
    FILE *output_file=fopen(output_filename, "a");
    if(output_file==NULL){
        perror("Failed to open output file");
        fclose(file); // Close the input file before exiting
        exit(EXIT_FAILURE);
    }

    char buffer[100];
    while(fgets(buffer, sizeof(buffer), file)!=NULL){
        // Modify the content (convert to uppercase)
        for(int i=0;buffer[i];i++){
            buffer[i]=toupper(buffer[i]);
        }
        // Write the modified content to the output file
        fprintf(output_file, "%s", buffer);
    }
    fclose(file);
    fclose(output_file);
    printf("I/O-bound task completed.\n");
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
    int thread_ids[NUM_THREADS];
    int *array=(int*)malloc(ARRAY_SIZE*sizeof(int));
    if(!array){
        fprintf(stderr,"Memory allocation failed.\n");
        return 1;
    }
    for(int i=0;i<ARRAY_SIZE;i++){
        array[i]=rand()%100;
    }

    int cpu_threads=NUM_THREADS/3;
    int mem_threads=NUM_THREADS/3;
    int io_threads=NUM_THREADS-cpu_threads-mem_threads;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for(int i=0;i<cpu_threads;i++){
        pthread_create(&th[i], NULL, &CPU_routine, NULL);
    }
    for(int i=cpu_threads;i<cpu_threads+mem_threads;i++){
        pthread_create(&th[i], NULL, &MEMORY_Routine, array);
    }
    for(int i=cpu_threads+mem_threads;i<NUM_THREADS;i++){
        thread_ids[i]=i;
        pthread_create(&th[i], NULL, &IO_Routine, &thread_ids[i]);
    }
    for(int i=0;i<NUM_THREADS;i++){
        pthread_join(th[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double time_taken=(end.tv_sec-start.tv_sec)+(end.tv_nsec-start.tv_nsec)/1e9;
    printf("Mixed workload execution time: %f seconds\n", time_taken);
    free(array);
    return 0;
}
