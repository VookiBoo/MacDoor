typedef void (*command_callback_t)(char*, char*);

typedef struct {
	command_callback_t callback;
	char* command;
} command_t;