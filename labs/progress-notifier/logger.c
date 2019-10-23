#include <stdio.h>
#include <stdarg.h>

#define RESET		0
#define BRIGHT 		1
#define HIDDEN		8

#define BLACK 		0
#define RED		    1
#define L_RED       61
#define L_YELLOW    63
#define BLUE		4
#define	WHITE		9

/* infof prototype */
int infof(const char *format, ...);
/* warnf prototype */
int warnf(const char *format, ...);
/* errorf prototype */
int errorf(const char *format, ...);
/* panicf prototype */
int panicf(const char *format, ...);

void textcolor(int attr, int fg, int bg)
{	char command[13];
    sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
    printf("%s", command);
}

int infof (const char *format, ...){
    // variable-length argument list
    va_list arg;
    int done;

    textcolor(BRIGHT, WHITE, BLUE);
    printf("INFO:");
    textcolor(RESET, WHITE, HIDDEN);
    printf(" ");

    va_start(arg, format); // initializing arguments to store all values after format
    done = vprintf(format, arg);
    va_end(arg); // cleans up the list
    printf("\n");
    return done;
}

int warnf (const char *format, ...){
    // variable-length argument list
    va_list arg;
    int done;

    textcolor(BRIGHT, BLACK, L_YELLOW);
    printf("WARNING:");
    textcolor(RESET, WHITE, HIDDEN);
    printf(" ");

    va_start(arg, format); // initializing arguments to store all values after format
    done = vprintf(format, arg);
    va_end(arg); // cleans up the list
    printf("\n");
    return done;
}

int errorf (const char *format, ...){
    // variable-length argument list
    va_list arg;
    int done;

    textcolor(BRIGHT, WHITE, RED);
    printf("ERROR:");
    textcolor(RESET, WHITE, HIDDEN);
    printf(" ");

    va_start(arg, format); // initializing arguments to store all values after format
    done = vprintf(format, arg);
    va_end(arg); // cleans up the list
    printf("\n");
    return done;
}

int panicf (const char *format, ...){
    // variable-length argument list
    va_list arg;
    int done;

    textcolor(BRIGHT, WHITE, L_RED);
    printf("PANIC:");
    textcolor(RESET, WHITE, HIDDEN);
    printf(" ");

    va_start(arg, format); // initializing arguments to store all values after format
    done = vprintf(format, arg);
    va_end(arg); // cleans up the list
    printf("\n");
    return done;
}