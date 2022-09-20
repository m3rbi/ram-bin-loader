#define _GNU_SOURCE
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <strings.h>
#include "server.h"
#include "consts.h"


/*************************************************************
 * Creates a new server_t struct, based on specified local address
 * and with specified connection handler function.
 * 
 * @param addr -	desired server address.
 * @param handler -	connection handler function.
 * 
 * @returns -	pointer to newly allocated server_t.
*************************************************************/
server_t *create_server(server_addr_t addr, handler_func_t handler) {
	int fd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
	if (0 >= fd) {
		goto end;
	}

	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(addr.ip);
	address.sin_port = htons(addr.port);

	if (0 != bind(fd, (struct sockaddr*)&address, sizeof(address))) {
		goto close_sock;
	}

	if (0 > listen(fd, LISTEN_QUEUE)) {
		goto close_sock;
	}

	server_t *server = malloc(sizeof(server_t));
	if (0 >= server) {
		goto close_sock;
	}

	server->listen_fd = fd;
	server->handler = handler;

	return server;

close_sock:
	close(fd);
end:
	return NULL;
}


/*************************************************************
 * Generates a new server_t struct for child socket, designated
 * for a new connection.
 * 
 * @param server -	father server.
 * @param fd -		fd of the new socket for this connection.
 * 
 * @returns	a new server_t (not pointer, but copy), needed to
 * 			handle the new connection.
*************************************************************/
static server_t spawn_child_socket(server_t *server, int fd) {
	server_t child = {
		.listen_fd = fd,
		.handler = server->handler
	};

	return child;
}


/*************************************************************
 * Start serving forever in a loop.
 * 
 * @param server -	server_t to start serving.
*************************************************************/
void serve_forever(server_t *server) {
	int fd;

	struct sockaddr_in client_addr;
	int len = sizeof(client_addr);
	bzero(&client_addr, len);

	struct timeval timeout;
	bzero(&timeout, sizeof(timeout));
	timeout.tv_sec = 0;
	timeout.tv_usec = USEC_TIMEOUT;


	while (1) {
		fd = accept4(server->listen_fd, (struct sockaddr*)&client_addr, &len, SOCK_CLOEXEC);
		if (0 >= fd) {
			goto end;
		}
		if (0 > setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout))) {
			goto close_fd;
		}
	
		if (0 > setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout))) {
			goto close_fd;
		}

		int child_pid = fork();
		if (0 > child_pid) {
			goto close_fd;
		}
	
		if (0 == child_pid) {
			server->handler(spawn_child_socket(server, fd));
			close(fd);
			exit(0);
		}

close_fd:
		close(fd);
	}

end:
	return;
}


/*************************************************************
 * Closes and frees all the server_t memory.
 * 
 * @param server -	specified server_t.
*************************************************************/
void close_server(server_t *server) {
	close(server->listen_fd);
	free(server);
}
