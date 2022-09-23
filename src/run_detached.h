#ifndef BIN_LOADER_RUN_DETACHED_H
#define BIN_LOADER_RUN_DETACHED_H


typedef enum run_status_code_t {
	SUCCESS = 0,
	FORK_ERROR = -1,
	SETSID_ERROR = -2,
	PIPE_ERROR = -3,
} run_status_code_t;


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
run_status_code_t run_detached(char *path, char *cmdline);


#endif
