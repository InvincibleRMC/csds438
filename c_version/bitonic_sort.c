#include <stdio.h>
#include <stdlib.h>

#define N 32
#define MAX_LEVELS 10

// Bitonic sort function for sorting array x in ascending order
/*void bitonic_sort(int *x) {
  int i, j, k;

  // Loop over the number of levels
  for (k = 0; k < MAX_LEVELS; k++) {
    // Loop over the number of subarrays for this level
    for (j = 0; j < (1 << k); j++) {
      // Loop over the number of elements in each subarray
      for (i = 0; i < (1 << (MAX_LEVELS - k)); i++) {
        int ixj = i + j;

        // Compare elements and swap if necessary
        int l;
        #pragma acc parallel loop
        for (l = 0; l < (1 << (MAX_LEVELS - k - 1)); l++) {
          int a = x[ixj + (l << (MAX_LEVELS - k))];
          int b = x[ixj + (1 << (MAX_LEVELS - k - 1)) + (l << (MAX_LEVELS - k))];

          if (a > b) {
            int temp = a;
            a = b;
            b = temp;
          }
        }
      }
    }
  }
}*/

void bitonic_sort(int x[N]) {
  int i, j, k;

  // Loop over the number of levels
  #pragma acc parallel loop
  for (k = 0; k < N; k++) {
    // Loop over the number of subarrays for this level
    for (j = 0; j < N; j++) {
      // Loop over the number of elements in each subarray
      for (i = 0; i < N; i++) {
        int ixj = i ^ j;

        // Compare elements and swap if necessary
        if (ixj > i && x[ixj] < x[i]) {
          int temp = x[ixj];
          x[ixj] = x[i];
          x[i] = temp;
        }
      }
    }
  }
}


int main(void) {
    // Create and initialize the array to be sorted
    int *x = malloc(N * sizeof(int));
    if (x == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        return 1;
    }
    int i;
    for (i = 0; i < N; i++) {
        x[i] = rand();
    }

    // Print the sorted array
    for (i = 0; i < N; i++) {
        printf("%d ", x[i]);
    }
    printf("\n ");

    // Sort the array
    bitonic_sort(x);

    // Print the sorted array
    for (i = 0; i < N; i++) {
        printf("%d ", x[i]);
    }

    // Verify the results
    for (i = 0; i < N - 1; i++)
    {
        if (x[i] > x[i + 1])
        {
            printf("Error: array is not sorted\n");
            return 1;
        }
    }

    free(x);
    return 0;
}
