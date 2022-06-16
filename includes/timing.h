#pragma once

#include <sys/time.h>
#include <time.h> 
#include <string.h>

typedef struct timing_predict {
    double time_svdm;
    double time_det_k_plus; 
    double time_sort_k_plus;
    double time_verif_rule;
} timing_predict;

typedef struct timing_optk {
    double time_svdm;
    double time_det_k_plus; 
    double time_sort_k_plus;
    double time_verif_rule;
} timing_optk;

double get_time_sec(void);

void calculate_average_times(int num_cv, timing_predict tp[num_cv], timing_predict *tp_avg);