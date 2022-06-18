#include "files.h"
#include "definitions.h"

#if defined(__MINGW32__)
    #include "direct.h"
#endif

void make_directory(const char* name)
{
#if defined(__linux__)  || (defined(__CYGWIN__) && !defined(_WIN32))
    mkdir(name, 0700); 
#else
    _mkdir(name);
#endif
}

bool get_file_path(char *data_folder_path, char *file_name, char *file_path)
{
    size_t stored = snprintf(file_path, MAX_PATH_LEN, "%s/%s.data", data_folder_path, file_name);
    return (stored >= 0) && (stored < MAX_PATH_LEN);
}

bool get_info_file_path(char *data_folder_path, char *file_name, char *info_path)
{
    size_t stored = snprintf(info_path, MAX_PATH_LEN, "%s/%s.info", data_folder_path, file_name);
    return (stored >= 0) && (stored < MAX_PATH_LEN);
}

bool get_out_folder_path(char *out_path, char *file_name, char *out_folder_path)
{
    size_t stored = snprintf(out_folder_path, MAX_PATH_LEN, "%s/%s", out_path, file_name);
    return (stored >= 0) && (stored < MAX_PATH_LEN);
}

bool get_out_file_path_main(char *out_folder_path, char *out_path)
{
    size_t stored = snprintf(out_path, MAX_PATH_LEN, "%s/_OUT_MAIN.txt", out_folder_path);
    return (stored >= 0) && (stored < MAX_PATH_LEN);
}

bool get_out_file_path_griona(char *out_folder_path, char *file_name, int num_attr, int num_inst, 
    int k_neigh, bool is_norm, char *out_path)
{
    char *norm = is_norm ? "norm" : "";
    size_t stored = snprintf(out_path, MAX_PATH_LEN, "%s/OUT_G-RIONA_%s_A%d_R%d_k%d_%s.txt", 
        out_folder_path, file_name, num_attr, num_inst, k_neigh, norm);
    return (stored >= 0) && (stored < MAX_PATH_LEN);
}

bool get_stat_file_path_griona(char *out_folder_path, char *file_name, int num_attr, int num_inst, 
    int k_neigh, bool is_norm, bool is_kopt, char *out_path)
{
    char *norm = is_norm ? "norm" : "";
    char *kopt = is_kopt ? "opt" : "";
    size_t stored = snprintf(out_path, MAX_PATH_LEN, "%s/STAT_G-RIONA_%s_A%d_R%d_k%d%s_%s.txt", 
        out_folder_path, file_name, num_attr, num_inst, k_neigh, kopt, norm);
    return (stored >= 0) && (stored < MAX_PATH_LEN);
}

bool get_out_file_path_optk(char *out_folder_path, char *file_name, int num_attr, int num_inst, 
    int k_max, bool is_norm, char *out_path)
{
    char *norm = is_norm ? "norm" : "";
    size_t stored = snprintf(out_path, MAX_PATH_LEN, "%s/OUT_FIND_OPT_K_%s_A%d_R%d_kmax%d_%s.txt", 
        out_folder_path, file_name, num_attr, num_inst, k_max, norm);
    return (stored >= 0) && (stored < MAX_PATH_LEN);
}

bool get_stat_file_path_optk(char *out_folder_path, char *file_name, int num_attr, int num_inst, 
    int k_max, bool is_norm, char *out_path)
{
    char *norm = is_norm ? "norm" : "";
    size_t stored = snprintf(out_path, MAX_PATH_LEN, "%s/STAT_FIND_OPT_K_%s_A%d_R%d_kmax%d_%s.txt", 
        out_folder_path, file_name, num_attr, num_inst, k_max, norm);
    return (stored >= 0) && (stored < MAX_PATH_LEN);
}

void create_out_folder_if_not_exist(char *out_folder_path)
{
    struct stat st = {0};
    if (stat(out_folder_path, &st) == -1) {
        make_directory(out_folder_path);
    }
}

bool save_out_file_griona(char *out_path, int num_inst, int num_attr, float values[][num_attr],  
    char *attr_types, int *classes, int num_k_fold, stats_predict sp[][num_inst], 
    int classes_pred[][num_inst])
{
    FILE * ptr;
    ptr = fopen(out_path, "w");
    if(ptr == NULL)
    {
        printf("Unable to create file.\n");
        return false;
    }

    for (int i = 0; i < num_inst; i++)
    {
        fprintf(ptr, "id:%d,\tattr:", i + 1);
        for (int j = 0; j < num_attr; j++)
        {
            if(attr_types[j] == NUMERICAL_TYPE)
            {
                fprintf(ptr, "%.2f,", values[i][j]);
            }
            else if(attr_types[j] == NOMINAL_TYPE)
            {
                fprintf(ptr, "%0.f,", values[i][j]);
            }
        }
        fprintf(ptr, "\tdec_true:%d,\t[#k+NN_i, #promising k+NN_i, d_i]:", classes[i]);
        for (int k = 0; k < num_k_fold; k++)
        {
            fprintf(ptr, "\t%d,%d,%d,", sp[k][i].k_plus, sp[k][i].k_plus_promising, 
            classes_pred[k][i]);
        }      
        fprintf(ptr, "\n");
    }

    fclose(ptr);
    return true;
}

bool save_out_file_optk(char *out_path, int num_inst, int num_attr, float values[][num_attr],  
    char *attr_types, int *classes, int k_max, int classes_pred[][k_max],
    stats_find_optk *sfoptk)
{
    FILE * ptr;
    ptr = fopen(out_path, "w");
    if(ptr == NULL)
    {
        printf("Unable to create file.\n");
        return false;
    }

    for (int i = 0; i < num_inst; i++)
    {
        fprintf(ptr, "id:%d,\tattr:", i + 1);
        for (int j = 0; j < num_attr; j++)
        {
            if(attr_types[j] == NUMERICAL_TYPE)
            {
                fprintf(ptr, "%.2f,", values[i][j]);
            }
            else if(attr_types[j] == NOMINAL_TYPE)
            {
                fprintf(ptr, "%0.f,", values[i][j]);
            }
        }
        fprintf(ptr, "\tdec_true:%d", classes[i]);
        fprintf(ptr, "\t1+NN: ");
        for (int j = 0; j < sfoptk[i].num_k_plus[0]; j++)
        {
            fprintf(ptr, "%d,", sfoptk[i].k_plus_1[j]);
        }
        fprintf(ptr, "\tpromising 1+NN: ");
        for (int j = 0; j < sfoptk[i].num_k_plus_promising[0]; j++)
        {
            fprintf(ptr, "%d,", sfoptk[i].k_plus_promising_1[j]);
        }
        fprintf(ptr, "\t3+NN: ");
        for (int j = 0; j < sfoptk[i].num_k_plus[2]; j++)
        {
            fprintf(ptr, "%d,", sfoptk[i].k_plus_3[j]);
        }
        fprintf(ptr, "\tpromising 3+NN: ");
        for (int j = 0; j < sfoptk[i].num_k_plus_promising[2]; j++)
        {
            fprintf(ptr, "%d,", sfoptk[i].k_plus_promising_3[j]);
        }
        fprintf(ptr, "\t[#i+NN, #promising i+NN, d_i]: ");
        for (int j = 0; j < k_max; j++)
        {
            fprintf(ptr, "\t%d,%d,%d,", sfoptk[i].num_k_plus[j], 
                sfoptk[i].num_k_plus_promising[j], classes_pred[i][j]);
        }
        fprintf(ptr, "\n");
    }
    
    fclose(ptr);
    return true;    
}

bool save_stat_file_griona(char *stat_path, stats_griona *st)
{
    FILE * ptr;
    ptr = fopen(stat_path, "w");
    if(ptr == NULL)
    {
        printf("Unable to create file.\n");
        return false;
    }

    fprintf(ptr, "name of the input file: %s\n", st->file_name);
    fprintf(ptr, "# of numerical cond. attributes: %d\n", st->num_numerical_attr); 
    fprintf(ptr, "# of nominal cond. attributes: %d\n", st->num_nominal_attr);
    fprintf(ptr, "# of all cond. attributes: %d\n", st->num_attr);
    fprintf(ptr, "# of objects in the input file: %d\n", st->num_inst); 
    fprintf(ptr, "# of distinct decision values: %d\n", st->num_classes);
    char *type_norm = st->is_norm ? "normalized" : "standard";
    fprintf(ptr, "type of G-RIONA: %s\n", type_norm);

    metrics (*pred_metrics)[st->k_fold] = st->pred_metrics;
    for(int i = 0; i < st->num_cv; i++)
    {
        fprintf(ptr, "%d. %d-fold cross-validation:\n", i + 1, st->k_fold);
        fprintf(ptr, "\taccuracies: {");
        for(int j = 0; j < st->k_fold; j++)
        {
            fprintf(ptr, "%.2f,", pred_metrics[i][j].acc);
        }
        fprintf(ptr, "}\n");
        fprintf(ptr, "\tbalanced accuracies: {");
        for(int j = 0; j < st->k_fold; j++)
        {
            fprintf(ptr, "%.2f,", pred_metrics[i][j].balanced_acc);
        }
        fprintf(ptr, "}\n");
        fprintf(ptr, "\taveraged accuracy: %.2f\n", st->avg_metrics[i].acc);
        fprintf(ptr, "\tstandard deviation of accuracy: %.2f\n", st->avg_metrics[i].std_acc);
        fprintf(ptr, "\taveraged balanced  accuracy: %.2f\n", st->avg_metrics[i].balanced_acc);
        fprintf(ptr, "\tstandard deviation of balanced accuracy: %.2f\n", st->avg_metrics[i].std_balanced_acc);
    }

    fprintf(ptr, "final results:\n");
    fprintf(ptr, "\tfinal accuracy: %.2f\n", st->final_metrics.acc);
    fprintf(ptr, "\tstandard deviation of accuracy: %.2f\n", st->final_metrics.std_acc);
    fprintf(ptr, "\tfinal balanced accuracy: %.2f\n", st->final_metrics.balanced_acc);
    fprintf(ptr, "\tstandard deviation of balanced accuracy: %.2f\n", st->final_metrics.std_balanced_acc);

    fprintf(ptr, "runtimes:\n");
    fprintf(ptr, "\ttime of reading the input file: %.10fsec\n", st->time_read_in_file);
    fprintf(ptr, "\tavg time of calculating SVDM metrics per fold: %.10fsec\n", st->tp_avg.time_svdm);
    fprintf(ptr, "\tavg time of determining k+ nearest neighbours per object: %.10fsec\n", st->tp_avg.time_det_k_plus);
    fprintf(ptr, "\tavg time of sorting k+ nearest neighbours per object: %.10fsec\n", st->tp_avg.time_sort_k_plus);
    fprintf(ptr, "\tavg time of rule verification of k+ nearest neighbours per object: %.10fsec\n", st->tp_avg.time_verif_rule);
    fprintf(ptr, "\ttime of storing the results in OUT file: %.10fsec\n", st->time_save_out_file);

    fclose(ptr);
    return true;
}

bool save_stat_file_optk(char *stat_path, stats_optk *st)
{
    FILE * ptr;
    ptr = fopen(stat_path, "w");
    if(ptr == NULL)
    {
        printf("Unable to create file.\n");
        return false;
    }

    fprintf(ptr, "name of the input file: %s\n", st->file_name);
    fprintf(ptr, "# of numerical cond. attributes: %d\n", st->num_numerical_attr); 
    fprintf(ptr, "# of nominal cond. attributes: %d\n", st->num_nominal_attr);
    fprintf(ptr, "# of all cond. attributes: %d\n", st->num_attr);
    fprintf(ptr, "# of objects in the input file: %d\n", st->num_inst); 
    fprintf(ptr, "# of distinct decision values: %d (", st->num_classes);
    for(int j = 0; j < st->num_classes; j++)
    {
        fprintf(ptr, "%d,", st->unique_classes[j]);
    }
    fprintf(ptr, ")\n");
    char *type_norm = st->is_norm ? "normalized" : "standard";
    fprintf(ptr, "type of findOptimalK: %s\n", type_norm);
    fprintf(ptr, "value of k_max: %d\n", st->k_max);
    fprintf(ptr, "value of k_opt: %d\n", st->k_opt);

    fprintf(ptr, "Metrics: \n");
    metrics *pred_metrics = st->pred_metrics;
    fprintf(ptr, "\taccuracy: %.2f\n", st->avg_metrics->acc);
    fprintf(ptr, "\taccuracies for each decision class: {");
    for(int j = 0; j < st->num_classes; j++)
    {
        fprintf(ptr, "%.2f,", pred_metrics[j].acc);
    }
    fprintf(ptr, "}\n");
    fprintf(ptr, "\tbalanced accuracy: %.2f\n", st->avg_metrics->balanced_acc);
    fprintf(ptr, "\tstandard deviation of accuracy: %.2f\n", st->avg_metrics->std_acc);

    fprintf(ptr, "runtimes:\n");
    fprintf(ptr, "\ttotal tile: %.10fsec\n", st->time_total);
    fprintf(ptr, "\ttime of reading the input file: %.10fsec\n", st->time_read_in_file);
    fprintf(ptr, "\ttotal time of calculating SVDM metrics per fold: %.10fsec\n", st->toptk.time_svdm);
    fprintf(ptr, "\ttotal time of determining k+ nearest neighbours per object: %.10fsec\n", st->toptk.time_det_k_plus);
    fprintf(ptr, "\ttotal time of sorting k+ nearest neighbours per object: %.10fsec\n", st->toptk.time_sort_k_plus);
    fprintf(ptr, "\ttotal time of rule verification of k+ nearest neighbours per object: %.10fsec\n", st->toptk.time_verif_rule);
    fprintf(ptr, "\ttime of storing the results in OUT file: %.10fsec\n", st->time_save_out_file);   

    fclose(ptr);
    return true;
}

bool get_num_inst_and_attributes(char *file_path, char *sep, int *num_inst, int *num_attr)
{
    FILE* ptr;
    ptr = fopen(file_path, "r");
    if (NULL == ptr) {
        printf("file can't be opened \n");
        return false;
    }

    *num_inst = 0;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, ptr)) != -1) 
    {
        char *token = strtok(line, sep);
        *num_attr = 0;
        while (token) 
        { 
            token = strtok(NULL, sep);
            (*num_attr)++;
        }
        (*num_inst)++;
    }
    (*num_attr)--;
    return true;
}


bool load_file(file_info *fi, char *file_path, int *classes, int num_attr, 
    float values[][num_attr])
{
    FILE* ptr;
    ptr = fopen(file_path, "r");
    if (NULL == ptr) {
        printf("file can't be opened \n");
        return false;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    int i, j, cnt;
    i = 0;
    while ((read = getline(&line, &len, ptr)) != -1) 
    {
        char *token = strtok(line, fi->sep);
        j = cnt = 0;
        while (token) 
        { 
            float n = atof(token);
            if(j == fi->class_idx)
            {
                classes[i] = n;
            }
            else
            {
                values[i][cnt] = n;
                cnt++;
            }
            token = strtok(NULL, fi->sep);
            j++;
        }
        i++;
    }

    fclose(ptr);
    return true;
}

bool load_info(char *info_path, int num_attr, char *attr_types, char attr_names[][MAX_NAME_LEN], 
    char *target_name, int *class_idx)
{
    FILE* ptr;
    ptr = fopen(info_path, "r");
    if (NULL == ptr) {
        printf("file can't be opened \n");
        return false;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int i, cnt;
    i = cnt = 0;
    while ((read = getline(&line, &len, ptr)) != -1 && i < num_attr + 1) 
    {
        char *type = strtok(line, ",");
        char *name = strtok(NULL, ",");
        if((type[0] == NUMERICAL_TYPE) || (type[0] == NOMINAL_TYPE))
        {
            attr_types[cnt] = type[0];
            strncpy(attr_names[cnt], name, MAX_NAME_LEN);
            cnt++;
        }
        else if (type[0] == TARGET_TYPE)
        {
            strncpy(target_name, name, MAX_NAME_LEN);
            *class_idx = i;
        }
        else
        {
            printf("Wrong type of column %d\n", i+1);
            return false;
        }
        i++;
    }

    fclose(ptr);
    return true;
}