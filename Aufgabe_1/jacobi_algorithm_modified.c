#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>


/**
 * The problem size, which is proportional to the number of executed instructions, is given by following formula:
 * problemsize = m_size * m_size * iterations
 *
 * Using the same problemsize one can change the granularity of parallelism by using different values for iterations
 * (and adjusting m_size accordingly). The higher iterations the more fine-granular the parallelism.
 *
 * The granularity of parallelism is represented by the measured "barrier activations per second".
 */


// How many threads should be used
//#define  THREAD_COUNT     4

// The number of measurements used to calculate the average execution time
//#define  MEASUREMENTS     6


/**
 * Here are some example values given for iterations and m_size which have roughly the same problem size
 * but different granularity of parallelism.
 *
 * Attention: A matrix size of 25000 needs 25000*25000*8 Bytes (~5GB) of Memory. Make sure to not exceed the amount of memory in your system!
 */


//#define  iterations  500
//#define  m_size      5000

//#define  iterations  2000
//#define  m_size      2500

//#define  iterations  8000
//#define  m_size      1250

//#define  iterations  50000
//#define  m_size      500

//#define  iterations  800000
//#define  m_size      125

int max_threads;
int runs;
int iterations;
int m_size;

typedef struct  {
    int thread_id;
    double** matrix;
    int matrix_size;
    double* right_hand_side;
    double* solution;
    int range_start;
    int range_size;
} thread_info_t;


pthread_barrier_t barrier;



void solve_jacobi_serial(double** matrix, int matrix_size, double* right_hand_side, double* solution) {
    double* tempsolution = (double*)malloc(m_size * sizeof(double));

    // Iteration loop
    for (int i = 0; i < iterations; i++){
        // loop through rows
        for (int j = 0; j < matrix_size; j++){
            double sigma_value = 0;
            // loop through columns
            for (int k = 0; k < matrix_size; k++){
                if (j != k) {
                    sigma_value += matrix[j][k] * solution[k];
                }
            }
            tempsolution[j] = (right_hand_side[j] - sigma_value) / matrix[j][j];
        }
        memcpy(solution, tempsolution, m_size);

        //printf("\rIteration #%6d/%d", i+1, iterations);
    }
    //printf("\n");
}


void* solve_jacobi_parallel(void* targ) {
    thread_info_t* info = (thread_info_t*)targ;
    double* tempsolution = (double*)malloc(m_size * sizeof(double));

    //printf("Thread #%d - range %d, %d\n", info->thread_id, info->range_start, info->range_size);

    // Iteration loop
    for (int i = 0; i < iterations; i++){
        // loop through rows
        for (int j = info->range_start; j < info->range_start+info->range_size; j++){
            double sigma_value = 0;
            // loop through columns
            for (int k = 0; k < info->matrix_size; k++){
                if (j != k) {
                    sigma_value += info->matrix[j][k] * info->solution[k];
                }
            }
            tempsolution[j] = (info->right_hand_side[j] - sigma_value) / info->matrix[j][j];
        }

        // Here we need to synchronise before we continue with the next iteration
        pthread_barrier_wait(&barrier);
        memcpy(info->solution+info->range_start, tempsolution+info->range_start, info->range_size);

        //if (info->thread_id == 0) {
        //	printf("\rIteration #%6d/%d", i+1, iterations);
        //}
    }
    //if (info->thread_id == 0) {
    //	printf("\n");
    //}
    free(tempsolution);
    return NULL;
}


double gettime() {
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    return (double)time.tv_sec + (double)time.tv_nsec/1000000000.0;
}



int main(int argc, char *argv[]) {

    max_threads = atoi(argv[1]);
    runs = atoi(argv[2]);
    iterations = atoi(argv[3]);
    m_size = atoi(argv[4]);

    // Declare variables
    int matrix_size = m_size;
    double **matrix = (double**)malloc(matrix_size * sizeof(double*));
    for (int i = 0; i < matrix_size; i++) {
        matrix[i] = (double*)malloc(matrix_size*sizeof(double));
    }
    double* right_hand_side = (double*)malloc(matrix_size*sizeof(double));
    double* solution = (double*)malloc(matrix_size*sizeof(double));

    //printf("matrix size: %d\niteration count: %d\nthread count: %d\n", matrix_size, iterations, max_threads);


    // Initialise barrier if more than one thread
    if(max_threads > 1) {
        pthread_barrier_init(&barrier, NULL, max_threads);
    }

    // measurement loop
    double avgtime = 0.0;
    for (int m = 0; m < runs; m++) {

        // Initialize matrix and vectors
        for (int i = 0; i < matrix_size; i++) {
            // Jacobi only works correctly with strictly diagonally dominant matrices
            // Therefore we need to ensure that our random matrix is strictly diagonally dominant
            double row_sum = 0;
            for (int j = 0; j < matrix_size; j++) {
                matrix[i][j] = 1 + rand() % 20;
                if (j != i) {
                    row_sum += matrix[i][j];
                }
            }
            matrix[i][i] = row_sum + 1;
        }

        for (int i = 0; i < matrix_size; i++) {
            right_hand_side[i] = rand() % 20;
            solution[i] = right_hand_side[i];
        }

        // if only one thread then execute serial version
        if(max_threads == 1) {

            double starttime = gettime(); // starttime

            solve_jacobi_serial(matrix, matrix_size, right_hand_side, solution);

            double endtime = gettime(); // endtime
            avgtime += endtime - starttime;
            //printf("Time: %lf s\n", endtime - starttime);
            printf("%f,", endtime - starttime);

            // if more than one thread create threads and execute parallel version
        }else {
            thread_info_t infos[max_threads];
            pthread_t threads[max_threads];

            double starttime = gettime(); // starttime

            for (int i = 0; i < max_threads; i++) {
                infos[i].thread_id = i;
                infos[i].matrix = matrix;
                infos[i].matrix_size = matrix_size;
                infos[i].right_hand_side = right_hand_side;
                infos[i].solution = solution;
                infos[i].range_start = (m_size / max_threads) * i;
                if (i == max_threads - 1) {
                    infos[i].range_size = m_size - infos[i].range_start;
                } else {
                    infos[i].range_size = (m_size / max_threads);
                }
                pthread_create(&threads[i], NULL, solve_jacobi_parallel, &infos[i]);
            }
            for (int i = 0; i < max_threads; i++) {
                pthread_join(threads[i], NULL);
            }

            double endtime = gettime(); // endtime
            avgtime += endtime - starttime;
            printf("%f,", endtime - starttime);
            //printf("Barrier activations per second: %lf\n", (double) iterations / (endtime - starttime));
        }
    }

    //printf("Average time: %lf s\n", avgtime / (double)runs);

    if(max_threads > 1) {
        pthread_barrier_destroy(&barrier);
    }
}

