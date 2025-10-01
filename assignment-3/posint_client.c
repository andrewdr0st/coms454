#include "posint.h"
#include <stdio.h>
#include <string.h>

void
pos_int_prog_1(char *host)
{
	CLIENT *clnt;
	int  *result_1;
	insert_args  insert_1_arg;
	int  *result_2;
	int  retrieve_1_arg;
	int  *result_3;
	int  delete_1_arg;

#ifndef	DEBUG
	clnt = clnt_create (host, POS_INT_PROG, POS_INT_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

	result_1 = insert_1(&insert_1_arg, clnt);
	if (result_1 == (int *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	result_2 = retrieve_1(&retrieve_1_arg, clnt);
	if (result_2 == (int *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	result_3 = delete_1(&delete_1_arg, clnt);
	if (result_3 == (int *) NULL) {
		clnt_perror (clnt, "call failed");
	}
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int
main (int argc, char *argv[])
{
	char *host;

	if (argc < 4) {
		printf ("usage: %s server_host command_name param(s)\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	pos_int_prog_1 (host);

	if (strcmp(argv[2], "insert") {

	} else if (strcmp(argv[2], "retrieve") {

	} else if (strcmp(argv[2], "delete") {

	} else {

	}
	exit (0);
}
