#include "OpenMPBitonicSort.h"
#include "Constants.h"
#include <omp.h>

void OpenMPBitonicSort::sort()
{

   #pragma omp parallel
   {
   #pragma omp single
      BaseBitonicSort::sort();
   }
}

void OpenMPBitonicSort::bitonicSort(int* a, int low, int cnt, int dir)
{
   if (cnt > 1)
   {
      int k = cnt / 2;
      // sort in ascending order since dir here is 1
      #pragma omp task shared(a) if (k>constants::TASK_SIZE) 
      bitonicSort(a, low, k, 1);

      // sort in descending order since dir here is 0
      #pragma omp task shared(a) if (k>constants::TASK_SIZE) 
      bitonicSort(a, low + k, k, 0);

      // Will merge whole sequence in ascending order
      // since dir=1.
      #pragma omp taskwait
      bitonicMerge(low, cnt, dir);
   }
}
