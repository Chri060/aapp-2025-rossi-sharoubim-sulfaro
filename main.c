#include <stdio.h>
#include <stdlib.h>
#include <tgmath.h>

int pivot(int* list, int left, int right);
int select (int* list, int left, int right, int n);
int partition (int* list, int left, int right, int pivotIndex, int n);
int partition5(int* list, int left, int right);
void swap(int* list1, int *list2, int i1, int i2);


int main(void) {
    int n, length, index;
    int* list;

    printf("Index i\n");
    scanf_s("%d", &n);
    printf("Array length\n");
    scanf_s("%d", &length);
    list = (int*)malloc(length * sizeof(int));
    for (int i = 0; i < length; i++) {
        scanf_s("%d", &list[i]);
    }
    index = select(list, 0, length - 1, n - 1);
    return list[index];
}

void swap(int* list1, int *list2, const int i1, const int i2) {
    const int temp = list1[i1];
    list1[i1] = list2[i2];
    list2[i2] = temp;
}

int pivot(int* list, int left, int right) {
    if (right - left < 5) return partition5(list, left, right);
    for(int i = left; i < right; i = i + 5) {
        int subRight = i + 4;
        if (subRight > right) subRight = right;
        int median5 = partition5(list, i, subRight);

        swap(list, list, median5, left + floor((i - left) / 5));
    }

    int mid = floor((right - left) / 10) + left + 1;
    return select(list, left, left + floor((right - left) / 5), mid);
}

int select (int* list, int left, int right, int n) {
    int pivotIndex = 0;

    while (1) {
        if (left == right) {
            return left;
        }
        pivotIndex = pivot(list, left, right);
        pivotIndex = partition(list, left, right, pivotIndex, n);

        if (n == pivotIndex) return n;

        if (n < pivotIndex) right = pivotIndex - 1;
        else left = pivotIndex + 1;
    }
}

int partition (int* list, int left, int right, int pivotIndex, int n) {
    int pivotValue = list[pivotIndex];

    swap(list, list, pivotIndex, right);
    int storeIndex = left;


    for (int i = left; i < right - 1; i++) {
        if (list[i] < pivotValue) {
            swap(list, list, storeIndex, i);
            storeIndex++;
        }
    }

    int storeIndexEq = storeIndex;
    for(int i = storeIndex; i < right - 1; i++) {
        if (list[i] = pivotValue) {
            swap(list, list, storeIndexEq, i);
            storeIndexEq++;
        }

    }

    swap(list, list, right, storeIndexEq);

    if (n < storeIndex) return storeIndex;
    if (n <= storeIndexEq) return n;
    return storeIndexEq;

}

int partition5(int* list, int left, int right) {
    int i = left + 1;
    while (i <= right) {
        int j = i;
        while (j > left && list[j-1] > list[j]) {
            swap(list, list, j-1, j);
            j--;
        }
        i++;
    }
    return left + (right - left) / 2;
}