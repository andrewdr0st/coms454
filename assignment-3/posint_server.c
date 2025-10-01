#include "posint.h"
#include <stdlib.h>

#define INIT_LIST_LEN 10

typedef struct {
	int* arr;
	int max_len;
	int len;
} array_list;

array_list list = {NULL, 0, 0};

int *
insert_1_svc(insert_args *argp, struct svc_req *rqstp)
{
	static int  result;

	/*
	 * insert server code here
	 */

	return &result;
}

int *
retrieve_1_svc(int *argp, struct svc_req *rqstp)
{
	static int  result;

	/*
	 * insert server code here
	 */

	return &result;
}

int *
delete_1_svc(int *argp, struct svc_req *rqstp)
{
	static int  result;

	/*
	 * insert server code here
	 */

	return &result;
}
