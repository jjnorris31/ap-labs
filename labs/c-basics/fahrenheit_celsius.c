#include <stdio.h>

int main(int argc, char **argv[]) {

    int fahr;
    int start;
    int end;
    int increment;

    if (argc == 2) {
        fahr = atoi(argv[1]);
        printf("Fahrenheit: %d, Celsius: %.1f\n", fahr, (5.0 / 9.0) * (fahr - 32));
        return 0;
    } else if (argc == 4) {
        start = atoi(argv[1]);
        end = atoi(argv[2]);
        increment = atoi(argv[3]);
        for (int i = start; i <= end; i += increment) {
            printf("Fahrenheit: %d, Celsius: %.1f\n", i, (5.0 / 9.0) * (i - 32));
        }
        return 0;
    } else {
        printf("U need to send the correct number of arguments...\n");
        printf("How to run: ./fc <num_fahrenheit_degrees> OR ./fc <start> <end> <increment>\n");
        return 1;
    }
}
