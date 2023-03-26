#include "smpi.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/time.h>

// Die Dimensionen sind zwar fix am Übungszettel vorgegeben,
// aber prinzipiell sollte man sie trotzdem im Programm nicht hart-coden.
// Wenn man sie als Konstanten definiert, kann man sie später leichter ändern.
#define A_DIMX	1000
#define B_DIMY	1000
#define S_DIM	1000

// Makro, welches einen 2-dimensionalen Index in einem 1-dimensionalen Index umwandelt
// x .. x-Index (Anzahl der Zeilen)
// y .. y-Index (Anzahl der Spalten)
// s .. Größe der y-Dimension (Max. Anzahl der Spalten)
#define MATINDEX(x,y,s)		((x)*(s)+(y))

// Die Matrizen A, B und C
// Diese als globale Variablen zu definieren, macht es für die Threads einfacher, darauf zuzugreifen
int *A, *B, *C;

// Struktur für die Datenübergabe an die Threads
// x .. Zeile der A Matrix
// y .. Spalte der B Matrix
typedef struct {
    int x;
    int y;
    int a;
    int b;
    int s;
} thread_data_t;


// Funktion, welche von den Threads ausgeführt wird
void* thread_func(void* arg) {
    // Caste übergebene Daten zum richtigen Datentyp
    auto* data = (thread_data_t*)arg;
    // Bereche Zeile * Spalte
    for (int x = data->x; x < data->x+data->y; x++) {
        for (int y = 0; y < B_DIMY; y++) {
            int result = 0;
            for (int i = 0; i < S_DIM; i++) {
                result += A[MATINDEX(x,i,S_DIM)] * B[MATINDEX(i,y,B_DIMY)];
            }
            C[MATINDEX(x,y, B_DIMY)] = result;
        }
    }
    return NULL;
}

void* thread_smpi_func(void* arg) {
    smpi_init();
    printf("Hello from thread %d\n", smpi_get_rank());

    // Caste übergebene Daten zum richtigen Datentyp
    thread_data_t data;

    // Receive thread data
    int source;
    smpi_recv(&data, sizeof(thread_data_t), &source);
    printf("Thread %d: Received data from %d.\n", smpi_get_rank(), source);

    // Allocate memory for matrices
    int* A = (int*)malloc(sizeof(int)*data.y*data.s);
    int* B = (int*)malloc(sizeof(int)*data.s*data.b);
    int* C = (int*)malloc(sizeof(int)*data.y*data.b);

    // Receive matrices
    smpi_recv(A, sizeof(int)*data.y*data.s, NULL);
    smpi_recv(B, sizeof(int)*data.s*data.b, NULL);

    printf("Thread %d: Received matrices.\n", smpi_get_rank());

    for (int x = 0; x < data.y; x++) {
        for (int y = 0; y < data.b; y++) {
            int result = 0;
            for (int i = 0; i < data.s; i++) {
                result += A[MATINDEX(x, i, data.s)] * B[MATINDEX(i, y, data.b)];
            }
            C[MATINDEX(x, y, data.b)] = result;
        }
    }

    smpi_send(&data, sizeof(thread_data_t), 0);
    smpi_send(C, sizeof(int)*data.y*data.b, 0);

    smpi_finalize();

    return NULL;
}

// Gebe die übergebene Matrix aus
// mat .. Matrix,
// x .. Anzahl der Zeilen
// y .. Anzahl der Spalten
void printMatrix(int* mat, int x, int y) {
    int min = INT_MAX, max = INT_MIN, sum = 0;
    // Schleife über alle Zeilen
    for (int i = 0; i < x; i++) {
        int firstElement = 1;
        // Schleife über alle Spalten
        for (int j = 0; j < y; j++) {
            // Für das erste Element müssen wir davor kein '\t' ausgeben
            if (firstElement) {
                firstElement = 0;
            } else {
                printf("\t");
            }
            int e = mat[MATINDEX(i,j,y)];
            // Gebe Element aus
            printf("%d", e);
            // Berechne min
            if (e < min) {
                min = e;
            }
            // Berechne max
            if (e > max) {
                max = e;
            }
            // Berechne Summe
            sum += e;
        }
        printf("\n");
    }
    // Gebe min, max und summe aus
    printf("Minimaler Wert: %d; Maximaler Wert: %d; Summe aller Werte: %d\n", min, max, sum);
}

void new_func(int max_threads) {
    // Definiere Thread-Handles und Thread-Eingabedaten
    thread_data_t threadData[max_threads];
    pthread_t threads[max_threads];

    // Erzeuge die Workers
    for (int w = 0; w < max_threads; w++) {
        // Fülle Thread-Datenstruktur
        thread_data_t* data = &threadData[w];
        // Berechne Range der äussersten Schleife, die dieser Worker berechnen soll
        data->x = (A_DIMX / max_threads) * w;
        if (w == max_threads-1) {
            data->y = A_DIMX - data->x;
        } else {
            data->y = A_DIMX / max_threads;
        }
        // Erzeuge Thread
        pthread_create(&threads[w], NULL, thread_func, data);
    }

    // Warte bis alle Threads fertig sind
    for (int i = 0; i < max_threads; i++) {
        //printf("%lu\n", i);
        pthread_join(threads[i], NULL);
    }

    // Gebe Ergebnis aus
    //printMatrix(C, A_DIMX, B_DIMY);
}

void new_smpi_func(int max_threads) {

    if (max_threads == 1) {
        // Bereche Zeile * Spalte
        for (int x = 0; x < A_DIMX; x++) {
            for (int y = 0; y < B_DIMY; y++) {
                int result = 0;
                for (int i = 0; i < S_DIM; i++) {
                    result += A[MATINDEX(x, i, S_DIM)] * B[MATINDEX(i, y, B_DIMY)];
                }
                C[MATINDEX(x, y, B_DIMY)] = result;
            }
        }
    } else {
        // Definiere Thread-Handles und Thread-Eingabedaten
        pthread_t threads[max_threads];

        // Erzeuge die Thread
        for (int t = 0; t < max_threads; t++) {
            // Fülle Thread-Datenstruktur
            thread_data_t data;
            data.x = (A_DIMX / max_threads) * t;
            if (t == max_threads - 1) {
                data.y = A_DIMX - data.x;
            } else {
                data.y = A_DIMX / max_threads;
            }
            data.a = A_DIMX;
            data.b = B_DIMY;
            data.s = S_DIM;

            // Erzeuge Thread
            pthread_create(&threads[t], NULL, thread_smpi_func, NULL);

            // Send thread data and matrices
            smpi_send(&data, sizeof(thread_data_t), t + 1);
            smpi_send(&A[MATINDEX(data.x, 0, S_DIM)],
                      sizeof(int) * data.y * data.s, t + 1);
            smpi_send(B, sizeof(int) * data.s * data.b, t + 1);
        }

        for (int i = 0; i < max_threads; i++) {
            thread_data_t data;
            int source;
            smpi_recv(&data, sizeof(thread_data_t), &source);
            printf("Received data from thread %d\n", source);
            smpi_recv(&C[MATINDEX(data.y, 0, S_DIM)], sizeof(int) * data.y * data.b, &source);
            printf("Received result from thread %d\n", source);
        }

        // Warte bis alle Threads fertig sind
        for (int i = 0; i < max_threads; i++) {
            pthread_join(threads[i], NULL);
            smpi_finalize();
        }
    }
}


int main(int argc, char *argv[]) {

    printf("ARGC COnter =: %i\n", argc);
    int max_threads;
    int runs;

    if (argc < 3) {
        max_threads = 6;
        runs = 1;
    }else{
        max_threads = atoi(argv[1]);
        runs = atoi(argv[2]);
    }


    if(max_threads > 1) {
        smpi_setup(max_threads+1);
        smpi_init();}


    // Alloziere Speicher für die Matrizen
    A = static_cast<int *>(malloc(sizeof(int) * A_DIMX * S_DIM));
    B = static_cast<int *>(malloc(sizeof(int) * S_DIM * B_DIMY));
    C = static_cast<int *>(malloc(sizeof(int) * A_DIMX * B_DIMY));

    // Fülle die Matrizen A und B mit Zufallswerten (zwischen 0 und 10)
    for (int i = 0; i < A_DIMX*S_DIM; i++) {
        A[i] = rand() % 10;
    }
    for (int i = 0; i < S_DIM*B_DIMY; i++) {
        B[i] = rand() % 10;
    }

    for(int i = 0; i < runs; i++) {
        clock_t b;
        b = clock();
        new_smpi_func(max_threads);
        b = clock() - b;
        double time_taken_b = ((double)b)/CLOCKS_PER_SEC; // in seconds
        printf("%f,", time_taken_b);
    }

    if(max_threads > 1){
        smpi_finalize();
    }




    return 0;
}

