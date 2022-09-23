#define _GNU_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "channel.h"

#define PIPE_MESSAGE ("abcdEFGH")
#define PIPE_MESSAGE_LEN (sizeof(PIPE_MESSAGE) - 1)


/*************************************************************
 * Allocates and initializes an empty channel.
 * 
 * @returns -	pointer to the new channel, NULL on failure.
*************************************************************/
channel_t* create_channel() {
	channel_t* channel = malloc(sizeof(channel_t));
	if (channel <= 0) {
		goto end;
	}

	int pipefd[2];

	if( pthread_mutex_init(&channel->lock, NULL) != 0) {
		goto free_channel;
	}

	if (pipe2(pipefd, O_CLOEXEC) != 0) {
		goto destroy_mutex;
	}

	channel->read_fd = pipefd[0];
	channel->write_fd = pipefd[1];

	return channel;

destroy_mutex:
	pthread_mutex_destroy(&channel->lock);
free_channel:
	free(channel);
end:
	return NULL;
}


/*************************************************************
 * Closes and frees all alocated memory needed for the channel.
 * 
 * @param channel -	pointer to the channel we want to close.
*************************************************************/
void close_channel(channel_t *channel) {
	pthread_mutex_lock(&channel->lock);

	close(channel->read_fd);
	close(channel->write_fd);
	pthread_mutex_destroy(&channel->lock);
	free(channel);
}


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
int channel_read(channel_t *channel, char *buff, const int buff_len) {
	pthread_mutex_lock(&channel->lock);
	
	int retval = read(channel->read_fd, buff, buff_len);
	pthread_mutex_unlock(&channel->lock);
	
	return retval;
}


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
int channel_write(channel_t *channel, char *buff, const int buff_len) {
	pthread_mutex_lock(&channel->lock);

	int retval = write(channel->write_fd, buff, buff_len);
	pthread_mutex_unlock(&channel->lock);
	
	return retval;
}


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
int channel_sync_send(channel_t *channel) {
	int retval = channel_write(channel, PIPE_MESSAGE, PIPE_MESSAGE_LEN);
	if (retval != PIPE_MESSAGE_LEN) {
		return -1;
	}

	return 0;
}


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
int channel_sync_recv(channel_t *channel) {
	char buff[PIPE_MESSAGE_LEN];
	
	memset(buff, 0, PIPE_MESSAGE_LEN);

	channel_read(channel, buff, PIPE_MESSAGE_LEN);
	if (strcmp(buff, PIPE_MESSAGE) != 0) {
		return *((int*)buff);
	}

	return 0;
}


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
int channel_sync_send_error(channel_t *channel, int error){
	int retval = channel_write(channel, (char*)&error, sizeof(int));
	if (retval != sizeof(int)) {
		return -1;
	}

	return 0;
}
