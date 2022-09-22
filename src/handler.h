#ifndef BIN_LOADER_HANDLER_H
#define BIN_LOADER_HANDLER_H

#include "server.h"
#include "message.h"


typedef void (*message_handler_t)(message_t *message);


/*************************************************************
 * Handles a connection, according to the message received.
 * NEEDS TO BE INITIALIZED!
 * 
 * @param conn -  server to read from.
*************************************************************/
void handle_connection(server_t conn);



/*************************************************************
 * Initializes all params necessary for the handler to work.
*************************************************************/
void init_handler();


#endif
