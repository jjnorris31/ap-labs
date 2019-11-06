#include "logger.c"

int infof(const char *format, ...);
int warnf(const char *format, ...);
int errorf(const char *format, ...);
int panicf(const char *format, ...);

int main()
{
    infof("Outputs an informational message to the console.\n");
    warnf("Write a warning message in the console.\n");
    errorf("An important message representing an unresolved failure.\n");
    panicf("Errors that we aren’t prepared to handle gracefully.\n");
    return 0;
}