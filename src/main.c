#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

#include <omp.h>

#include <sys/time.h>
#include <time.h> 

#include "files.h"
#include "definitions.h"
#include "metrics.h"
#include "rules.h"
#include "timing.h"
#include "riona.h"
#include "classes.h"
#include "values.h"
#include "values_classes.h"
#include "attributes.h"
#include "cross_validation.h"
#include "my_cuda.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define DATA_FOLDER_PATH "./data"
#define OUT_FOLDER_PATH "./out"

enum mode_t {
    NORMAL_MODE,
    TRIANGLE_INEQ_MODE,
    TRIANGLE_INEQ_DYNAMIC_MODE
} ;

void get_k_max(int num_train, int *k_max)
{
    *k_max = (int)sqrt(num_train);
}

void translate_predictions(int num_inst, int *indices_shuffled, int *classes_pred_shuffled, 
    int *classes_pred)
{
    for (int i = 0; i < num_inst; i++)
    {
        classes_pred[indices_shuffled[i]] = classes_pred_shuffled[i];
    }
}

int get_optimal_k_neigh(int k_max, int num_inst, int num_attr, float values[][num_attr], 
    int *classes, int num_unique_classes, int *unique_classes, char *attr_types, bool is_norm,
    int classes_pred_find_optk[][k_max], int *classes_pred_optk, stats_find_optk *sfoptk, 
    timing_optk *toptk)
{

    float min_values[num_attr];
    float max_values[num_attr];
    get_min_max_attributes(num_attr, num_inst, values, min_values, 
    max_values);

    int classes_cnt[num_unique_classes];
    get_classes_cnt(num_unique_classes, unique_classes, num_inst, classes,
        classes_cnt);

    for (int i = 0; i < num_inst; i++)
    {
        sfoptk[i].num_k_plus = malloc(sizeof(*(sfoptk[i].num_k_plus)) * k_max);
        if(sfoptk[i].num_k_plus == NULL) {printf("Blad allokacji");};
        sfoptk[i].num_k_plus_promising = malloc(sizeof(*(sfoptk[i].num_k_plus_promising)) * k_max);
        if(sfoptk[i].num_k_plus_promising == NULL) {printf("Blad allokacji");};
    } 

    int k_opt;
    find_optimal_k(k_max, num_inst, attr_types, num_attr, values, classes, 
        num_unique_classes, unique_classes, classes_cnt, min_values, max_values, is_norm, 
        &k_opt, classes_pred_find_optk, sfoptk, toptk);

    for (int i = 0; i < num_inst; i++)
    {
        classes_pred_optk[i] = classes_pred_find_optk[i][k_opt-1];
    } 
    
    return k_opt;
}

void deallocate_sfoptk(int num_inst, stats_find_optk *sfoptk)
{
    for (int i = 0; i < num_inst; i++)
    {
        free(sfoptk[i].num_k_plus);
        free(sfoptk[i].num_k_plus_promising);
        free(sfoptk[i].k_plus_promising_1);
        free(sfoptk[i].k_plus_1);
        free(sfoptk[i].k_plus_promising_3);
        free(sfoptk[i].k_plus_3);
    } 
}

bool get_params(int argc, char *argv[], file_info *fi, file_info *fi_default, mode_t *mode)
{
    int cnt = 0;
    *mode = NORMAL_MODE;
    while ((argc > 1))
	{
        if((argv[1][0] == '-'))
        {
            if(argv[1][1] == 't' && argv[1][2] == 'i' && argv[1][3] == 'd')
            {
                printf("dynamic triangle inequality mode\n");
                *mode = TRIANGLE_INEQ_DYNAMIC_MODE;
            }
            else if(argv[1][1] == 't' && argv[1][2] == 'i')
            {
                printf("triangle inequality mode\n");
                *mode = TRIANGLE_INEQ_MODE;
            }
            else
            {
                printf("Wrong keyword argument: %s\n", argv[1]);
            }
        }
        else
        {
            if (cnt == 0)
            {
                strncpy(fi->file_name, argv[1], MAX_PATH_LEN - 1);
                strncpy(fi->sep, " ", MAX_SEP_LEN);
            }
            else if (cnt == 1)
            {
                strncpy(fi->sep, argv[1], MAX_SEP_LEN - 1);
            }
            cnt++;
        }

		++argv;
		--argc;
	}

    if(cnt == 0)
    {
        memcpy(fi, fi_default, sizeof(*fi));
    }

    return true;
}

FILE * ptr;
void printf_combo(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    vfprintf(ptr, fmt, args);
    va_end(args);
}

int main(int argc, char *argv[])
{
    #ifdef DEBUG
        printf("DEBUG\n");
    #endif
    //------------------------ READ FILES ----------------------

    file_info fi_default = {"simple", " "};
    // file_info fi_default = {"heart", " "};
    // file_info fi_default = {"spambase", ","};
    // file_info fi_default = {"lymphography", ","};
    file_info fi;
    mode_t mode;
    get_params(argc, argv, &fi, &fi_default, &mode); 

    char out_folder_path[MAX_PATH_LEN];
    get_out_folder_path(OUT_FOLDER_PATH, fi.file_name, out_folder_path);
    create_out_folder_if_not_exist(out_folder_path);

    char out_path_main[MAX_PATH_LEN];
    get_out_file_path_main(out_folder_path, out_path_main);

    ptr = fopen(out_path_main, "w");
    if(ptr == NULL)
    {
        printf("Unable to create main output file.\n");
        return false;
    }

    double time_total_s = get_time_sec();
    double time_read_in_file_s = get_time_sec();

    char file_path[MAX_PATH_LEN];
    get_file_path(DATA_FOLDER_PATH, fi.file_name, file_path);
    printf_combo("File path %s\n", file_path);

    int num_inst, num_attr;
    get_num_inst_and_attributes(file_path, fi.sep, &num_inst, &num_attr);
    printf_combo("Number of attributes %d\n", num_attr);
    printf_combo("Number of instances %d\n", num_inst);

    char info_path[MAX_PATH_LEN];
    get_info_file_path(DATA_FOLDER_PATH, fi.file_name, info_path);
    char attr_types[num_attr];
    char attr_names[num_attr][MAX_NAME_LEN];
    char target_name[MAX_NAME_LEN];
    if(!load_info(info_path, num_attr, attr_types, attr_names, target_name,
        &(fi.class_idx)))
    {
        return -1;
    }
    printf_combo("Decision class index: %d\n", fi.class_idx);

    int *classes = malloc( sizeof(*classes) * num_inst);
    float (*values) [num_attr] = malloc( sizeof(float[num_inst][num_attr]) );
    if(!load_file(&fi, file_path, classes, num_attr, values))
    {
        return -1;
    }

    double time_read_in_file = get_time_sec() - time_read_in_file_s;
    
    int unique_classes[MAX_CLASSES];
    int num_unique_classes;
    get_unique_classes(num_inst, classes, unique_classes, &num_unique_classes);

    int k_max;
    get_k_max(num_inst, &k_max);

    #ifdef CUDA
    #pragma omp parallel
    { 
        int id = omp_get_thread_num();
        printf_combo("thread: %d\n", id);
        my_cuda_init(id);
    }
    #endif

    for(int is_norm = 0; is_norm < 2; is_norm++) // normalization
    {
        //------------------------ FIND OPTIMAL K NEIGHBOURS ----------------------
        printf_combo("FINDING OPTIMAL K NEIGHBOURS - ");
        printf_combo("%s\n", is_norm ? "NORMALIZED" : "STANDARD");
        double time_total_optk_s = get_time_sec();

        int (*classes_pred_find_optk) [k_max] = malloc( sizeof(float[num_inst][k_max]) );
        int classes_pred_optk[num_inst];

        timing_optk toptk;
        stats_find_optk *sfoptk = malloc(sizeof(*sfoptk) * num_inst);
        int k_opt = get_optimal_k_neigh(k_max, num_inst, num_attr, values, classes, num_unique_classes, 
            unique_classes, attr_types, is_norm, classes_pred_find_optk, classes_pred_optk, sfoptk,
            &toptk);

        double time_total_optk = get_time_sec() - time_total_optk_s;

        printf_combo("\tk_max: %d, k_opt: %d\n", k_max, k_opt);

        metrics pred_metrics_optk[num_unique_classes];
        calculate_metrics_for_each_class(num_inst, num_unique_classes, unique_classes, 
            classes, classes_pred_optk, pred_metrics_optk);

        metrics avg_metrics;
        calculate_average_metrics(num_unique_classes, pred_metrics_optk, &avg_metrics);

        int num_numerical_attr, num_nominal_attr;
        get_num_numerical_and_nominal_attr(attr_types, num_attr, &num_numerical_attr,
            &num_nominal_attr);
        //--------------------------- SAVE FIND OPTIMAL K NEIGHBOURS RESULTS -------------------------

        char out_path_optk[MAX_PATH_LEN];
        
        get_out_file_path_optk(out_folder_path, fi.file_name, num_attr, num_inst, k_max, is_norm, 
            out_path_optk);

        create_out_folder_if_not_exist(OUT_FOLDER_PATH);

        double time_save_out_file_optk_s = get_time_sec();
        save_out_file_optk(out_path_optk, num_inst, num_attr, values, attr_types, classes, k_max, 
            classes_pred_find_optk, sfoptk);
        double time_save_out_file_optk = get_time_sec() - time_save_out_file_optk_s;
        deallocate_sfoptk(num_inst, sfoptk);
        free(classes_pred_find_optk);
        char stat_path_optk[MAX_PATH_LEN];
        get_stat_file_path_optk(out_folder_path, fi.file_name, num_attr, num_inst, k_max, is_norm, 
            stat_path_optk);

        stats_optk stoptk = { .file_name = fi.file_name,
                            .num_numerical_attr = num_numerical_attr,
                            .num_nominal_attr = num_nominal_attr,
                            .num_attr = num_attr,
                            .num_inst = num_inst,
                            .num_classes = num_unique_classes,
                            .unique_classes = unique_classes,
                            .is_norm = is_norm,
                            .k_max = k_max,
                            .k_opt = k_opt,
                            .pred_metrics = pred_metrics_optk,
                            .avg_metrics = &avg_metrics,
                            .time_total = time_total_optk,
                            .time_read_in_file = time_read_in_file,
                            .time_save_out_file = time_save_out_file_optk,
                            .toptk = toptk
                        };

        save_stat_file_optk(stat_path_optk, &stoptk);

        // ------------------------------ GRIONA - PREDICT ----------------------------------
        printf_combo("PREDICTION - ");
        printf_combo("%s\n", is_norm ? "NORMALIZED" : "STANDARD");
        (void)k_opt;
        int k_neigh[] = {1, 3, k_opt, k_max};

        for(int k = 0; k < ARRAY_SIZE(k_neigh); k++) // k neighbours
        {
            printf_combo("\tk+ neighbours:%3d - ", k_neigh[k]);
            metrics pred_metrics[NUM_CV][NUM_K_FOLD];
            metrics avg_metrics[NUM_CV];
            metrics final_metrics;

            int (*classes_pred) [num_inst] = malloc( sizeof(int[NUM_CV][num_inst]) );
            stats_predict (*sp) [num_inst] = malloc( sizeof(stats_predict[NUM_CV][num_inst]) );

            timing_predict tp[NUM_CV];

            for(int cv = 0; cv < NUM_CV; cv++)  // cross validation
            {
                int max_k_fold_inst = (num_inst / NUM_K_FOLD) + 1;
                int k_fold_indices[NUM_K_FOLD][max_k_fold_inst];
                int k_fold_cnt[NUM_K_FOLD];
                create_k_fold(num_inst, max_k_fold_inst, NUM_K_FOLD, k_fold_indices, k_fold_cnt);

                for(int k_fold = 0; k_fold < NUM_K_FOLD; k_fold++) // k-fold
                {
                    int num_train, num_test;
                    get_num_train_test(k_fold, num_inst, k_fold_cnt, &num_train, &num_test);

                    float (*train_values) [num_attr] = malloc( sizeof(float[num_train][num_attr]) );
                    int train_classes[num_train];
                    float (*test_values) [num_attr] = malloc( sizeof(float[num_test][num_attr]) );
                    int test_classes[num_test];
                    split_train_test(num_inst, num_attr, values, classes, NUM_K_FOLD, k_fold, 
                        k_fold_indices, k_fold_cnt, train_values, train_classes, test_values, 
                        test_classes);

                    int test_classes_pred[num_test];
                    stats_predict test_sp[num_test];

                    if(mode == NORMAL_MODE)
                    {
                        predict(num_train, num_test, num_attr, k_neigh[k], train_values, train_classes,
                            test_values, attr_types, num_unique_classes, unique_classes,  
                            test_classes_pred, is_norm, test_sp, &tp[cv]);
                    } 
                    else if(mode == TRIANGLE_INEQ_MODE)
                    {
                        predict_triangle_ineq(num_train, num_test, num_attr, k_neigh[k], train_values, train_classes,
                        test_values, attr_types, num_unique_classes, unique_classes,  
                        test_classes_pred, is_norm, test_sp, &tp[cv]);
                    }
                    else if(mode == TRIANGLE_INEQ_DYNAMIC_MODE)
                    {
                        predict_triangle_ineq_with_updating_eps_neigh(num_train, num_test, num_attr, k_neigh[k], train_values, train_classes,
                        test_values, attr_types, num_unique_classes, unique_classes,  
                        test_classes_pred, is_norm, test_sp, &tp[cv]);
                    }

                    free(train_values);
                    free(test_values);

                    int *test_indices = k_fold_indices[k_fold];
                    prepare_k_fold_results(num_inst, num_test, test_indices, test_sp, sp[cv], 
                        test_classes_pred, classes_pred[cv]);

                    int unique_classes_test[MAX_CLASSES];
                    int num_unique_classes_test;
                    get_unique_classes(num_test, test_classes, unique_classes_test, 
                        &num_unique_classes_test);

                    calculate_metrics(num_test, num_unique_classes_test, unique_classes_test, 
                        test_classes, test_classes_pred, &pred_metrics[cv][k_fold]);
                }   // k-fold
                calculate_average_metrics(NUM_K_FOLD, pred_metrics[cv], &avg_metrics[cv]);
            }   // cross validation

            calculate_average_metrics(NUM_CV, avg_metrics, &final_metrics);
            
            timing_predict tp_avg;
            calculate_average_times(NUM_CV, tp, &tp_avg);

            printf_combo("accuracy:%.2f +- %.2f balanced accuracy:%.2f +- %.2f\n", final_metrics.acc, final_metrics.std_acc, 
                final_metrics.balanced_acc, final_metrics.std_balanced_acc);

            //--------------------------------- SAVE GRIONA RESULTS --------------------------------
            char out_path[MAX_PATH_LEN];
            get_out_file_path_griona(out_folder_path, fi.file_name, num_attr, num_inst, k_neigh[k],
                is_norm, out_path);

            create_out_folder_if_not_exist(out_folder_path);

            double time_save_out_file_s = get_time_sec();
            save_out_file_griona(out_path, num_inst, num_attr, values, attr_types, classes, 
                NUM_K_FOLD, sp, classes_pred);
            double time_save_out_file = get_time_sec() - time_save_out_file_s;

            free(classes_pred);
            free(sp);

            char stat_path[MAX_PATH_LEN];
            bool is_kopt = k_neigh[k] == k_opt;
            get_stat_file_path_griona(out_folder_path, fi.file_name, num_attr, num_inst, k_neigh[k],
                is_norm, is_kopt, stat_path);

            stats_griona stgr = { .file_name = fi.file_name,
                                    .num_numerical_attr = num_numerical_attr,
                                    .num_nominal_attr = num_nominal_attr,
                                    .num_attr = num_attr,
                                    .num_inst = num_inst,
                                    .num_classes = num_unique_classes,
                                    .num_cv = NUM_CV,
                                    .is_norm = is_norm,
                                    .k_fold = NUM_K_FOLD,
                                    .pred_metrics = pred_metrics,
                                    .avg_metrics = avg_metrics,
                                    .final_metrics = final_metrics,
                                    .time_read_in_file = time_read_in_file,
                                    .time_save_out_file = time_save_out_file,
                                    .tp_avg = tp_avg
                                };
            save_stat_file_griona(stat_path, &stgr);
        }   // k neighbours
    }   // normalization

    free(values);

    #ifdef CUDA
    #pragma omp parallel
    { 
        int id = omp_get_thread_num();
        printf_combo("thread: %d\n", id);
        my_cuda_destroy(id);
    }
    #endif

    double time_total = get_time_sec() - time_total_s;
    printf_combo("Total time: %f sec", time_total);

    fclose(ptr);
    return 0;
}