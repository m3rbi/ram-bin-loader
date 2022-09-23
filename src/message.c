#include <stdlib.h>
#include <unistd.h>
#include "message.h"


/*************************************************************
 * Generates reads a new message_t from an incoming data source.
 * 
 * @param fd -	fd from which to read the message.
 * 
 * @returns	pointer to a newly allocated message_t.
*************************************************************/
message_t *read_message(int fd) {
	message_t *msg = NULL;
	msg = malloc(sizeof(message_t));
	if (0 >= msg) {
		goto end;
	}

	if (0 >= read(fd, &(msg->message_type), sizeof(msg->message_type))) {
		goto free_msg;
	}

	if (0 >= read(fd, &(msg->content_length), sizeof(msg->content_length))) {
		goto free_msg;
	}

	msg->content = malloc(msg->content_length);
	if (0 >= msg->content) {
		goto free_msg;
	}

	if (msg->content_length != read(fd, msg->content, msg->content_length)) {
		goto free_content;
	}
	return msg;

free_content:
	free(msg->content);
free_msg:
	free(msg);
end:
	return NULL;
}


/*************************************************************
 * Destroys an existing allocated message.
 * 
 * @param msg -	pointer to a message to be destroyed.
*************************************************************/
void close_message(message_t *msg) {
	free(msg->content);
	free(msg);
}
