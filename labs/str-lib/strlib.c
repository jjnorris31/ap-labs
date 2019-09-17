#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int mystrlen(char *str){
    int n;
    for (n = 0; *str != '\0'; str++) {
        n++;
    }

    return n;
}

char *mystradd(char *origin, char *addition){
    char* result = malloc(sizeof(char)*mystrlen(origin) + mystrlen(addition));
    strcpy(result, origin);
    strcpy(result + mystrlen(origin), addition);
    return result;
}

int mystrfind(char *origin, char *substr){
    int n;
    int o;
    int tempn;
    int res = 0;
    for (n = 0; origin[n] != '\0'; n++) {
        tempn = n;
        for (o = 0; substr[o] != '\0'; o++){
            if (origin[tempn] == substr[o]) {
                tempn++;
                res = 1;
            } else {
                res = 0;
                break;
            }
        }
        if (res == 1) {
            return 1;
        }
    }
    return res;
}