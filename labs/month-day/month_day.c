#include <stdio.h>

typedef char * string;

int main(int argc, char **argv[]) {

    string month;
    int day;

    if (argc == 3) {
        if (atoi(argv[1]) < 0) {
            printf("Please, give me positive year number...\n");
            return 1;
        } else if (atoi(argv[2]) > 366) {
            printf("Please give me a number less than 366...\n");
            return 1;
        } else {
            month_day(atoi(argv[1]), atoi(argv[2]), &month, &day);
        }
    } else {
        for (int i = 0; i < 366; i++) {}
        printf("U need to send the correct number of arguments...\n");
        printf("How to run: ./md <year> <days>\n");
        return 1;
    }

    printf(month);
    printf(" %d,", day);
    printf(" %d\n", atoi(argv[1]));
}

int isLeap(int year) {
    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)){
        return 1;
    } else {
        return 0;
    }
}

int month_day(int year, int yearDay, string *pMonth, int *pDay) {

    int febDays = isLeap(year) ? 29 : 28;
    int carry = 0;
    int current = 0;

    int monthDays[12]= {31, febDays, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    string monthNames[12] = {"Ene", "Feb", "Mar", "Abr", "May", "Jun", "Jul", "Ago", "Sep", "Oct", "Nov", "Dic"};

    for (int i = 0; i < 12; i++) {
        carry += monthDays[i];
        if (yearDay <= carry) {
            *pMonth = monthNames[i];
            *pDay = yearDay - current;
            break;
        } else {
            current += monthDays[i];
        }
    }
}

