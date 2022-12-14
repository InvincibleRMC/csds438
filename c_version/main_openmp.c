#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define TASK_SIZE 127

typedef void (*func)(int *, int, int, int);

// Utility functions

// unsigned int rand_interval(unsigned int min, unsigned int max)
// {
//    // https://stackoverflow.com/questions/2509679/
//    int r;
//    const unsigned int range = 1 + max - min;
//    const unsigned int buckets = RAND_MAX / range;
//    const unsigned int limit = buckets * range;

//    do
//    {
//       r = rand();
//    } while (r >= limit);

//    return min + (r / buckets);
// }

void swap(int *arr, int i, int j)
{
   int temp = arr[i];
   arr[i] = arr[j];
   arr[j] = temp;
}

void fillupRandomly(int *m, int size, unsigned int min, unsigned int max)
{
   m[0] = 0;
   for (int i = 1; i < size; i++)
   {
      m[i] = i;

      swap(m, i, rand() % i);
   }
}

void printArray(int *a, int size)
{
   int i;
   for (i = 0; i < size; i++)
      printf("%d ", a[i]);
   printf("\n");
}

int isSorted(int *a, int size)
{
   int i;
   for (i = 0; i < size - 1; i++)
      if (a[i] > a[i + 1])
         return 0;
   return 1;
}

void compAndSwap(int *a, int i, int j, int dir)
{
   if (dir == (a[i] > a[j]))
      swap(a, i, j);
}

void bitonicMerge(int *a, int low, int cnt, int dir)
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

void bitonicSort(int *a, int low, int cnt, int dir)
{
   if (cnt > 1)
   {
      int k = cnt / 2;
// sort in ascending order since dir here is 1
#pragma omp task shared(a) if (k > TASK_SIZE)
      bitonicSort(a, low, k, 1);

// sort in descending order since dir here is 0
#pragma omp task shared(a) if (k > TASK_SIZE)
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
void qswap(int *a, int *b)
{
   int t = *a;
   *a = *b;
   *b = t;
}

// function to find the partition position
int partition(int array[], int low, int high)
{

   // select the rightmost element as pivot
   int pivot = array[high];

   // pointer for greater element
   int i = (low - 1);

   // traverse each element of the array
   // compare them with the pivot
   for (int j = low; j < high; j++)
   {
      if (array[j] <= pivot)
      {

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
// TODO WEIRD OFF BY ON ERROR
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
int cmpfunc(const void *a, const void *b)
{
   int va = *(const int *)a;
   int vb = *(const int *)b;
   // print("va =%i ")
   return (va > vb) - (va < vb);
}

int sameElements(int *arr1, int *arr2, int l)
{
   for (int i = 0; i < l; i++)
   {
      if (arr1[i] != arr2[i])
      {
         return 0;
      }
   }
   return 1;
}

void uniqueElement(int *arr, int length)
{
   for (int i = 0; i < length; i++)
   {
      for (int j = 0; j < length; j++)
      {
         if (arr[j] == arr[i] && i != j)
         {
            assert(0);
         }
      }
   }
}

int getNumAmount(int *arr, int length, int val)
{
   int count = 0;
   for (int i = 0; i < length; i++)
   {
      if (val == arr[i])
      {
         count++;
      }
   }
   return count;
}

int allValsSame(int *arr, int length)
{
   int val = arr[0];
   for (int i = 1; i < length; i++)
   {
      if (val != arr[i])
      {
         return 0;
      }
   }
   return 1;
}

int getBucketIndex(int e, int *splitter, int splitterCount, int p)
{
   if (e <= splitter[0])
   {
      return 0;
   }
   else if (e >= splitter[splitterCount - 1])
   {
      return p - 1;
   }
   else
   {
      for (int l = 1; l < splitterCount; l++)
      {
         if (splitter[l - 1] < e && e <= splitter[l])
         {
            return l;
         }
      }
   }
   assert(0);
}

int *sampleSortHelper(int arr[], int p, int k, int length)
{
   //Testing
   int *copy = (int *)malloc(length * sizeof(int));
   memcpy(copy, arr, length * sizeof(int));
   //  SPECIAL CASE
   if (p == 1)
   {
      printf("Special Case of One Bucket just calls qsort\n");
      qsort(arr, length, sizeof(arr[0]), cmpfunc);
      return arr;
   }

   if (length < k)
   {
      qsort(arr, length, sizeof(arr[0]), cmpfunc);
      return arr;
   }
   
   // More Threads than min bucket Size
   if (p > k)
   {
      assert(0);
   }

   // Generates Splitters
   int splitterCount = p - 1;
   int splitter[splitterCount];
   for (int i = 0; i < splitterCount - 1;i++){
      splitter[i] = arr[rand() % length];
   }
      qsort(splitter, splitterCount, sizeof(splitter[0]), cmpfunc);

   // Not effiecent memory wise
   // Generate Buckets
   int *buckets[p];
   int indicies[p];
   for (int i = 0; i < p; i++)
   {
      buckets[i] = (int *)malloc(length * sizeof(int));
      indicies[i] = 0;
   }

#pragma omp parallel
{
   #pragma omp taskloop
   //Divide  
   for (int j = 0; j < length; j++)
   {
      #pragma omp critical
      {
      int bucketNum = getBucketIndex(arr[j], splitter, splitterCount, p);
      buckets[bucketNum][indicies[bucketNum]] = arr[j];
      indicies[bucketNum] = indicies[bucketNum] + 1;
      }
   }

#pragma omp taskloop
//Conquer
   for (int i = 0; i < p; i++)
   {
      int *bucket = sampleSortHelper(buckets[i], p, k, indicies[i]);
      int startingPoint = 0;
      for (int l = 0; l < i; l++)
      {
         startingPoint = startingPoint + indicies[l];
      }
      memcpy(arr + startingPoint, bucket, indicies[i] * sizeof(int));
      free(buckets[i]);
   }
}

   //TESTING
   qsort(copy, length, sizeof(int), cmpfunc);
   if (!sameElements(arr, copy, length))
   {
      printArray(arr, length);
      printArray(copy, length);
   }
   assert(sameElements(arr, copy, length));
   free(copy);


   return arr;
}

void sampleSort(int *arr, int l, int h, int dir)
{
   int k = 1024;
   int p = omp_get_num_threads();
   memcpy(arr, sampleSortHelper(arr, p, k, h), h * sizeof(int));
}

/*
 * Counting sort methods
 */

// TODO SEEMS REALLY REALLY SLOW
// ALSO SEG FAULTS
void counting_parallel_omp(int* array, int low, int high, int dir) {
    int i, j, count;
    int size = high - low;
    int *sorted = (int *)malloc(size * sizeof(int));
    
    /*
     * This line just sets the number of threads to be run on.
        omp_set_num_threads(50);
    */
    double start_time = omp_get_wtime();
    #pragma omp parallel private(i, j, count)
    {
        #pragma omp for
        for (i = 0; i < size; i++) {
            count = 0;
            for (j = 0; j < size; j++) {
                if (array[i] > array[j])
                    count++;
            }
            while (sorted[count] != 0)
                count++;
            sorted[count] = array[i];
        }
    }
    double end_time = omp_get_wtime();
    double time_used = end_time - start_time;
}
/*
 * End of counting sort methods.
 */

int runExperiments(int up, int low, int high, int print) {

   FILE *fpt;
   fpt = fopen("Group18Data.csv", "w+");
   fprintf(fpt, "Algorithm, Implementation, Thread Count, Elements, Time\n");

   // Experiment value setup
   // 67108864, 16777216, 2097152
   // BITONIC NEEDS POWERS OF 2
   int arraySizes[] = {2097152/2 / 2/2/2/2/2/2/2};
   int threadCount[] = {1, 4};
   int i;
   for (i = 0; i < sizeof(arraySizes) / sizeof(arraySizes[0]); i++)
   {
      int N = arraySizes[i];
      int *X = (int *)malloc(N * sizeof(int));
      int *Y = (int *)malloc(N * sizeof(int));

      // Dealing with failed memory allocation
      if (!X)
      {
         if (X)
            free(X);
         return (EXIT_FAILURE);
      }
      if (!Y)
      {
         if (Y)
            free(Y);
         return (EXIT_FAILURE);
      }

      func sortingAlgorithms[] = {&sampleSort};
      char *sortingNames[] = {"Sample sort"};
      
      // func sortingAlgorithms[] = {&bitonicSort, &counting_parallel_omp, &quickSort};
      // char *sortingNames[] = {"Bitonic Sort", "QuickSort","Counting Sort"};
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
            memcpy(Y, X, N * sizeof(int));
            if (print == 1)
            {
               printArray(X, N);
            }

            double begin = omp_get_wtime();
// #pragma omp parallel
//             {
// #pragma omp single
               sortAlgo(X, 0, N, up);
            // }
            double end = omp_get_wtime();
            printf("Time: %f (s) \n", end - begin);

            if (print == 1)
            {
               printArray(X, N);
            }
            // CSV
            fprintf(fpt, "%s, %s, %i, %i, %f\n", sortingNames[j], implemenation, threadCount[k], N, end - begin);
            printf("%s, %s, %i, %i, %f\n", sortingNames[j], implemenation, threadCount[k], N, end - begin);

            qsort(Y, N, sizeof(int), cmpfunc);
            if (!sameElements(X, Y, N))
            {
               printArray(X, 10);
               printArray(Y, 10);
            }
            assert(isSorted(X, N));
            assert(sameElements(X, Y, N));
         }
      }
      free(X);
      free(Y);
      fclose(fpt);
   }
   return 0;
}

// Driver code
int main(int argc, char *argv[])
{
   srand(123456);
   int print = 0;
   int up = 1;   // means sort in ascending order
   runExperiments(up, 0, 10000, print);

   return (EXIT_SUCCESS);
}
