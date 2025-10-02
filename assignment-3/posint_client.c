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
	if (argc < 4) {
		printf ("usage: %s server_host request_name param(s)\n", argv[0]);
		exit (1);
	}

	//connect client to sertver
	CLIENT* clnt = clnt_create(argv[1], POS_INT_PROG, POS_INT_VERS, "udp");

	//check if request is valid, then perform it
	if (strcmp(argv[2], "insert") == 0) {
		if (argc < 5) {
			printf("insert requires 2 params\n");
			exit(1);
		}
		insert_args args;
		args.new_element = atoi(argv[3]);
		if (args.new_element <= 0) {
			printf("new_element must be positive\n");
			exit(1);
		}
		args.index = atoi(argv[4]);
		if (args.index < 0) {
			printf("index must be >= 0\n");
			exit(1);
		}
		insert_1(&args, clnt);
	} else if (strcmp(argv[2], "retrieve") == 0) {
		int index = atoi(argv[3]);
		if (index < 0) {
			printf("index must be >= 0\n");
			exit(1);
		}
		int* result = retrieve_1(&index, clnt);
		if (*result == -1) {
			printf("no item at index %d\n", index);
		} else {
			printf("retrieved %d\n", *result);
		}
	} else if (strcmp(argv[2], "delete") == 0) {
		int index = atoi(argv[3]);
		if (index < 0) {
			printf("index must be >= 0\n");
			exit(1);
		}
		delete_1(&index, clnt);
	} else {
		printf("invalid request\naccepted requests are:\ninsert new_element index\nretrieve index\ndelete index\n");
		exit(1);
	}
	exit (0);
}
