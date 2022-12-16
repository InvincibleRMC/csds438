#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <math.h>
#include <time.h>

#define TASK_SIZE 63
const int RUN = 32; // TimSort "run" size. Factors of 2 only, 32 or 64 recommended.

typedef void (*func)(int *, int, int, int);

/*
Utility  METHODS
*/

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

void swap(int *arr, int i, int j)
{
   int temp = arr[i];
   arr[i] = arr[j];
   arr[j] = temp;
}

void fillupRandomly(int *m, int size, unsigned int min, unsigned int max)
{
   int i;
   for (i = 0; i < size; i++)
      m[i] = rand_interval(min, max);
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

/*
Bitonic Sort METHODS
*/

void bitonicMerge(int *a, int low, int cnt, int dir)
{
   if (cnt > 1)
   {
      int k = cnt / 2;
      int i;
      // #pragma omp parallel for shared(a)
      for (i = low; i < low + k; i++)
         compAndSwap(a, i, i + k, dir);

      // #pragma omp parallel sections shared(a)
      // {
      // // sort in ascending order since dir here is 1
      // #pragma omp section
      // bitonicMerge(a, low, k, dir);

      // // sort in descending order since dir here is 0
      // #pragma omp section
      // bitonicMerge(a, low + k, k, dir);
      // }
      bitonicMerge(a, low, k, dir);
      bitonicMerge(a, low + k, k, dir);
      // #pragma omp taskwait
   }
}

void bitonicSort(int *a, int low, int cnt, int dir)
{
   if (cnt > 1)
   {
      int k = cnt / 2;
      {
         // sort in ascending order since dir here is 1
         #pragma omp task shared(a) if (k > TASK_SIZE)
         bitonicSort(a, low, k, 1);

         // sort in descending order since dir here is 0
         #pragma omp task shared(a) if (k > TASK_SIZE)
         bitonicSort(a, low + k, k, 0);
      }

      // Will merge whole sequence in ascending order
      // since dir=1.
      // #pragma omp barier
      #pragma omp taskwait
      bitonicMerge(a, low, cnt, dir);
   }
}

void bitonicSortWrapper(int *a, int low, int cnt, int dir)
{
   #pragma omp parallel
   {
      #pragma omp single
      bitonicSort(a, low, cnt, dir);
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
   int j;
   for (j = low; j < high; j++)
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

void quickSortHelper(int *array, int low, int high, int dir)
{
   if (low < high)
   {

      // find the pivot element such that
      // elements smaller than pivot are on left of pivot
      // elements greater than pivot are on right of pivot
      int pi = partition(array, low, high);

#pragma omp parallel sections
      {
#pragma omp section
         {
            // recursive call on the left of pivot
            quickSortHelper(array, low, pi - 1, dir);
         }
#pragma omp section
         {
            // recursive call on the right of pivot
            quickSortHelper(array, pi + 1, high, dir);
         }
      }
   }
}

void quickSort(int *array, int low, int high, int dir)
{
   quickSortHelper(array, low, high - 1, dir);
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
   printf("Element %i\n", e);
   printArray(splitter, splitterCount);
   assert(0);
}

int allPostive(int *arr, int length)
{
   for (int i = 0; i < length; i++)
   {
      if (arr[i] < 0)
      {
         return 0;
      }
   }
   return 1;
}

void sampleSortHelper(int arr[], int p, int k, int length)
{
   if (length < k)
   {
      qsort(arr, length, sizeof(arr[0]), cmpfunc);
      return;
   }

   // Generates Splitters
   int splitterCount = p - 1;
   int splitter[splitterCount];
   for (int i = 0; i < splitterCount; i++)
   {
      splitter[i] = arr[rand() % length];
   }
   qsort(splitter, splitterCount, sizeof(splitter[0]), cmpfunc);

   // Not effiecent memory wise
   // Generate Buckets
   int *buckets[p];
   for (int i = 0; i < p; i++)
   {
      buckets[i] = (int *)malloc(length * sizeof(int));
   }
   // Generate Buckets Indicies
   int indicies[p];
   memset(indicies, 0, p * sizeof(int));

#pragma omp parallel
   {

#pragma omp for

      // Divide
      for (int j = 0; j < length; j++)
      {
         int bucketNum = getBucketIndex(arr[j], splitter, splitterCount, p);
         int indexToWrite;

#pragma omp atomic capture
         indexToWrite = indicies[bucketNum]++;

         buckets[bucketNum][indexToWrite] = arr[j];
      }

#pragma omp for
      // Conquer
      for (int i = 0; i < p; i++)
      {
         sampleSortHelper(buckets[i], p, k, indicies[i]);
         int startingPoint = 0;
         for (int l = 0; l < i; l++)
         {
            startingPoint = startingPoint + indicies[l];
         }
         memcpy(arr + startingPoint, buckets[i], indicies[i] * sizeof(int));
         free(buckets[i]);
      }
   }
   return;
}

void sampleSort(int *arr, int l, int h, int dir)
{
   int k = 1024;
   int p = omp_get_num_threads();

   // More Threads than min bucket Size
   if (p > k)
   {
      k *= 2;
   }
   // Need at least buckets
   if (p <= 1)
   {
      p = 2;
   }
   sampleSortHelper(arr, p, k, h);
}

/*
 * Counting sort methods
 */

// TODO SEEMS REALLY REALLY SLOW
// ALSO SEG FAULTS
void counting_parallel_omp(int *array, int low, int high, int dir)
{
   int i, j, count;
   int size = high - low;
   int *sorted = (int *)malloc(size * sizeof(int));
   omp_set_num_threads(50);
   double start_time = omp_get_wtime();
#pragma omp parallel private(i, j, count)
   {
#pragma omp for
      for (i = 0; i < size; i++)
      {
         count = 0;
         for (j = 0; j < size; j++)
         {
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
   // int l;
   // for (l = 0; l < size; ++l)
   // {
   //    printf("%d ", sorted[l]);
   // }
   printf("SOrted?\n");
   printArray(sorted, size);
   memcpy(array, sorted, size * sizeof(sorted));
   free(sorted);
}
/*
 * End of counting sort methods.
 */

/**
 * Merge sort methods
 */
void merge(int *array, int l, int m, int r)
{
   int size1 = m - l + 1;
   int size2 = r - m;

   int *left = malloc(size1 * sizeof(int));
   int *right = malloc(size2 * sizeof(int));

   memcpy(left, array + l, size1 * sizeof(int));
   memcpy(right, array + m + 1, size2 * sizeof(int));

   int i = 0,
       j = 0, k = l;
   // compare each element of the two arrays and
   //  put the smaller element in the result array
   while (i < size1 && j < size2)
   {
      if (left[i] <= right[j])
      {
         array[k++] = left[i++];
      }
      else
      {
         array[k++] = right[j++];
      }
   }
   // put the remaining elements of arr1[] (if any) into arr[]
   while (i < size1)
   {
      array[k++] = left[i++];
   }
   while (j < size2)
   {
      array[k++] = right[j++];
   }
   free(left);
   free(right);
}


void mergeSortAux(int *X, int n, int *tmp) {
   int i = 0;
   int j = n/2;
   int ti = 0;

   while (i<n/2 && j<n) {
      if (X[i] < X[j]) {
         tmp[ti] = X[i];
         ti++; i++;
      } else {
         tmp[ti] = X[j];
         ti++; j++;
      }
   }
   while (i<n/2) { /* finish up lower half */
      tmp[ti] = X[i];
      ti++; i++;
   }
   while (j<n) { /* finish up upper half */
      tmp[ti] = X[j];
      ti++; j++;
   }
   memcpy(X, tmp, n*sizeof(int));
} 

void mergeSortHelper(int *X, int n, int *tmp)
{
   if (n < 2) return;

   #pragma omp task shared(X) if (n > TASK_SIZE)
   mergeSortHelper(X, n/2, tmp);

   #pragma omp task shared(X) if (n > TASK_SIZE)
   mergeSortHelper(X+(n/2), n-(n/2), tmp + n/2);

   #pragma omp taskwait
   mergeSortAux(X, n, tmp);
}

void merge_sort_helper(int *array, int l, int r, int dir)
{
   if (l < r)
   {
      // find the midpoint of the array
      int m = l + (r - l) / 2;
#pragma omp parallel sections
      {
#pragma omp section
         {
            merge_sort_helper(array, l, m, dir);
         }
#pragma omp section
         {
            merge_sort_helper(array, m + 1, r, dir);
         }
      }
      merge(array, l, m, r);
   }
}

void merge_sort(int *array, int l, int N, int dir)
{
   int *temp = malloc(N*sizeof(int));
   #pragma omp parallel
   {
      #pragma omp single
      mergeSortHelper(array, N, temp);
   }   
   free(temp);
}

/*
 *End of merge sort method
 */

int evenInput(int *a, int l)
{
   for (int i = 0; i < l; i++)
   {
      if (a[i] % 2 != 0)
      {
         return 0;
      }
   }
   return 1;
}

// TimSort: (InsertionSort + MergeSort Hybrid)
// My min function :)
int min(int a, int b)
{
   return (a > b) ? b : a;
}

// Insertion sort edited. Original source: https://www.programiz.com/dsa/insertion-sort
void insertionSort(int input[], int left, int right)
{
   for (int i = left + 1; i <= right; i++)
   {
      int temp = input[i];
      int j = i - 1;
      while (j >= left && input[j] > temp)
      {
         input[j + 1] = input[j];
         j--;
      }
      input[j + 1] = temp;
   }
}

// Merge sort edited. Original source: https://www.programiz.com/dsa/merge-sort
// USED FOR TIMSORT, NOT ACTUAL MERGE SORT IMPLEMENTATION
void mergeSort(int input[], int left, int mid, int right)
{
   int x = mid - left + 1; // First half size/index
   int y = right - mid;    // Second half size/index

   int *leftArr = malloc(sizeof(int) * x);
   int *rightArr = malloc(sizeof(int) * y);

   for (int i = 0; i < x; i++)
      leftArr[i] = input[left + i];
   for (int i = 0; i < y; i++)
      rightArr[i] = input[mid + 1 + i];

   int i = 0;
   int j = 0;
   int k = left;

   while (i < x && j < y)
   {
      if (leftArr[i] <= rightArr[j])
      {
         input[k] = leftArr[i];
         i++;
      }
      else
      {
         input[k] = rightArr[j];
         j++;
      }
      k++;
   }

   while (i < x)
   {
      input[k] = leftArr[i];
      k++;
      i++;
   }

   while (j < y)
   {
      input[k] = rightArr[j];
      k++;
      j++;
   }
}

// OMP TimSort implementation using InsertionSort and MergeSort in a hybrid manner.
// Parts of code were taken from: https://www.geeksforgeeks.org/timsort/
void timSortHelper(int input[], int n)
{
   int *temp = malloc(n*sizeof(int));
   #pragma omp parallel for shared(input)
   for (int i = 0; i < n; i += RUN)
      insertionSort(input, i, min((i + RUN - 1), (n - 1)));


   #pragma omp task
   for (int size = RUN; size < n; size = size * 2) 
   {
      #pragma omp task
      for (int left = 0; left < n; left += size * 2)
      {
         int mid = left + size - 1;
         int right = min((left + (size * 2) - 1), (n - 1));

         if (mid < right)
            mergeSortAux(input+left, right-left+1, temp);
      }
   }
   free(temp);
   
}

void timSortRecursiveHelper(int *X, int n, int *tmp)
{
   if (n <= RUN) {
      insertionSort(X, 0, n-1);
      return;
   }

   #pragma omp task shared(X) if (n > TASK_SIZE)
   timSortRecursiveHelper(X, n/2, tmp);

   #pragma omp task shared(X) if (n > TASK_SIZE)
   timSortRecursiveHelper(X+(n/2), n-(n/2), tmp + n/2);

   #pragma omp taskwait
   mergeSortAux(X, n, tmp);
}

void timSort(int* input, int left, int N, int direction)
{
   int *temp = malloc(N*sizeof(int));
   #pragma omp parallel
   {
      #pragma omp single
      timSortRecursiveHelper(input, N, temp);
   }   
   free(temp);
}

void setArraySize(int *a, int minSize, int l)
{
   for (int i = 0; i < l-minSize; i++)
   {
      a[i] = pow(2, i + minSize);
      printf("%d\n", a[i]);
   }
}

int runExperiments(int up, int low, int high, int print)
{
   // File nameing
   time_t rawtime;
   char buffer[255];
   char buffer2[300];

   time(&rawtime);
   sprintf(buffer, "data/Group18Data_%s.csv", ctime(&rawtime));
   sprintf(buffer2, "../%s", buffer);
   // Lets convert space to _ in

   char *p = buffer;
   for (; *p; ++p)
   {
      if (*p == ' ')
         *p = '_';
   }
   char *p2 = buffer2;
   for (; *p2; ++p2)
   {
      if (*p2 == ' ')
         *p2 = '_';
   }

   FILE *fpt;
   // Checks if being run from inside c_version or CSDS438 directory
   fpt = fopen(buffer, "w+");
   if (fpt == NULL)
   {
      fpt = fopen(buffer2, "w+");
      if (fpt == NULL)
      {
         assert(0);
         exit(1);
      }
   }
   fprintf(fpt, "Algorithm,Implementation,Thread Count,Elements,Time\n");

   // Experiment value setup
   // 67108864, 16777216, 2097152
   // BITONIC NEEDS POWERS OF 2
   // int minSize = 10;
   // int sizeAmount = 20;
   // int arraySizes[sizeAmount-minSize];
   // setArraySize(arraySizes, minSize, sizeAmount);
   int minSize = 20;
   int sizeAmount = 25;
   int arraySizes[sizeAmount-minSize];
   setArraySize(arraySizes, minSize, sizeAmount);

   assert(evenInput(arraySizes, sizeof(arraySizes) / sizeof(arraySizes[0])));
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

      int threadCount[] = {1, 2, 3, 4, 5, 6, 7, 8, 16, 32};
      int trials = 10;
      for (int trialCount = 0; trialCount < trials; trialCount++)
      {

         // func sortingAlgorithms[] = {&sampleSort};
         // char *sortingNames[] = {"Sample Sort"};

         func sortingAlgorithms[] = {&timSort, &bitonicSortWrapper, &merge_sort, &quickSort, &sampleSort};
         char *sortingNames[] = {"TimSort", "Bitonic Sort", "MergeSort", "QuickSort", "Sample Sort"};

         char implemenation[] = "OpenMP";

         for (int j = 0; j < sizeof(sortingAlgorithms) / sizeof(sortingAlgorithms[0]); j++)
         {
            func sortAlgo = sortingAlgorithms[j];
            for (int k = 0; k < sizeof(threadCount) / sizeof(threadCount[0]); k++)
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
               sortAlgo(X, 0, N, up);
               double end = omp_get_wtime();
               printf("Time: %f (s) \n", end - begin);

               if (print == 1)
               {
                  printArray(X, N);
               }
               // CSV
               fprintf(fpt, "%s, %s, %i, %i, %f\n", sortingNames[j], implemenation, threadCount[k], N, end - begin);
               printf("%s, %s, %i, %i, %f\n", sortingNames[j], implemenation, threadCount[k], N, end - begin);

               // Validates Sorts
               qsort(Y, N, sizeof(int), cmpfunc);
               if (!sameElements(X, Y, N))
               {
                  printArray(X, N);
                  printArray(Y, N);
               }
               assert(isSorted(X, N));
               assert(sameElements(X, Y, N));
            }
         }
         fflush(fpt);
      }
   }
   fclose(fpt);
   return 0;
}

// Driver code
int main(int argc, char *argv[])
{
   srand(123456);
   int print = 0;
   int up = 1; // means sort in ascending order
   runExperiments(up, 0, 500000, print);
   return (EXIT_SUCCESS);
}
