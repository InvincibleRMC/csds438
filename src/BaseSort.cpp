#include "BaseSort.h"
#include <stdio.h>
#include <iostream>

BaseSort::BaseSort(int* a, int N, int up)
{
   this->a = a;
   this->N = N;
   this->up = up;
}

void BaseSort::printArray()
{
   return printArray(this->a, this->getSize());
}

void BaseSort::printArray(int* a, int size)
{
   for (int i = 0; i < size; i++)
      printf("%d ", a[i]);
   printf("\n");
}

int BaseSort::isSorted()
{
   for (int i = 0; i < this->getSize() - 1; i++)
      if (this->a[i] > this->a[i + 1])
         return 0;
   return 1;
}

int BaseSort::getSize()
{
   return this->N;
}

int BaseSort::getDirection()
{
   return this->up;
}

int* BaseSort::getA()
{
   return this->a;
}

void BaseSort::compAndSwap(int i, int j, int dir)
{
   if (dir == (this->a[i] > this->a[j]))
      std::swap(this->a[i], this->a[j]);
}

void BaseSort::sort()
{
}
