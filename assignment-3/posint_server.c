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
	int new_element = argp->new_element;
	int index = argp->index;

	if (list.len >= list.max_len) {
		if (list.max_len == 0) {
			//if list is not initialized, init
			list.max_len = INIT_LIST_LEN;
			list.arr = malloc(sizeof(int) * list.max_len);
		} else {
			list.max_len *= 2;
			list.arr = realloc(list.arr, sizeof(int) * list.max_len);
		}
	}

	if (index >= list.len) {
		list.arr[list.len] = new_element;
	} else {
		//reposition array elements
		for (int i = list.len; i > index; i--) {
			list.arr[i] = list.arr[i - 1];
		}
		list.arr[index] = new_element;
	}

	list.len++;

	return &result;
}

int *
retrieve_1_svc(int *argp, struct svc_req *rqstp)
{
	static int  result;
	int index = *argp;

	result = index >= list.len ? -1 : list.arr[index];

	return &result;
}

int *
delete_1_svc(int *argp, struct svc_req *rqstp)
{
	static int  result;
	int index = *argp;

	if (index < list.len) {
		//reposition array elements
		for (int i = index; i < list.len - 1; i++) {
			list.arr[i] = list.arr[i + 1];
		}
		list.len--;
	}

	return &result;
}
