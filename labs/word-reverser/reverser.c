#include <stdio.h>

void reverse(char next) {
    if (next != '\n') {
        reverse(getchar());
        putchar(next);
    }
}

int main() {
    int c;
    while ((c = getchar()) != EOF) {
        reverse(c);
        printf("\n");
    }
}
