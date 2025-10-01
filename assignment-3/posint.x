struct insert_args {
	int new_element;
	int index;
};

program POS_INT_PROG {
	version POS_INT_VERS {
		int INSERT(insert_args) = 1;
		int RETRIEVE(int) = 2;
		int DELETE(int) = 3;
	} = 1;
} = 0x2cc73fb9;
