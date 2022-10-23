#pragma once
#include "BaseBitonicSort.h"
class OpenACCBitonicSort :
    public BaseBitonicSort
{
public: 
   OpenACCBitonicSort(int* a, int N, int up) :BaseBitonicSort(a, N, up) {}
   void sort();
   void bitonicSort(int* a, int low, int cnt, int dir);
};

