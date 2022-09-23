#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "handler.h"
#include "message.h"
#include "run_detached.h"
#include "consts.h"


static void handle_exec_bin(message_t *msg);
static void handle_close_server(message_t *_);


static message_handler_t message_handlers[MESSAGE_TYPE_COUNT] = {
	[EXEC_BIN] = handle_exec_bin,
	[CLOSE_SERVER] = handle_close_server
};


/*************************************************************
 * An handler_func_t implementation. Reads a message from the 
 * given server, and executes the given binary.
 * 
 * @param server -  server to read from.
*************************************************************/
void handle_connection(server_t server) {
	message_t *msg = read_message(server.listen_fd);
	if (0 >= msg) {
		printf("read_msg\n");
		return;
	}

	if (0 > msg->message_type || MESSAGE_TYPE_COUNT <= msg->message_type) {
		printf("invalid message type %d\n", msg->message_type);
		goto free_msg;
	}

	message_handlers[msg->message_type](msg);

free_msg:
	close_message(msg);
	return;
}


/*************************************************************
 * Executes binary inside message as a detached process using
 * configured process name.
 * 
 * @param server -  server to read from.
*************************************************************/
static void handle_exec_bin(message_t *msg) {
	int fd = memfd_create("", FD_CLOEXEC);
	if (0 >= fd) {
		printf("memfd\n");
		return;
	}

	if (msg->content_length != write(fd, (char*)msg->content, msg->content_length)) {
		printf("write\n");
		goto close_fd;
	}
	
	char *p = NULL;
	if (0 >= asprintf(&p, "/proc/self/fd/%i", fd)) {
		printf("asprintf\n");
		goto close_fd;
	}

	run_status_code_t retval = run_detached(p, PROC_NAME);
	printf("run detached code: %d\n", retval);
	
	free(p);
close_fd:
	close(fd);
}


/*************************************************************
 * Closes the process.
*************************************************************/
static void handle_close_server(message_t *_) {
	kill(getppid(), GRACEFUL_EXIT_SIGNAL);
	exit(0);
}


/*************************************************************
 * Callback for the signal, that just exits.
*********************************************1****************/
static void exit_signal_handler(int _) {
	exit(0);
}


/*************************************************************
 * Initializes all params necessary for the handler to work.
*************************************************************/
void init_handler() {
	signal(GRACEFUL_EXIT_SIGNAL, exit_signal_handler);
}
