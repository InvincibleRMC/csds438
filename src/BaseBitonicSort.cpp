#include "BaseBitonicSort.h"

void BaseBitonicSort::sort()
{
   bitonicSort(this->getA(), 0, this->getSize(), this->getDirection());
}

void BaseBitonicSort::bitonicMerge(int low, int cnt, int dir)
{
   if (cnt > 1)
   {
      int k = cnt / 2;
      for (int i = low; i < low + k; i++)
         this->compAndSwap(i, i + k, dir);
      bitonicMerge(low, k, dir);
      bitonicMerge(low + k, k, dir);
   }
}

void BaseBitonicSort::bitonicSort(int* a, int low, int cnt, int dir)
{
}
