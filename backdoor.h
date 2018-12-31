#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#include "config.h"
#include "handlers.h"
#include "utils.h"

#define TRUE 1
#define FALSE 0

int connection_initialize();
int send_data(char *data, unsigned int len);
void register_command(const char *command, command_callback_t handler);
void init_commands();
void crypt(char* data, char* key, unsigned int data_size, unsigned int key_size);