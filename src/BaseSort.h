#pragma once
class BaseSort
{
public: 
   BaseSort(int* a, int N, int up);
   void printArray();
   int isSorted();
   int getSize();
   int getDirection();
   int* getA();
   /*The parameter dir indicates the sorting direction, ASCENDING
      or DESCENDING; if (a[i] > a[j]) agrees with the direction,
      then a[i] and a[j] are interchanged.*/
   void compAndSwap(int i, int j, int dir);
   virtual void sort();

private:
   int* a;
   int N;
   int up;
   void printArray(int* a, int size);
};

