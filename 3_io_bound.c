// sudo perf stat -e cpu-clock,task-clock,L1-dcache-loads,L1-dcache-load-misses,cache-references,cache-misses,context-switches -o 3_io_bound_stats.txt ./3_io_bound 6 > /dev/null

// sudo perf stat -e cpu-clock,task-clock,L1-dcache-loads,L1-dcache-load-misses,cache-references,cache-misses,context-switches ./3_io_bound 6

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <ctype.h>

#define NUM_OPERATIONS 100000
#define CLOCK_MONOTONIC 1

void *IO_Routine(void *arg){
    int thread_id = *(int *)arg;
    char filename[20];
    sprintf(filename, "file_%d.txt", thread_id);

    // Perform file write operations
    for(int i=0;i<NUM_OPERATIONS;i++){
        FILE *file = fopen(filename, "a"); // Open in write mode to overwrite
        if(file==NULL){
            perror("Failed to open file for writing");
            exit(EXIT_FAILURE);
        }
        fprintf(file, "Thread %d, Operation %d\n", thread_id, i);
        fclose(file);
    }

    char output_filename[50];
    sprintf(output_filename, "output_%d.txt", thread_id);

    // Perform file read and write operations
    FILE *file = fopen(filename, "r");
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

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    for(int i=0;i<NUM_THREADS;i++){
        thread_ids[i]=i;
        pthread_create(&th[i], NULL, &IO_Routine, &thread_ids[i]);
    }

    for(int i=0;i<NUM_THREADS;i++){
        pthread_join(th[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double time_taken=(end.tv_sec-start.tv_sec)+(end.tv_nsec-start.tv_nsec)/1e9;
    printf("I/O-bound program execution time: %f seconds\n", time_taken);

    return 0;

}