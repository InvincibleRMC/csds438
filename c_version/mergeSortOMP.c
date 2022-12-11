#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
// merge two sorted arrays arr1[0..n1-1] and arr2[0..n2-1] into arr[0..n1+n2-1]

void merge(int *array, int l, int m, int r) {
  int size1 = m - l + 1;
  int size2 = r - m;

  int *left = malloc(size1 * sizeof(int));
  int *right = malloc(size2 * sizeof(int));

  memcpy(left, array + l, size1 * sizeof(int));
  memcpy(right, array + m + 1, size2 * sizeof(int));

  int i = 0, j = 0, k = l;
  // compare each element of the two arrays and
  // put the smaller element in the result array
  while (i < size1 && j < size2) {
    if (left[i] <= right[j]) {
      array[k++] = left[i++];
    } else {
      array[k++] = right[j++];
    }
  }
  // put the remaining elements of arr1[] (if any) into arr[]
  while (i < size1) {
    array[k++] = left[i++];
  }
  while (j < size2) {
    array[k++] = right[j++];
  }

  free(left);
  free(right);
}
// put the remaining elements of arr2[] (if any) into arr[]

void merge_sort(int *array, int l, int r) {
  if (l < r) {
    // find the midpoint of the array
    int m = l + (r - l) / 2;

    #pragma omp parallel sections
    {
      #pragma omp section
      {
        merge_sort(array, l, m);
      }
      #pragma omp section
      {
        merge_sort(array, m + 1, r);
      }
    }

    merge(array, l, m, r);
  }
}

int main(int argc, char *argv[]) {
  int threads = 4;
  int i;
  if (argc > 1) {
    threads = atoi(argv[1]);
  }
  // sort the two halves using parallel threads

  omp_set_num_threads(threads);

  int size = 1000000;
  int *array = malloc(size * sizeof(int));
  for (i = 0; i < size; i++) {
    array[i] = rand();
  }

  double start = omp_get_wtime();
  merge_sort(array, 0, size - 1);
  double end = omp_get_wtime();
  for(i = 0; i < 100; ++i){
  printf("%d ", array[i]);
}
  printf("Sorted %d elements in %.6f seconds\n", size, end - start);

  free(array);
}

