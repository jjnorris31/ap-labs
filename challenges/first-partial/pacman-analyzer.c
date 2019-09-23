#include <stdio.h>
#include <stdlib.h> // for exit() function
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "gtln.c"

#define REPORT_FILE "packages_report.txt"
#define MAX_LIST_SIZE 1000

int rm_pkg, up_pkg, ins_pkg;
char *line_buff = NULL;
int out_fd;
ssize_t lnin_size = 0;
size_t line_buf_size = 0;
int line_count = 0;

struct pkg {
    char *name;
    char *install_date;
};

/* prototype functions */
int mystrfind(char *origin, char *substr);
struct pkg findpkg(char *origin, char *substr, int flag);
struct pkg pkg_list[MAX_LIST_SIZE];
void analizeLog(char *logFile, char *report);

int main(int argc, char **argv) {

    if (argc < 2) {
	printf("Usage: ./pacman-analizer.o pacman.log\n");
	return 1;
    }

    analizeLog(argv[1], REPORT_FILE);

    return 0;
}

void analizeLog(char *logFile, char *report) {
    printf("Generating Report from: [%s] log file\n", logFile);

    /* read */
    FILE *fp = fopen(logFile, "r");

    /* write */
    out_fd = open(report, O_WRONLY | O_CREAT, 0644);
    if (out_fd == -1) {
        printf("Failed to create and open the file.\n");
        exit(1);
    }

    write(out_fd, "Pacman Packages Report\n", 23);
    write(out_fd, "----------------------\n", 23);


    lnin_size = getline(&line_buff, &line_buf_size, fp);
    rm_pkg = up_pkg = ins_pkg = 0;

    int pkg_count = 0;

    while (lnin_size >= 0) {

        /* increment our line count */
        line_count++;
        /* manipulate the line */
        if (mystrfind(line_buff, "[ALPM] installed")){
            printf("\nPackage name : ");
            printf(findpkg(line_buff, "installed", 1).install_date);
            pkg_list[pkg_count] = findpkg(line_buff, "installed", 1);
            pkg_count++;
            printf("\n");
            ins_pkg++;
        } else if (mystrfind(line_buff, "removed")){
            rm_pkg++;
        } else if (mystrfind(line_buff, "upgraded")){
            up_pkg++;
        }

        /* get the next line */
        lnin_size = getline(&line_buff, &line_buf_size, fp);
    }

    // write out log file
    write(out_fd, "- Installed packages : ", 23);
    char str[12];
    sprintf(str, "%d", ins_pkg);
    write(out_fd, str, strlen(str));
    write(out_fd, "\n- Removed packages : ", 22);
    sprintf(str, "%d", rm_pkg);
    write(out_fd, str, strlen(str));
    write(out_fd, "\n- Upgraded packages : ", 23);
    sprintf(str, "%d", up_pkg);
    write(out_fd, str, strlen(str));
    write(out_fd, "\n- Current installed : ", 24);
    int current = ins_pkg - rm_pkg;
    sprintf(str, "%d", current);
    write(out_fd, str, strlen(str));

    write(out_fd, "\n\nList of packages\n", 19);
    write(out_fd, "----------------------\n", 23);

    for (int i = 0; i < MAX_LIST_SIZE; i++){
        if (pkg_list[i].name != NULL){
            write(out_fd, "\n- Package Name : ", 17);
            write(out_fd, pkg_list[i].name, strlen(pkg_list[i].name));
            write(out_fd, "\n-     Install Date : ", 22);
            write(out_fd, pkg_list[i].install_date, strlen(pkg_list[i].install_date));
        } else {
            break;
        }
    }


    /* free the allocated line buffer */
    free(line_buff);
    line_buff = NULL;

    /* close the file now that we are done with it */
    fclose(fp);
    close(out_fd);

    printf("Report is generated at: [%s]\n", report);
}

/* fill the struct pgk with the information of the line */
struct pkg findpkg(char *origin, char *substr, int flag){

    int n;
    int o;
    int tempn;
    char *ins_date = malloc(sizeof(char) * 17);
    char c;
    int i = 0;
    char tmp[100];
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
            while ((c = origin[(tempn + 1 ) + i]) != ' '){
                tmp[i] = c;
                i++;
            }
            char *name = malloc(sizeof(char) * i);
            struct pkg result = { name, ins_date };

            // save the package name
            for (int j = 0; j < i; ++j) {
                name[j] = tmp[j];
            }

            // save the install date
            for (int i = 0; i < 18; i++) {
                ins_date[i] = origin[i];
            }

            return result;
        }
    }

}

/* check if a given string is in a line */
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