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
#include "OpenMPBitonicSort.h"

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

int runExperiments(int up, int low, int high, int print = 0) {
   // Experiment value setup
   // 67108864, 16777216, 2097152
   int arraySizes[]{ 2097152 };
   int threadCount[]{ 4 };

   for (int N : arraySizes) {
      int* X = (int*)malloc(N * sizeof(int));
      // Dealing with fail memory allocation
      if (!X)
      {
         if (X) free(X);
         return (EXIT_FAILURE);
      }
      BaseSort* sortingAlgorithms[]{ new OpenMPBitonicSort(X, N, up) };
      for (BaseSort* sortAlgo : sortingAlgorithms) {
         for (int numThreads : threadCount) {
            omp_set_dynamic(0);              /** Explicitly disable dynamic teams **/
            omp_set_num_threads(numThreads); /** Use N threads for all parallel regions **/

            // Have to fill up with random numbers every time since sorts in place
            fillupRandomly(X, N, low, high);

            if (print == 1) {
               sortAlgo->printArray();
            }

            double begin = omp_get_wtime();
            sortAlgo->sort();
            double end = omp_get_wtime();
            printf("Time: %f (s) \n", end - begin);

            if (print == 1) {
               sortAlgo->printArray();
            }
            // TODO: store results in a csv

            assert(1 == sortAlgo->isSorted());
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
   int print = 0;
   int up = 1;   // means sort in ascending order
   runExperiments(up, 0, 500, print);

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
