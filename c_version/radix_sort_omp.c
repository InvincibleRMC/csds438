#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#define BUCKETS 10
void radix_sort(int *array, int low_i, int high_i, int direction_of_sort) {
   int max = 0;
   int size = high_i - low_i;
  int i, exp;
  for (i = 0; i < size; i++) {
    if (array[i] > max) {
      max = array[i];
    }
  }
  for (exp = 1; max / exp > 0; exp *= BUCKETS) {
    int buckets[BUCKETS] = {0};
    for (i = 0; i < size; i++) {
      int digit = (array[i] / exp) % BUCKETS;
      buckets[digit]++;
    }
    for (i = 1; i < BUCKETS; i++) {
      buckets[i] += buckets[i - 1];
    }
    int *sorted = malloc(size * sizeof(int));
    memcpy(sorted, array, size * sizeof(int));
    for (i = size - 1; i >= 0; i--) {
      int digit = (sorted[i] / exp) % BUCKETS;
      array[--buckets[digit]] = sorted[i];
    }
    free(sorted);
  }
}
int main(int argc, char *argv[]) {
  int i;
  int threads = 1;
  if (argc > 1) {
    threads = atoi(argv[1]);
  }
  omp_set_num_threads(threads);
  int size = 1000000;
  int *array = malloc(size * sizeof(int));
  for (i = 0; i < size; i++) {
    array[i] = rand();
  }
  double start = omp_get_wtime();
  radix_sort(array, 0, 1000000, 0);
  double end = omp_get_wtime();
  for(i=0; i < 100; ++i){
  printf("%d ",array[i]);
}
  printf("Sorted %d elements in %.6f seconds\n", size, end - start);
  free(array);
  return 0;
}
