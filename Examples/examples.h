void fillrand(char *cp, int s);

#define talloc(type, num) (type *) malloc(sizeof(type)*(num))
void print_data_and_coding_1(int k, int m, int w, int size, 
		char **data, char **coding);
void print_data_and_coding_2(int k, int m, int w, int psize, 
		char **data, char **coding);
