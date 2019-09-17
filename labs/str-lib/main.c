#include <stdio.h>
#include "strlib.c"

int main(int argc, char **argv[]) {
    if (argc == 4) {
        int len = mystrlen(argv[1]);
        char* add = mystradd(argv[1], argv[2]);
        char* find = mystrfind(add, argv[3]) ? "yes":"no";
        printf("Initial Length: %d\nNew String: %s\nSubString was found: %s\n", len, add, find);
        return 0;
    } else {
        printf("U need to send the correct number of arguments...\n");
        printf("How to run: ./main <originalSt> <addSt> <addSt>\n");
        return -1;
    }
}
