#ifndef MY_DUMMY
#define MY_DUMMY


void my_cuda_init(int id);
void my_cuda_destroy(int id);

void copy_train_test_cuda(int num_train, int num_test, int num_attr, float *train,
    float *test);

void free_test_train_cuda();

void calculate_manhattan_cuda(int id, float value1, int num_values2, int idx_value2, float *value2, 
int num_attr,float min, float max, float *dissim);

void calculate_manhattan_cuda2(int id, float idx_value1, int idx_value2,
    float min, float max, float *dissim);

#endif