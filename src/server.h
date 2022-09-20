#ifndef BIN_LOADER_SERVER_H
#define BIN_LOADER_SERVER_H


struct server_t;


typedef void (*handler_func_t)(struct server_t);


typedef struct server_t {
	int listen_fd;
	handler_func_t handler;
} server_t;


typedef struct server_addr_t {
	char *ip;
	int port;
} server_addr_t;


/*************************************************************
 * Creates a new server_t struct, based on specified local address
 * and with specified connection handler function.
 * 
 * @param addr -	desired server address.
 * @param handler -	connection handler function.
 * 
 * @returns -	pointer to newly allocated server_t.
*************************************************************/
server_t *create_server(server_addr_t addr, handler_func_t handler);


/*************************************************************
 * Start serving forever in a loop.
 * 
 * @param server -	server_t to start serving.
*************************************************************/
void serve_forever(server_t *server);


/*************************************************************
 * Closes and frees all the server_t memory.
 * 
 * @param server -	specified server_t.
*************************************************************/
void close_server(server_t *server);


#endif
