#include "timing.h"

double get_time_sec(void)
{
    struct timeval s;
    gettimeofday(&s, NULL);
    return s.tv_sec + 1e-6 * s.tv_usec;
}

void calculate_average_times(int num_cv, timing_predict tp[num_cv], timing_predict *tp_avg)
{
    memset(tp_avg, 0, sizeof(*tp_avg));
    for (int i = 0; i < num_cv; i++)
    {
        tp_avg->time_svdm += tp[i].time_svdm; 
        tp_avg->time_det_k_plus += tp[i].time_det_k_plus; 
        tp_avg->time_sort_k_plus += tp[i].time_sort_k_plus;
        tp_avg->time_verif_rule += tp[i].time_verif_rule;
    }
    tp_avg->time_svdm /= num_cv; 
    tp_avg->time_det_k_plus /= num_cv; 
    tp_avg->time_sort_k_plus /= num_cv;
    tp_avg->time_verif_rule /= num_cv;
}