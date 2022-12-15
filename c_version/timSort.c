//Hybrid TimSort using InsertionSort and MergeSort
#include <stdio.h>
const int RUN = 32; // Size of a run. Must be a multiple of 2. 32 or 64 are standard choices.

//My min function :)
int min(int a, int b){
	return (a > b ) ? b : a;
}

//Insertion sort edited. Original source: https://www.programiz.com/dsa/insertion-sort
void insertionSort(int input[], int left, int right){
	for(int i = left + 1; i <= right; i++){
		int temp = input[i];
		int j = i - 1;
		while (j >= left && input[j] > temp){
			input[j+1] = input[j];
			j--;
		}
		input[j+1] = temp;
	}
}

//Merge sort edited. Original source: https://www.programiz.com/dsa/merge-sort
void mergeSort(int input[], int left, int mid, int right){
	int x = mid- left + 1; //First half size/index
	int y = right - mid;  //Second half size/index

	int leftArr[x];
	int rightArr[y];

	for(int i = 0; i < x; i++)
		leftArr[i] = input[left+i];
	for(int i = 0; i < y; i++)
		rightArr[i] = input[mid+1+i];

	int i = 0;
	int j = 0;
	int k = left;

	while(i < x && j < y){
		if(leftArr[i] <= rightArr[j]){
			input[k] = leftArr[i];
			i++;
		}
		else{
			input[k] = rightArr[j];
			j++;
		}
		k++;
	}

	while(i < x){
		input[k] = leftArr[i];
		k++;
		i++;
	}

	while(j < y){
		input[k] = rightArr[j];
		k++;
		j++;
	}
}

//OMP TimSort implementation using InsertionSort and MergeSort in a hybrid manner.
//Parts of code were taken from: https://www.geeksforgeeks.org/timsort/
void timSortHelper(int input[], int n){
	#pragma omp parallel for
	for(int i = 0; i < n; i+=RUN)
		insertionSort(input, i, min((i+RUN-1), (n-1)));

	for(int size = RUN; size < n; size = size*2){
		#pragma omp parallel for
		for(int left = 0; left < n; left += size*2){
			int mid = left + size - 1;
			int right = min((left + (size*2) - 1), (n-1));

			if(mid < right)
				mergeSort(input, left, mid, right);
		}
	}
}

void timSort(int array[], int l, int r, int dir){
	timSortHelper(array, r-1);
}

//Testing code
void printArray(int arr[], int n) {
    for (int i = 0; i < n; i++)
        printf("%d  ", arr[i]);
    printf("\n");
}

int main() {
    int arr[] = {-2, 7, 15, -14, 0, 15, 0, 7, -7,
                       -4, -13, 5, 8, -14, 12};
    int n = sizeof(arr)/sizeof(arr[0]);
    printf("Given Array is\n");
    printArray(arr, n);

    // Function Call
    timSort(arr, n, n, 0);

    printf("After Sorting Array is\n");
    printArray(arr, n);
    return 0;
}