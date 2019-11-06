#include <stdio.h>
#include <syslog.h>
#include <stdarg.h>
#include <string.h>
#include "logger.h"

#define RESET		0
#define BRIGHT 		1
#define HIDDEN		8

#define BLACK 		0
#define RED		    1
#define L_RED       61
#define L_YELLOW    63
#define BLUE		4
#define	WHITE		9

int sysFlag = 0;

int initLogger(char *logType) {

    printf("Initializing Logger on: %s\n", logType);
    int result = 0;

    if (strcmp("syslog", logType) == 0) {
        result = sysFlag = 1;
    } else if (strcmp("stdout", logType) == 0 || strcmp("", logType) == 0){
        result = 1;
    }

    return result;
}

void textcolor(int attr, int fg, int bg)
{	char command[13];
    sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
    printf("%s", command);
}

int infof (const char *format, ...){

    // variable-length argument list
    va_list arg;

    if (sysFlag == 1){
        va_start(arg, format);
        vsyslog(LOG_USER | LOG_INFO, format, arg);
        va_end(arg);
        return 1;
    } else {
        textcolor(BRIGHT, WHITE, BLUE);
        printf("INFO:");
        textcolor(RESET, WHITE, HIDDEN);
        printf(" ");

        va_start(arg, format); // initializing arguments to store all values after format
        int done = vprintf(format, arg);
        va_end(arg); // cleans up the list
        printf("\n");
        return done;
    }
}

int warnf (const char *format, ...){
    // variable-length argument list
    va_list arg;

    if (sysFlag == 1){
        va_start(arg, format);
        vsyslog(LOG_USER | LOG_WARNING, format, arg);
        va_end(arg);
        return 1;
    } else {
        textcolor(BRIGHT, BLACK, L_YELLOW);
        printf("WARNING:");
        textcolor(RESET, WHITE, HIDDEN);
        printf(" ");

        va_start(arg, format); // initializing arguments to store all values after format
        int done = vprintf(format, arg);
        va_end(arg); // cleans up the list
        printf("\n");
        return done;
    }
}

int errorf (const char *format, ...){
    // variable-length argument list
    va_list arg;

    if (sysFlag == 1){
        va_start(arg, format);
        vsyslog(LOG_USER | LOG_ERR, format, arg);
        va_end(arg);
        return 1;
    } else {
        textcolor(BRIGHT, WHITE, RED);
        printf("ERROR:");
        textcolor(RESET, WHITE, HIDDEN);
        printf(" ");

        va_start(arg, format); // initializing arguments to store all values after format
        int done = vprintf(format, arg);
        va_end(arg); // cleans up the list
        printf("\n");
        return done;
    }
}

int panicf (const char *format, ...){
    // variable-length argument list
    va_list arg;

    if (sysFlag == 1){
        va_start(arg, format);
        vsyslog(LOG_USER | LOG_EMERG, format, arg);
        va_end(arg);
        return 1;
    } else {
        textcolor(BRIGHT, WHITE, L_RED);
        printf("PANIC:");
        textcolor(RESET, WHITE, HIDDEN);
        printf(" ");

        va_start(arg, format); // initializing arguments to store all values after format
        int done = vprintf(format, arg);
        va_end(arg); // cleans up the list
        printf("\n");
        return done;
    }
}
