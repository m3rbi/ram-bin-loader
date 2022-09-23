#ifndef BIN_LOADER_MESSAGE_H
#define BIN_LOADER_MESSAGE_H


typedef enum message_type_t {
	EXEC_BIN = 0,
	CLOSE_SERVER = 1,
	MESSAGE_TYPE_COUNT
} message_type_t;


typedef struct message_t {
	message_type_t message_type;
	size_t content_length;
	void* content;
} message_t;


/*************************************************************
 * Generates reads a new message_t from an incoming data source.
 * 
 * @param fd -	fd from which to read the message.
 * 
 * @returns	pointer to a newly allocated message_t.
*************************************************************/
message_t* read_message(int fd);


/*************************************************************
 * Destroys an existing allocated message.
 * 
 * @param msg -	pointer to a message to be destroyed.
*************************************************************/
void close_message(message_t *msg);


#endif
