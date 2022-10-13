// ParallelSortingComparison.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

/* C++ Program for Bitonic Sort. Note that this program
   works only when size of input is a power of 2. */
#include<iostream>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#define TASK_SIZE 127

using namespace std;

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
   for (int i = 0; i < size; i++)
      m[i] = rand_interval(min, max);
}

void printArray(int* a, int size) {
   for (int i = 0; i < size; i++)
      printf("%d ", a[i]);
   printf("\n");
}

int isSorted(int* a, int size) {
   for (int i = 0; i < size - 1; i++)
      if (a[i] > a[i + 1])
         return 0;
   return 1;
}

/*The parameter dir indicates the sorting direction, ASCENDING
   or DESCENDING; if (a[i] > a[j]) agrees with the direction,
   then a[i] and a[j] are interchanged.*/
void compAndSwap(int* a, int i, int j, int dir)
{
   if (dir == (a[i] > a[j]))
      swap(a[i], a[j]);
}

/*It recursively sorts a bitonic sequence in ascending order,
  if dir = 1, and in descending order otherwise (means dir=0).
  The sequence to be sorted starts at index position low,
  the parameter cnt is the number of elements to be sorted.*/
void bitonicMerge(int* a, int low, int cnt, int dir)
{
   if (cnt > 1)
   {
      int k = cnt / 2;
      for (int i = low; i < low + k; i++)
         compAndSwap(a, i, i + k, dir);
      bitonicMerge(a, low, k, dir);
      bitonicMerge(a, low + k, k, dir);
   }
}

/* This function first produces a bitonic sequence by recursively
    sorting its two halves in opposite sorting orders, and then
    calls bitonicMerge to make them in the same order */
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

/* Caller of bitonicSort for sorting the entire array of
   length N in ASCENDING order */
void sort(int* a, int N, int up)
{
   bitonicSort(a, 0, N, up);
}

void init(int* a, int size) {
   for (int i = 0; i < size; i++)
      a[i] = 0;
}

// Driver code
int main(int argc, char* argv[])
{
   srand(123456);
   // Instantiating arrays
   // 67108864, 16777216, 2097152
   int N = (argc > 1) ? atoi(argv[1]) : 2097152;
   int print = (argc > 2) ? atoi(argv[2]) : 0;
   int numThreads = (argc > 3) ? atoi(argv[3]) : 1;
   int* X = (int*)malloc(N * sizeof(int));
   int* tmp = (int*)malloc(N * sizeof(int));
   int up = 1;   // means sort in ascending order

   omp_set_dynamic(0);              /** Explicitly disable dynamic teams **/
   omp_set_num_threads(numThreads); /** Use N threads for all parallel regions **/

   // Dealing with fail memory allocation
   if (!X || !tmp)
   {
      if (X) free(X);
      if (tmp) free(tmp);
      return (EXIT_FAILURE);
   }

   fillupRandomly(X, N, 0, 500);

   //printArray(X, N);
   double begin = omp_get_wtime();
#pragma omp parallel
   {
#pragma omp single
      sort(X, N, up);
   }
   double end = omp_get_wtime();
   printf("Time: %f (s) \n", end - begin);

   //printArray(X, N);
   assert(1 == isSorted(X, N));
   printf("Sorted\n");

   free(X);
   free(tmp);
   return (EXIT_SUCCESS);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
