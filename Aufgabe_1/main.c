#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/time.h>

// Die Dimensionen sind zwar fix am Übungszettel vorgegeben,
// aber prinzipiell sollte man sie trotzdem im Programm nicht hart-coden.
// Wenn man sie als Konstanten definiert, kann man sie später leichter ändern.
#define A_DIMX	10
#define B_DIMY	10
#define S_DIM	10

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
} thread_data_t;

// Funktion, welche von den Threads ausgeführt wird
void* thread_func(void* arg) {
    // Caste übergebene Daten zum richtigen Datentyp
    thread_data_t* data = (thread_data_t*)arg;
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


int main(int argc, char *argv[]) {

    int max_threads = atoi(argv[1]);
    int runs = atoi(argv[2]);

    // Alloziere Speicher für die Matrizen
    A = malloc(sizeof(int)*A_DIMX*S_DIM);
    B = malloc(sizeof(int)*S_DIM*B_DIMY);
    C = malloc(sizeof(int)*A_DIMX*B_DIMY);

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
        new_func(max_threads);
        b = clock() - b;
        double time_taken_b = ((double)b)/CLOCKS_PER_SEC; // in seconds
        printf("%f,", time_taken_b);
    }


    return 0;
}

