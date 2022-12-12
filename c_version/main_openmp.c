#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define TASK_SIZE 127

typedef void (*func)(int*, int, int, int);

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

void compAndSwap(int* a, int i, int j, int dir)
{
   if (dir == (a[i] > a[j]))
      swap(a, i, j);
}

void bitonicMerge(int* a, int low, int cnt, int dir)
{
   if (cnt > 1)
   {
      int k = cnt / 2;
      int i;
      for (i = low; i < low + k; i++)
         compAndSwap(a, i, i + k, dir);
      bitonicMerge(a, low, k, dir);
      bitonicMerge(a, low + k, k, dir);
   }
}

void bitonicSort(int* a, int low, int cnt, int dir)
{
   if (cnt > 1)
   {
      int k = cnt / 2;
      // sort in ascending order since dir here is 1
      #pragma omp task shared(a) if (k>TASK_SIZE) 
      bitonicSort(a, low, k, 1);

      // sort in descending order since dir here is 0
      #pragma omp task shared(a) if (k>TASK_SIZE) 
      bitonicSort(a, low + k, k, 0);

      // Will merge whole sequence in ascending order
      // since dir=1.
      #pragma omp taskwait
      bitonicMerge(a, low, cnt, dir);
   }
}

/*
QUICKSORT METHODS
*/

// function to swap elements
void qswap(int *a, int *b) {
  int t = *a;
  *a = *b;
  *b = t;
}

// function to find the partition position
int partition(int array[], int low, int high) {

  // select the rightmost element as pivot
  int pivot = array[high];

  // pointer for greater element
  int i = (low - 1);

  // traverse each element of the array
  // compare them with the pivot
  for (int j = low; j < high; j++) {
    if (array[j] <= pivot) {

      // if element smaller than pivot is found
      // swap it with the greater element pointed by i
      i++;

      // swap element at i with element at j
      qswap(&array[i], &array[j]);
    }
  }

  // swap the pivot element with the greater element at i
  qswap(&array[i + 1], &array[high]);

  // return the partition point
  return (i + 1);
}

void quickSort(int* array, int low, int high, int dir) {
  if (low < high) {

    // find the pivot element such that
    // elements smaller than pivot are on left of pivot
    // elements greater than pivot are on right of pivot
    int pi = partition(array, low, high);

    #pragma omp parallel sections
    {
     	#pragma omp section
        {
            // recursive call on the left of pivot
            quickSort(array, low, pi - 1, dir);
        }
	#pragma omp section
        {
            // recursive call on the right of pivot
            quickSort(array, pi + 1, high, dir);
        }
    }
  }
}

/*
END OF QUICKSORT METHODS 
*/

int runExperiments(int up, int low, int high, int print) {

   FILE *fpt;
   fpt = fopen("Group18Data.csv", "w+");
   fprintf(fpt, "Algorithm, Implementation, Thread Count, Elements, Time\n");

   // Experiment value setup
   // 67108864, 16777216, 2097152
   int arraySizes[] = {2097152};
   int threadCount[] = {1, 4};
   int i;
   for (i = 0; i < sizeof(arraySizes) / sizeof(arraySizes[0]); i++)
   {
      int N = arraySizes[i];
      int *X = (int *)malloc(N * sizeof(int));
      // Dealing with fail memory allocation
      if (!X)
      {
         if (X)
            free(X);
         return (EXIT_FAILURE);
      }
      
      func sortingAlgorithms[] = {&bitonicSort, &quickSort};

      char *sortingNames[] = {"Bitonic Sort"};
      char implemenation[] = "OpenMP";

      int j, k;
      for (j = 0; j < sizeof(sortingAlgorithms) / sizeof(sortingAlgorithms[0]); j++)
      {
         func sortAlgo = sortingAlgorithms[j];
         for (k = 0; k < sizeof(threadCount) / sizeof(threadCount[0]); k++)
         {
            omp_set_dynamic(0);                  // Explicitly disable dynamic teams
            omp_set_num_threads(threadCount[k]); // Use N threads for all parallel regions

            // Have to fill up with random numbers every time since sorts in place
            fillupRandomly(X, N, low, high);

            if (print == 1)
            {
               printArray(X, N);
            }

            double begin = omp_get_wtime();
                #pragma omp parallel
                {
                #pragma omp single
                    sortAlgo(X, 0, N, up);
                }
                double end = omp_get_wtime();
                printf("Time: %f (s) \n", end - begin);

                if (print == 1) {
                printArray(X, N);
                }
               // CSV
               fprintf(fpt,"%s, %s, %i, %i, %f\n", sortingNames[j], implemenation, threadCount[k],N, end - begin);


                assert(1 == isSorted(X, N));
                printf("Sorted\n");
            }
        }
        free(X);
        fclose(fpt);
    }
}

// Driver code
int main(int argc, char* argv[])
{
   srand(123456);
   int print = 0;
   int up = 1;   // means sort in ascending order
   runExperiments(up, 0, 500, print);

   return (EXIT_SUCCESS);
}
