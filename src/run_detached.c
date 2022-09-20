#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "run_detached.h"
#include "channel.h"


/*************************************************************
 * Executes specified binary, as a detached process, having
 * specified cmdline.
 * 
 * @param path -	path to desired binary.
 * @param cmdline -	desired cmdline.
 * 
 * @returns -	on success, returns SUCCESS, otherwise 
 * 				returns a negative status code from 
 * 				run_status_code_t.
*************************************************************/
run_status_code_t run_detached(char *path, char *cmdline) {
	run_status_code_t retval = SUCCESS;
	int child_pid = 0;

	channel_t *channel = create_channel();
	if (NULL == channel) {
		retval = PIPE_ERROR;
		goto end;
	}

	child_pid = fork();
	if (0 > child_pid) {
		retval = FORK_ERROR;
		goto close_channel;
	}
	
	if (child_pid == 0) {
		// child
		if (setsid() < 0) {
			channel_sync_send_error(channel, SETSID_ERROR);
			exit(0);
		}
		channel_sync_send(channel);

		child_pid = fork();
		if (0 > child_pid) {
			channel_sync_send_error(channel, FORK_ERROR);
			exit(0);
		}

		if (0 != child_pid) {
			exit(0);
		}

		channel_sync_send(channel);
		execl(path, cmdline, NULL);

	} else {
		// parent
		retval = channel_sync_recv(channel);
		if (0 != retval) {
			goto close_channel;
		}

		retval = channel_sync_recv(channel);
	}

close_channel:
	close_channel(channel);
end:
	return retval;
}
