#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>
#include <time.h>
#define max_num 100000
int n;
int a[max_num], sorted[max_num];
void generate() {
    srand(time(NULL));
    int i;
    for (i = 0; i < n; i++) {
        a[i] = rand() % 1000 + 1;
        sorted[i] = 0;
    }
}
void parallel() {
    int i, j, count;
    omp_set_num_threads(50);
    double start_time = omp_get_wtime();
    #pragma omp parallel private(i, j, count)
    {
        #pragma omp for
        for (i = 0; i < n; i++) {
            count = 0;
            for (j = 0; j < n; j++) {
                if (a[i] > a[j])
                    count++;
            }
            while (sorted[count] != 0)
                count++;
            sorted[count] = a[i];
        }
    }
    double end_time = omp_get_wtime();
    double time_used = end_time - start_time;
    printf("Parallel time: %f s\n", time_used);
    for (i = 0; i <n; i ++){ 
        printf("%d ", sorted[i]);
    }
}
void serial() {
    int i, j, count;
    double start_time = omp_get_wtime();
    for (i = 0; i < n; i++) {
        count = 0;
        for (j = 0; j < n; j++) {
            if (a[i] > a[j]) {
                count++;
            }
        }
        while (sorted[count] != 0)
            count++;
        sorted[count] = a[i];
    }
    double end_time = omp_get_wtime();
    double time_used = end_time - start_time;
    printf("\nSerial time: %f s\n", time_used);
}
int main() {
    int i;
    printf("Enter the size of the data to be sorted (the maximum value is 100000, enter 0 to end) : ");
    while (scanf("%d", &n), n) {
        if (n >= max_num) {
            puts("Data size is too large, re-enter");
            continue;
        }
        generate();
        parallel();
        /*serial();*/
        printf("\nEnter the data size to be sorted, enter 0 to end : ");
    }
    return 0;
}
