#include "BaseBitonicSort.h"

void BaseBitonicSort::sort()
{
   bitonicSort(this->getA(), 0, this->getSize(), this->getDirection());
}

void BaseBitonicSort::bitonicMerge(int low, int cnt)
{
}

void BaseBitonicSort::bitonicSort(int* a, int low, int cnt, int dir)
{
}
