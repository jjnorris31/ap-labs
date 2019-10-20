// References:
// example of inotify https://www.thegeekstuff.com/2010/04/inotify-c-program-example/
// documentation of nftw https://linux.die.net/man/3/nftw

#define _XOPEN_SOURCE 500

#include <signal.h>
#include <stdio.h>
#include "logger.h"
#include <sys/inotify.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <ftw.h>

#define EVENT_SIZE  (sizeof (struct inotify_event))
#define EVENT_BUF_LEN ( 4096 * ( EVENT_SIZE + 16 ))

int inst_notify;
int watcher;
char buffer[EVENT_BUF_LEN];
int length;
static volatile int keepRunning = 1;

void intHandler(int dummy) {
    keepRunning = 0;
}

static void displayEvents(struct inotify_event *event) {
    if (event->len) {
        if (event->mask & IN_CREATE) {
            if (event->mask & IN_ISDIR) {
                infof("monitor -> new directory: %s", event->name);
            } else {
                infof("monitor -> new file created: %s", event->name);
            }
        } else if (event->mask & IN_DELETE) {
            if (event->mask & IN_ISDIR) {
                infof("monitor -> directory deleted: %s", event->name);
            } else {
                infof("monitor -> file deleted: %s", event->name);
            }
        } else if (event->mask & IN_MOVED_TO | IN_MOVED_FROM) {
            if (event->mask & IN_ISDIR) {
                infof("monitor -> directory modified: %s", event->name);
            } else {
                infof("monitor -> file modified: %s", event->name);
            }
        }
    }
}

static int display_info(const char *fpath, const struct stat *sb, int flag, struct FTW *ftwbuf) {

    // todo validate the path
    infof(fpath);
    watcher = inotify_add_watch(inst_notify, fpath, IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO);

    if (watcher < 0) {
        errorf("monitor -> inotify cannot start the watcher");
        exit(EXIT_FAILURE);
    }

    return 0;
}

int main(int argc, char *argv[]) {

    int length = 0;
    char *p;
    char key;

    inst_notify = inotify_init();
    signal(SIGINT, intHandler);

    // error al instanciar inotify
    if (inst_notify < 0) {
        errorf("monitor: cannot instantiate inotify");
        exit(EXIT_FAILURE);
    }

    if (nftw((argc < 2) ? "." : argv[1], display_info, 20, FTW_PHYS) == -1) {
        panicf("monitor: nftw encounters an error");
        exit(EXIT_FAILURE);
    }


    while (keepRunning) {
        length = read(inst_notify, buffer, EVENT_BUF_LEN);

        // error al leer el path
        if (length < 0) {
            panicf("monitor: inotify cannot read the path");
            break;
        } else if (length == 0) {
            panicf("monitor: inotify read returns 0");
        }

        for (p = buffer; p < buffer + length;) {
            struct inotify_event *event = (struct inotify_event *)p;
            displayEvents(event);
            p += EVENT_SIZE + event->len;
        }

        inst_notify = inotify_init();
        if (nftw((argc < 2) ? "." : argv[1], display_info, 20, FTW_PHYS) == -1) {
            panicf("monitor: nftw encounters an error");
        }
    }

    inotify_rm_watch(inst_notify, watcher); // removing directory from the watch list
    close(inst_notify); // closing inotify instance
    infof("monitor: stopping monitor");
    exit(EXIT_SUCCESS);
}

