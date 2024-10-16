#include <stdio.h>
#include <stdlib.h>
#include <tgmath.h>

int pivot(int* list, int left, int right);
int select (int* list, int left, int right, int n);
int partition (int* list, int left, int right, int pivotIndex, int n);
int partition5(int* list, int left, int right);
void swap(int* list, int i1, int i2);

int main(void) {
    int list[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    const int length = sizeof(list) / sizeof(int);
    const int n = 15;

    const int index = select(list, 0, length - 1, n - 1);
    return list[index];
}

void swap(int* list, const int i1, const int i2) {
    if (list == NULL) return;
    const int temp = list[i1];
    list[i1] = list[i2];
    list[i2] = temp;
}

int pivot(int* list, int left, const int right) {
    if (right - left < 5) return partition5(list, left, right);
    for(int i = left; i <= right; i = i + 5) {
        int subRight = i + 4;
        if (subRight > right) subRight = right;
        const int median5 = partition5(list, i, subRight);

        swap(list, median5, left + (int) floor((double) (i - left) / 5));
    }
    return select(list, left, left + (int) floor((double) (right - left) / 5), (int) floor((double) (right - left) / 10) + left + 1);
}

int select(int* list, int left, int right, const int n) {
    while (1) {
        if (left == right) return left;
        int pivotIndex = pivot(list, left, right);
        pivotIndex = partition(list, left, right, pivotIndex, n);

        if (n == pivotIndex) return n;
        if (n < pivotIndex) right = pivotIndex - 1;
        else left = pivotIndex + 1;
    }
}

int partition(int* list, const int left, const int right, const int pivotIndex, const int n) {
    if (list == NULL) return -1;
    const int pivotValue = list[pivotIndex];

    swap(list, pivotIndex, right);
    int storeIndex = left;

    for (int i = left; i < right; i++) {
        if (list[i] < pivotValue) {
            swap(list, storeIndex, i);
            storeIndex++;
        }
    }

    int storeIndexEq = storeIndex;
    for(int i = storeIndex; i < right; i++) {
        if (list[i] == pivotValue) {
            swap(list, storeIndexEq, i);
            storeIndexEq++;
        }
    }

    swap(list, right, storeIndexEq);

    if (n < storeIndex) return storeIndex;
    if (n <= storeIndexEq) return n;
    return storeIndexEq;
}

int partition5(int* list, const int left, const int right) {
    int i = left + 1;
    while (i <= right) {
        int j = i;
        while (j > left && list != NULL && list[j-1] > list[j]) {
            swap(list, j-1, j);
            j--;
        }
        i++;
    }
    return left + (right - left) / 2;
}