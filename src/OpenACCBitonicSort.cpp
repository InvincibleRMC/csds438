#include "OpenACCBitonicSort.h"
#include "Constants.h"

void OpenACCBitonicSort::sort()
{

   // #pragma acc parallel num_gangs(1) num_workers (1)
   {
      bitonicSort(this->getA(), 0, this->getSize(), this->getDirection());
   }
   
}

void OpenACCBitonicSort::bitonicSort(int* a, int low, int cnt, int dir)
{
   if (cnt > 1)
   {
      int k = cnt / 2;
      // sort in ascending order since dir here is 1
      bitonicSort(a, low, k, 1);

      // sort in descending order since dir here is 0
      bitonicSort(a, low + k, k, 0);

      // Will merge whole sequence in ascending order
      // since dir=1.
      bitonicMerge(low, cnt, dir);
   }
}
