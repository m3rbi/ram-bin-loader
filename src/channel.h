#ifndef BIN_LOADER_CHANNEL_H
#define BIN_LOADER_CHANNEL_H

#include <pthread.h>


typedef struct channel_t {
	int read_fd;
	int write_fd;
	pthread_mutex_t lock;
} channel_t;


/*************************************************************
 * Allocates and initializes an empty channel.
 * 
 * @returns -	pointer to the new channel, NULL on failure.
*************************************************************/
channel_t *create_channel();


/*************************************************************
 * Closes and frees all alocated memory needed for the channel.
 * 
 * @param channel -	pointer to the channel we want to close.
*************************************************************/
void close_channel(channel_t *channel);


/*************************************************************
 * Reads data from given channel into a buffer.
 * 
 * @param channel -		pointer to the channel we want to read from.
 * @param buff -		buffer to which we read the data to.
 * @param buff_len -	size of buff, amount of bytes to read.
 * 
 * @returns -	on sucess, returns the amount of bytes read.
 * 				on failure, returns error-number according
 * 				to the read() function error-numbers.
*************************************************************/
int channel_read(channel_t *channel, char *buff, const int buff_len);


/*************************************************************
 * Writes data from given buffer into a channel
 * 
 * @param channel -		pointer to the channel we want to write to.
 * @param buff -		the buffer we want to write.
 * @param buff_len -	size of buff, amount of bytes to write.
 * 
 * @returns -	on sucess, returns the amount of bytes written.
 * 				on failure, returns error-number according
 * 				to the write() function error-numbers.
*************************************************************/
int channel_write(channel_t *channel, char *buff, const int buff_len);


/*************************************************************
 * Sends a magic buffer through the channel. meant for 
 * synchronization between processes. This function "signals"
 * to the other channel.
 * This function doesn't block.
 * 
 * @param channel -		pointer to the channel we want to signal.
 * 
 * @returns -	on sucess, returns 0. -1 otherwise.
*************************************************************/
int channel_sync_send(channel_t *channel);


/*************************************************************
 * Expects to receive a magic buffer through the channel,
 * or an error code. meant for synchronization between processes.
 * This function blocks until it receives a message, whether its
 * an error, or the magic buffer.
 * 
 * @param channel -		pointer to the channel we want to wait
 * 						for a signal from.
 * 
 * @returns -	on receiving a signal, returns 0. returns the 
 * 				error code otherwise.
*************************************************************/
int channel_sync_recv(channel_t *channel);


/*************************************************************
 * Sends the specified error code to a channel that expects
 * synchronization.
 * This function doesn't block.
 * 
 * @param channel -		pointer to the channel we want to signal.
 * @param error   -		error code we want to send.
 * 
 * @returns -	on sucess, returns 0. -1 otherwise.
*************************************************************/
int channel_sync_send_error(channel_t *channel, int error);


#endif
