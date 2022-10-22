#pragma once
#include "BaseSort.h"
class BaseBitonicSort :
    public BaseSort
{
   public: 
      BaseBitonicSort(int* a, int N, int up) :BaseSort(a, N, up) {}
      /* Caller of bitonicSort for sorting the entire array of
         length N in ASCENDING order */
      virtual void sort();
      /*It recursively sorts a bitonic sequence in ascending order,
        if dir = 1, and in descending order otherwise (means dir=0).
        The sequence to be sorted starts at index position low,
        the parameter cnt is the number of elements to be sorted.*/
      virtual void bitonicMerge(int low, int cnt);
      /* This function first produces a bitonic sequence by recursively
          sorting its two halves in opposite sorting orders, and then
          calls bitonicMerge to make them in the same order */
      virtual void bitonicSort(int* a, int low, int cnt, int dir);
};

