#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timing.h"
#include "definitions.h"
#include "metrics.h"
#include "riona.h"

#include <sys/stat.h>

#define MAX_PATH_LEN 255
#define MAX_NAME_LEN 100
#define MAX_SEP_LEN 20

typedef struct file_info {
    char file_name[MAX_PATH_LEN];
    char sep[MAX_SEP_LEN];
    int class_idx;
} file_info;

typedef struct stats_griona {
    char *file_name;
    int num_numerical_attr;
    int num_nominal_attr;
    int num_attr;
    int num_inst;
    int num_classes;
    int num_cv;
    bool is_norm;
    int k_fold;
    metrics (*pred_metrics)[];
    metrics *avg_metrics;
    metrics final_metrics;
    double time_read_in_file;
    double time_save_out_file;
    timing_predict tp_avg;
} stats_griona;

typedef struct stats_optk {
    char *file_name;
    int num_numerical_attr;
    int num_nominal_attr;
    int num_attr;
    int num_inst;
    int num_classes;
    int *unique_classes;
    bool is_norm;
    int k_max;
    int k_opt;
    metrics *pred_metrics;
    metrics *avg_metrics;
    double time_total;
    double time_read_in_file;
    double time_save_out_file;
    timing_optk toptk;
} stats_optk;

bool get_file_path(char *data_folder_path, char *file_name, char *file_path);

bool get_info_file_path(char *data_folder_path, char *file_name, char *info_path);

bool get_out_folder_path(char *out_path, char *file_name, char *out_folder_path);

bool load_file(file_info *fi, char *file_path, int *classes, int num_attr, 
    float values[][num_attr]);

bool load_info(char *info_path, int num_attr, char *attr_types, char attr_names[][MAX_NAME_LEN], 
    char *target_name, int *class_idx);

bool get_out_file_path_main(char *out_folder_path, char *out_path);

bool get_out_file_path_griona(char *out_folder_path, char *file_name, int num_attr, int num_inst, 
    int k_neigh, bool is_norm, char *out_path);

bool get_stat_file_path_griona(char *out_folder_path, char *file_name, int num_attr, int num_inst, 
    int k_neigh, bool is_norm, bool is_kopt, char *out_path);

bool get_out_file_path_optk(char *out_folder_path, char *file_name, int num_attr, int num_inst, 
    int k_max, bool is_norm, char *out_path);

bool get_stat_file_path_optk(char *out_folder_path, char *file_name, int num_attr, int num_inst, 
    int k_max, bool is_norm, char *out_path);

void make_directory(const char* name);

void create_out_folder_if_not_exist(char *out_folder_path);

bool save_out_file_griona(char *out_path, int num_inst, int num_attr, float values[][num_attr],  
    char *attr_types, int *classes, int num_k_fold, stats_predict sp[][num_inst], 
    int classes_pred[][num_inst]);

bool save_stat_file_griona(char *stat_path, stats_griona *st);

bool save_out_file_optk(char *out_path, int num_inst, int num_attr, float values[][num_attr],  
    char *attr_types, int *classes, int k_max, int classes_pred[][k_max],
    stats_find_optk *sfoptk);

bool save_stat_file_optk(char *stat_path, stats_optk *st);

bool get_num_inst_and_attributes(char *file_path, char *sep, int *num_inst, int *num_attr);
