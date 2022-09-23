#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include "server.h"
#include "handler.h"
#include "consts.h"


bool validate_args(int argc, char *argv[]) {
	if (3 != argc) {
		printf("Usage: %s {ip} {port}\n", argv[0]);
		return false;
	}

	return true;
}

int main(int argc, char *argv[]) {
	if (!validate_args(argc, argv)) {
		return -1;
	}

	server_addr_t addr = {
		.ip = argv[1], 
		.port = atoi(argv[2])
	};

	init_handler();
	server_t *server = create_server(addr, handle_connection);
	if (0 >= server) {
		printf("create_server\n");
		exit(-1);
	}

	serve_forever(server);
	
	close_server(server);

	return 0;
}
