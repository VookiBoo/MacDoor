#include "utils.h"

void execute_command(const char* command, char* result) {
	char cmd[5000];
	
	strcpy(cmd, command);
	strcat(cmd, " 2>&1");

	FILE *fp = popen(cmd, "r");

	char line[1024];

	while (fgets(line, sizeof(line)-1, fp) != NULL) {
    	strcat(result, line);
  	}

  	pclose(fp);
}