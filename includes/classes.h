#pragma once

void get_unique_classes(int num_train, int *train_classes, int *unique_classes, 
    int *num_unique_classes);
int get_unique_class_idx(int num_unique_classes, int *unique_classes, int target);
void get_classes_cnt(int num_unique_classes, int *unique_classes, int num_train, 
    int *train_classes, int *train_classes_cnt);
void get_most_freq_class_idx(int *train_classes_cnt, int num_unique_classes, 
    int *most_freq_class_idx_train);





