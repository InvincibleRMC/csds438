#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openacc.h>

#define SIZE 32

typedef void (*func)(int*, int);

// Utility functions 

unsigned int rand_interval(unsigned int min, unsigned int max)
{
   // https://stackoverflow.com/questions/2509679/
   int r;
   const unsigned int range = 1 + max - min;
   const unsigned int buckets = RAND_MAX / range;
   const unsigned int limit = buckets * range;

   do
   {
      r = rand();
   } while (r >= limit);

   return min + (r / buckets);
}

void fillupRandomly(int* m, int size, unsigned int min, unsigned int max) {
    int i;
   for (i = 0; i < size; i++)
      m[i] = rand_interval(min, max);
}

void printArray(int* a, int size)
{
    int i;
   for (i = 0; i < size; i++)
      printf("%d ", a[i]);
   printf("\n");
}

int isSorted(int* a, int size)
{
    int i;
   for (i = 0; i < size - 1; i++)
      if (a[i] > a[i + 1])
         return 0;
   return 1;
}

void swap(int *arr, int i, int j) {
  int temp = arr[i];
  arr[i] = arr[j];
  arr[j] = temp;
}

void compAndSwap(int* a, int i, int l, int k, int dir)
{
    if (l > i) {
        if (
            (i & k == 0 && (a[i] > a[l])) || 
            (i & k != 0 && (a[i] < a[l]))
        )
            swap(a, i, l);
    }
   
}

void bitonicSort(int *x, int N) {
    int i, j, k;
    int temp;
    int ixj;

    // #pragma acc data copy(x[0:SIZE]) create(temp, ixj)
    {
        // Loop over the number of levels
        // #pragma acc parallel loop
        for (k = 2; k < N; k*=2) {
            // Loop over the number of subarrays for this level
            for (j = k/2; j < N; j/=2) {
            // Loop over the number of elements in each subarray
                // #pragma acc loop seq
                // #pragma acc data copy(x[0:SIZE]) create(temp, ixj)
                {
                    // #pragma acc kernels
                    for (i = 0; i < N; i++) {
                        ixj = i ^ j;
                        compAndSwap(x, i, ixj, k, 1);
                    }
                }
            }
        }
    }
}

int runExperiments(int up, int low, int high, int print) {
    int num_gpu_devices,device_num;
    num_gpu_devices = acc_get_num_devices(acc_device_nvidia);
    device_num = acc_get_device_num(acc_device_nvidia);

    printf("device %d is used out of NVIDIA devices %d\n", device_num,num_gpu_devices);
    // Experiment value setup
    // 67108864, 16777216, 2097152
    int arraySizes[] = { 128 };
    int threadCount[] = { 1 };
    int i;
    for (i = 0; i < sizeof(arraySizes) / sizeof(arraySizes[0]); i++) {
        // int N = arraySizes[i];
        int N = SIZE;
        int* X = (int*)malloc(N * sizeof(int));
        // Dealing with fail memory allocation
        if (!X)
        {
            if (X) free(X);
            return (EXIT_FAILURE);
        }
        func sortingAlgorithms[] = { &bitonicSort };
        int j, k;
        for (j = 0; j < sizeof(sortingAlgorithms) / sizeof(sortingAlgorithms[0]); j++) {
            func sortAlgo = sortingAlgorithms[j];
            for (k = 0; k < sizeof(threadCount) / sizeof(threadCount[0]); k++) {
                // Have to fill up with random numbers every time since sorts in place
                fillupRandomly(X, N, low, high);
 
                if (print == 1) {
                    printArray(X, N);
                }

                // double begin = omp_get_wtime();
                // #pragma acc data copy(X[0:N])
                {
                    // #pragma acc kernels
                }
                sortAlgo(X, N);
                // double end = omp_get_wtime();
                // printf("Time: %f (s) \n", end - begin);

                if (print == 1) {
                    printArray(X, N);
                }
                // TODO: store results in a csv

                assert(1 == isSorted(X, N));
                printf("Sorted\n");
            }
        }
        free(X);
    }
}

// Driver code
int main(int argc, char* argv[])
{
   srand(123456);
   int print = 1;
   int up = 1;   // means sort in ascending order
   runExperiments(up, 0, 500, print);

   return (EXIT_SUCCESS);
}
