
#ifndef ALGORITHMS_H
#define ALGORITHMS_H

void mergeSort(int arr[], int left, int right);
void quickSort(int arr[], int low, int high);

int binarySearchIterative(
    const int arr[], int size, int target
);

int binarySearchRecursive(
    const int arr[], int left, int right, int target
);

#endif
