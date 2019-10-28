#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>

#define MAX_INFO_L 50
#define MAX_PROCESSES 1000000

// References: https://stackoverflow.com/questions/34575285/read-proc-stat-information
//             https://linux.die.net/man/5/proc
//             https://stackoverflow.com/questions/29105448/get-process-info-from-proc

struct process {
    char name[MAX_INFO_L],
            state[MAX_INFO_L],
            pid[MAX_INFO_L],
            ppid[MAX_INFO_L],
            memory[MAX_INFO_L],
            threads[MAX_INFO_L];
            int files;
};

char *header = "|%-6s | %-5s | %-36s | %-10s | %-10s | %-4s | %-11s|\n",
     *headDiv =	"|-------|--------|--------------------------------------|------------|------------|-----------|------------|\n",
     *tableFormat = "| %-5s | %-6s | %-36s | %-10s | %-9.1fM | %-9s | %-11d|\n";

static const struct process emptyProcess;
struct process processes[MAX_PROCESSES];

void reinit();
int readProcesses();
int getProcesses();

void reinit() {
    for(int i = 0; processes[i].name[0] != '\0'; i++) {
        processes[i] = emptyProcess;
    }
    printf("mytop: cleanup done!\n");
}

static void saveFile(int signo) {

    FILE *fp;
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char f_name[30];
	float memory;

	sprintf(f_name, "mytop-status_%d_%d_%d.txt", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	fp = fopen(f_name, "w");
	fprintf(fp, header, " PID", "Parent", "Name", "State", "Memory", "# Threads", "Open Files");
	fprintf(fp, headDiv);

	for (int i = 0; processes[i].name[0] != '\0'; i++) {
		memory = atof(processes[i].memory) / 1000;
		fprintf(fp, tableFormat, processes[i].pid, processes[i].ppid, processes[i].name, processes[i].state, memory, processes[i].threads, processes[i].files);
	}

	printf("\n File saved in: %s\n", f_name);
	fclose(fp);
	sleep(3); // a dormir después de guardar, qué no
}

int main(){
	// Place your magic here
	if (signal(SIGINT, saveFile) == SIG_ERR) {
		printf("mytop: cannot capture the signal\n");
	}

	while(1) {
		readProcesses();
		getProcesses();
		sleep(5);
		reinit();
	}
	return 0;
}

int readProcesses() {

	DIR *d = opendir("/proc/");
	struct dirent *dir;
	FILE *fp;

	char path[30],
		 fdpath[30],
		 info[MAX_INFO_L],
		 c;

	int currentProc = 0,
		files;

	memset(info, 0, MAX_INFO_L);
	strcpy(path, "/proc/");
	strcpy(fdpath, "/proc/");

	while((dir = readdir(d)) != NULL) {
	    // revisa si dir es un path que contiene sólo dígitos
		if(isdigit(*dir->d_name) != 0) {

            files = 0;
			strcat(path, dir->d_name);
			strcat(path, "/stat");

			strcat(fdpath, dir->d_name);
			strcat(fdpath, "/fd");

			fp = fopen(path, "r");
            int pid;
			char name[MAX_INFO_L];
			char temp_name [MAX_INFO_L];
			char state;
			int index;
			int ppid;
			long memory;
			long threads;

			// obteniendo sólo los datos que nos interesan
			fscanf(fp, "%d %s %c %d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %*lu %*lu %*ld %*ld %*ld %*ld %*ld %ld %*llu %*lu %*ld %ld", &pid, name, &state, &ppid, &threads, &memory);

			// remueve los paréntesis obtenidos en name
			int j = 0;
			for (int i = 0; i < MAX_INFO_L; i++){
			    if (name[i] == '(' || name[i] == ')') {
			    } else {
                    temp_name[j] = name[i];
                    j++;
			    }
			}
			strcpy(processes[currentProc].name, temp_name);
            snprintf(processes[currentProc].ppid, MAX_INFO_L, "%d", ppid);
            snprintf(processes[currentProc].pid, MAX_INFO_L, "%d", pid);
            snprintf(processes[currentProc].memory, MAX_INFO_L, "%lu", memory);
            snprintf(processes[currentProc].threads, MAX_INFO_L, "%lu", threads);
			processes[currentProc].state[0] = state;

			fclose(fp);

			// open files
			DIR *fdd = opendir(fdpath);
			struct dirent *fd_dir;
			while((fd_dir = readdir(fdd)) != NULL) {
				files++;
			}

			closedir(fdd);
			processes[currentProc].files = files - 2;
			strcpy(fdpath, "/proc/");
			strcpy(path, "/proc/");
			currentProc++;
		}
	}

	closedir(d);
	return 0;
}

int getProcesses() {

    float memory;
	int index = 0;

	printf(header, " PID", "Parent", "Name", "State", "Memory", "# Threads", "Open Files");
	printf(headDiv);

	while(processes[index].name[0] != '\0') {
		memory = atof(processes[index].memory) / 1000;
		printf(tableFormat, processes[index].pid, processes[index].ppid, processes[index].name, processes[index].state, memory, processes[index].threads, processes[index].files);
        index++;
	}

	return 0;
}
