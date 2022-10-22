#pragma once
#include "BaseBitonicSort.h"
class OpenMPBitonicSort :
    public BaseBitonicSort
{
public:
   OpenMPBitonicSort(int* a, int N, int up) :BaseBitonicSort(a, N, up) {}
   void sort();
   void bitonicSort(int* a, int low, int cnt, int dir);
};

