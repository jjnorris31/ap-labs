#include <stdio.h>
#include <string.h>

char *strArray[5] = {"has", "aff", "ac", "ad", "as"};
int intArray[5] = {1, 2, 5, 4, 3};
int intAux[5];
char *strAux[5];

int intcmp(int x, int y){

    int result = 0;

    if ((x - y) < 0){
        result = -1;
    } else if ((x - y) > 0) {
        result = 1;
    }

    return result;
}


void intMerge(int init, int mid, int last) {

    int beginHalf1 = init;
    int beginHalf2 = mid + 1;
    int index = init;

    while ((beginHalf1 <= mid) && (beginHalf2 <= last)) {
        if (intcmp(intArray[beginHalf1], intArray[beginHalf2]) <= 0) {
            intAux[index] = intArray[beginHalf1++];
        } else {
            intAux[index] = intArray[beginHalf2++];
        }
        index++;
    }

    while (beginHalf1 <= mid) {
        intAux[index++] = intArray[beginHalf1++];
    }

    while (beginHalf2 <= last) {
        intAux[index++] = intArray[beginHalf2++];
    }

    for (index = init; index <= last; index++){
        intArray[index] = intAux[index];
    }

}

void strMerge(int init, int mid, int last) {

    int beginHalf1 = init;
    int beginHalf2 = mid + 1;
    int index = init;

    while ((beginHalf1 <= mid) && (beginHalf2 <= last)) {
        if (strcmp(strArray[beginHalf1], strArray[beginHalf2]) <= 0) {
            strAux[index] = strArray[beginHalf1++];
        } else {
            strAux[index] = strArray[beginHalf2++];
        }
        index++;
    }

    while (beginHalf1 <= mid) {
        strAux[index++] = strArray[beginHalf1++];
    }

    while (beginHalf2 <= last) {
        strAux[index++] = strArray[beginHalf2++];
    }

    for (index = init; index <= last; index++){
        strArray[index] = strAux[index];
    }

}

void sort(int first, int last, char type) {

    int mid;

    if (type == 'i') {
        if (first < last) {
            mid = (first + last) / 2;
            sort(first, mid, 'i');
            sort((mid + 1), last, 'i');
            intMerge(first, mid, last);
        } else {
            return;
        }
    } else if (type == 's'){
        if (first < last) {
            mid = (first + last) / 2;
            sort(first, mid, 's');
            sort((mid + 1), last, 's');
            strMerge(first, mid, last);
        } else {
            return;
        }
    }
}

int main() {
    printf("Before sort: ");
    for (int i = 0; i < 5; i++){
        printf("%d ", intArray[i]);
    }
    printf("\n");
    printf("After sort: ");
    sort(0, 4, 'i');
    for (int i = 0; i < 5; i++){
        printf("%d ", intArray[i]);
    }
    printf("\n");

    printf("Before sort: ");
    for (int i = 0; i < 5; i++){
        printf(strArray[i]);
        printf(" ");
    }
    printf("\n");
    printf("After sort: ");
    sort(0, 4, 's');
    for (int i = 0; i < 5; i++){
        printf(strArray[i]);
        printf(" ");
    }
    printf("\n");
}