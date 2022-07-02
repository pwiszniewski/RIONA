#include "riona.h"


#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

int get_k_plus(int k_neigh, int num_max, float *dissim, int *indices)
{
    int k_plus = k_neigh;
    while(k_plus < num_max && dissim[indices[k_plus]] == dissim[indices[k_neigh - 1]])
    {
        k_plus++;
    }
    return k_plus;
}

static inline void swap(int *a, int *b) 
{
  int t = *a;
  *a = *b;
  *b = t;
}


static int partition(float *dissim, int *indices, int low, int high) 
{
  float pivot = dissim[indices[high]];
  
  int greater_idx = (low - 1);
  for (int j = low; j < high; j++) 
  {
    if (dissim[indices[j]] <= pivot) 
    {
      greater_idx++;
      swap(&indices[greater_idx], &indices[j]);
    }
  }
  swap(&indices[greater_idx + 1], &indices[high]);
  
  return (greater_idx + 1);
}

static void quicksort_indices(float *dissim, int *indices, int low, int high)
{
    if(low < high)
    {
        int pivot = partition(dissim, indices, low, high);
        quicksort_indices(dissim, indices, low, pivot - 1);
        quicksort_indices(dissim, indices, pivot + 1, high);
    }
}


int cmp_func (float *dissim, const void * a, const void * b) 
{
   float fa = dissim[*(int*)a];
   float fb = dissim[*(int*)b];
   return (fa > fb) - (fa < fb);
}


typedef struct pair {
    int idx;
    float val; 
} pair;


int compare (const void * a, const void * b)
{
  float fa = (*(pair*)a).val;
  float fb = (*(pair*)b).val;
  return (fa > fb) - (fa < fb);
}

// static void quicksort_indices1(float *dissim, int *indices, int num)
// {
//     int wc (const void * a, const void * b) {return(cmp_func(dissim, a,b));};
//     qsort(indices, num, sizeof(*indices), wc);
// }

// static void quicksort_indices2(float *dissim, int *indices, int num)
// {
//     pair parr[num];
//     for (int i = 0; i < num; i++)
//     {
//         parr[i].idx = indices[i];
//         parr[i].val = dissim[i];
//     }

//     qsort(parr, num, sizeof(*parr), compare);

//         for (int i = 0; i < num; i++)
//     {
//         indices[i] = parr[i].idx;
//     }
// }


void sort_indices(int num, float *dissim, int *indices)
{
    quicksort_indices(dissim, indices, 0, num - 1);
}

void copy_k_plus_indices(int k_plus, int **dest, int *indices, int *indices_org)
{
    *dest = malloc(sizeof(*dest) * k_plus);
    for (int kp = 0; kp < k_plus; kp++)
    {
        (*dest)[kp] = indices_org[indices[kp]];
    }
}

void get_classification_vector(float *trn_values, int num_attr, float train_values[][num_attr], 
    int *train_classes, int *indices, int k_plus, int k_max, int num_unique_classes, 
    int *unique_classes, int *classes_cnt, int values_classes_cnt[][num_unique_classes][MAX_UNIQUE_VALUES], 
    float unique_values[][MAX_UNIQUE_VALUES], int unique_values_cnt[][MAX_UNIQUE_VALUES], int *num_unique_values, 
    char *attr_types, int most_freq_class_idx_train, float *dec_strength, bool is_norm, 
    int *num_promising, int *decisions, double *time_verif_rule)
{
    int dec_idx = most_freq_class_idx_train;

    for (int j = 0; j < num_unique_classes; j++)
    {
        dec_strength[j] = 0;
    }

    int cnt_promising = 0;
    for (int k = 0; k < k_plus; k++)
    {
        double time_verif_rule_obj_s = get_time_sec();        
        bool is_consist = is_consistent(trn_values, num_attr, train_values, 
        train_classes, k, indices, k_plus, num_unique_classes, values_classes_cnt, 
        unique_values, unique_values_cnt, num_unique_values, attr_types);
        *time_verif_rule += get_time_sec() - time_verif_rule_obj_s; 

        if(is_consist)
        {
            int idx = get_unique_class_idx(num_unique_classes, unique_classes, 
                train_classes[indices[k]]);

            if(is_norm)
            {
                dec_strength[idx] += 1.0 / classes_cnt[idx];
            }
            else
            {
                dec_strength[idx] += 1;
            }

            if (dec_strength[idx] > dec_strength[dec_idx])
            {
                dec_idx = idx;
            }
            cnt_promising++;
        }
        if(k < k_max)
        {
            decisions[k] = unique_classes[dec_idx];
        }
        num_promising[k] = cnt_promising;
    }
}

static void translate_indices(int num, int *indices, int *indices_org, int *indices_org_sorted)
{
    for (int j = 0; j < num; j++)
    {
        indices_org_sorted[j] = indices_org[indices[j]];
    }
}

void get_k_opt(int k_max, int num_train, int decisions[][k_max], int *train_classes, 
    int num_unique_classes, int *unique_classes, int *classes_cnt, bool is_norm, int *k_opt)
{
    int k_best = 1;
    float sum_max = 0;
    for (int k = 0; k < k_max; k++)
    {
        float curr_sum = 0;
        for (int i = 0; i < num_train; i++)
        {
            if(decisions[i][k] == train_classes[i])
            {
                if(is_norm)
                {
                    int idx = get_unique_class_idx(num_unique_classes, unique_classes, 
                        train_classes[i]);
                    curr_sum += 1.0 / classes_cnt[idx];
                }
                else
                {
                    curr_sum += 1;
                }
            }
        }
        
        if(curr_sum > sum_max)
        {
            sum_max = curr_sum;
            k_best = k + 1;
        }
    }
    *k_opt = k_best;
}

void get_num_k_plus_promising(int k_max, int *num_k_plus_promising, int *num_promising, 
    int *num_k_plus)
{
    for (int k = 1; k <= k_max; k++)
    {
        num_k_plus_promising[k-1] = num_promising[num_k_plus[k-1]-1];
    }
}

void find_optimal_k(int k_max, int num_train, char *attr_types, int num_attr, 
    float values[][num_attr], int *classes, int num_unique_classes, int *unique_classes, 
    int *classes_cnt, float *min_values, float *max_values, bool is_norm, int *k_opt,
    int classes_pred[][k_max], stats_find_optk *sfoptk, timing_optk *toptk)
{
    int most_freq_class_idx_train;
    get_most_freq_class_idx(classes_cnt, num_unique_classes, &most_freq_class_idx_train); 

    int *num_unique_values = malloc( sizeof(*num_unique_values) * num_attr);
    float (*unique_values) [MAX_UNIQUE_VALUES] = malloc( sizeof(float[num_attr][MAX_UNIQUE_VALUES]));
    get_unique_values(num_attr, attr_types, num_train, values, num_unique_values, unique_values);

    int values_classes_cnt[num_attr][num_unique_classes][MAX_UNIQUE_VALUES];
    int (*unique_values_cnt)[MAX_UNIQUE_VALUES] = malloc(sizeof(int[num_attr][MAX_UNIQUE_VALUES]));
    get_unique_values_cnt(num_attr, attr_types, num_unique_classes, unique_classes, classes, 
        num_unique_values, num_train, MAX_UNIQUE_VALUES, values, unique_values, values_classes_cnt, 
        unique_values_cnt);

    memset(toptk, 0, sizeof(*toptk));

    #pragma omp parallel for 
    for (int i = 0; i < num_train; i++)
    {
        float *dissim_train = malloc(sizeof(*dissim_train) * (num_train - 1));
        int *indices = malloc(sizeof(*indices) * (num_train - 1));
        int *indices_org = malloc(sizeof(*indices_org) * (num_train - 1));
        int *indices_org_sorted = malloc(sizeof(*indices_org_sorted) * (num_train - 1));

        int jj = 0;
        double time_svdm_s = get_time_sec();
        
        for (int j = 0; j < num_train; j++)
        {
            if(j == i) { continue;}

            calculate_dissimilairty(values[i], values[j], num_attr, attr_types,
            min_values, max_values, num_unique_classes, values_classes_cnt, unique_values, 
            unique_values_cnt, num_unique_values, &dissim_train[jj]);
            indices[jj] = jj;
            indices_org[jj] = j;
            jj++;
        }
        toptk->time_svdm += get_time_sec() - time_svdm_s;    
        double time_sort_k_plus_obj_s = get_time_sec();        
        sort_indices(num_train - 1, dissim_train, indices);
        toptk->time_sort_k_plus += get_time_sec() - time_sort_k_plus_obj_s;

        translate_indices(num_train - 1, indices, indices_org, indices_org_sorted);
        
        double time_det_k_plus_obj_s = get_time_sec();
        for (int k = 1; k <= k_max; k++)
        {
            sfoptk[i].num_k_plus[k-1] = get_k_plus(k, num_train, dissim_train, indices);
            if(k == 1)
            {
                copy_k_plus_indices(sfoptk[i].num_k_plus[k-1], &(sfoptk[i].k_plus_1), indices, 
                    indices_org);
            }
            else if(k == 3)
            {
                copy_k_plus_indices(sfoptk[i].num_k_plus[k-1], &(sfoptk[i].k_plus_3), indices, 
                    indices_org);
            }
        }
        toptk->time_det_k_plus += get_time_sec() - time_det_k_plus_obj_s;

        free(dissim_train);

        float *dec_strength = malloc(sizeof(*dec_strength) * num_unique_classes);
        int k_max_plus = sfoptk[i].num_k_plus[k_max-1];
        int num_promising[k_max_plus];
        get_classification_vector(values[i], num_attr, values, 
            classes, indices_org_sorted, k_max_plus, k_max, 
            num_unique_classes, unique_classes, classes_cnt, values_classes_cnt, unique_values,
            unique_values_cnt, num_unique_values, attr_types, most_freq_class_idx_train, 
            dec_strength, is_norm, num_promising, classes_pred[i], 
            &(toptk->time_verif_rule));

        free(dec_strength);
        free(indices_org_sorted);

        get_num_k_plus_promising(k_max, sfoptk[i].num_k_plus_promising, num_promising, 
            sfoptk[i].num_k_plus);

        copy_k_plus_indices(sfoptk[i].num_k_plus_promising[0], &(sfoptk[i].k_plus_promising_1), 
            indices, indices_org);   
        copy_k_plus_indices(sfoptk[i].num_k_plus_promising[2], &(sfoptk[i].k_plus_promising_3), 
            indices, indices_org); 

        free(indices);
        free(indices_org);
    }
    free(unique_values);
    free(unique_values_cnt);
    free(num_unique_values);

    get_k_opt(k_max, num_train, classes_pred, classes, num_unique_classes, 
        unique_classes, classes_cnt, is_norm, k_opt);
}


int get_decision_with_greatest_support(int num_unique_classes, int *dec_strength)
{
    int dec = 0;
    for (int j = 1; j < num_unique_classes; j++)
    {
        if (dec_strength[j] > dec_strength[dec])
        {
            dec = j;
        }
    }
    return dec;
}

int get_decision_with_greatest_norm_support(int num_unique_classes, int *dec_strength, 
    int *train_classes_cnt)
{
    int dec = 0;
    for (int j = 1; j < num_unique_classes; j++)
    {
        if ((float)dec_strength[j] / train_classes_cnt[j] > (float)dec_strength[dec] / train_classes_cnt[dec])
        {
            dec = j;
        }
    }
    return dec;
}

void calculate_decisions_strength(int num_unique_classes, int *unique_classes, int *dec_strength, int k_plus,
    int num_attr, float *test_values, float train_values[][num_attr], int *train_classes, 
    int *indices,
    int values_classes_cnt[][num_unique_classes][MAX_UNIQUE_VALUES], float unique_values[][MAX_UNIQUE_VALUES], 
    int unique_values_cnt[][MAX_UNIQUE_VALUES], int *num_unique_values, char *attr_types,
    int *k_plus_promising)
{
    memset(dec_strength, 0, sizeof(*dec_strength) * num_unique_classes);
    for (int j = 0; j < k_plus; j++)
    {           
        bool is_consist = is_consistent(test_values, num_attr, train_values, 
                train_classes, j, indices, k_plus, num_unique_classes, values_classes_cnt, 
                unique_values, unique_values_cnt, num_unique_values, attr_types);        
        if(is_consist)
        {
            int idx = get_unique_class_idx(num_unique_classes, unique_classes, train_classes[indices[j]]);
            dec_strength[idx]++;
            (*k_plus_promising)++;
        }
    }
}

void predict(int num_train, int num_test, int num_attr, int k_neigh, float train_values[num_train][num_attr], 
    int *train_classes, float test_values[num_test][num_attr], char *attr_types, 
    int num_unique_classes, int *unique_classes, int *test_classes_pred, bool is_norm,
    stats_predict *sp, timing_predict *tp)
{
    static int counter = 0;

    float min_values_train[num_attr];
    float max_values_train[num_attr];
    get_min_max_attributes(num_attr, num_train, train_values, min_values_train, 
    max_values_train);

    int train_classes_cnt[num_unique_classes];
    get_classes_cnt(num_unique_classes, unique_classes, num_train, train_classes,
        train_classes_cnt);

    int num_unique_values[num_attr];
    float unique_values[num_attr][MAX_UNIQUE_VALUES];
    get_unique_values(num_attr, attr_types, num_train, train_values, num_unique_values, unique_values);

    int (*unique_values_cnt)[MAX_UNIQUE_VALUES] = malloc(sizeof(int[num_attr][MAX_UNIQUE_VALUES]));
    int (*values_classes_cnt)[num_unique_classes][MAX_UNIQUE_VALUES] = malloc(sizeof(int[num_attr][num_unique_classes][MAX_UNIQUE_VALUES]));
    get_unique_values_cnt(num_attr, attr_types, num_unique_classes, unique_classes, train_classes, 
    num_unique_values, num_train, MAX_UNIQUE_VALUES, train_values, unique_values, values_classes_cnt, 
    unique_values_cnt);

    memset(tp, 0, sizeof(*tp));
    memset(sp, 0, sizeof(*sp) * num_test);

    #if defined(VECT) || defined(CUDA)
    float (*train_values_t) [num_train] = malloc( sizeof(float[num_attr][num_train]) );
    cblas_somatcopy(CblasRowMajor, CblasTrans, num_train, num_attr, 1.0, 
        (float *)train_values, num_attr, (float *)train_values_t, num_train);
    #endif

    #ifdef CUDA
    float (*test_values_t) [num_test] = malloc( sizeof(float[num_attr][num_test]) );
    cblas_somatcopy(CblasRowMajor, CblasTrans, num_test, num_attr, 1.0, 
        (float *)test_values, num_attr, (float *)test_values_t, num_test);
    copy_train_test_cuda(num_train, num_test, num_attr, (float *)train_values_t,
    (float *)test_values_t);
    #endif

    #pragma omp parallel for 
    for (int i = 0; i < num_test; i++)
    {
        float *dissim = malloc(sizeof(*dissim) * num_train);
        memset(dissim, 0, sizeof(*dissim) * num_train);
        int *indices = malloc(sizeof(*indices) * num_train);
        double time_svdm_s = get_time_sec();

        #ifdef VECT
        for (int j = 0; j < num_train; j++)
        {
            indices[j] = j;
        }
        calculate_dissimilairty_vect(test_values[i], num_train, (float *)train_values_t, 
        num_attr, attr_types, min_values_train, max_values_train, num_unique_classes, 
        values_classes_cnt, unique_values, unique_values_cnt, num_unique_values, dissim);
        #elif CUDA
        for (int j = 0; j < num_train; j++)
        {
            indices[j] = j;
        }
        calculate_dissimilairty_cuda(omp_get_thread_num(), test_values[i], num_train, (float *)train_values_t, 
        num_attr, attr_types, min_values_train, max_values_train, num_unique_classes, 
        values_classes_cnt, unique_values, unique_values_cnt, num_unique_values, dissim);
        #else
        for (int j = 0; j < num_train; j++)
        {
            calculate_dissimilairty(test_values[i], train_values[j], num_attr, attr_types,
            min_values_train, max_values_train, num_unique_classes, values_classes_cnt, unique_values, 
            unique_values_cnt, num_unique_values, &dissim[j]);
            indices[j] = j;
        }
        #endif

        tp->time_svdm += get_time_sec() - time_svdm_s;

        double time_sort_k_plus_obj_s = get_time_sec();
        sort_indices(num_train, dissim, indices);
        tp->time_sort_k_plus += get_time_sec() - time_sort_k_plus_obj_s;

    
        

        double time_det_k_plus_obj_s = get_time_sec();
        sp[i].k_plus = get_k_plus(k_neigh, num_train, dissim, indices);
        tp->time_det_k_plus += get_time_sec() - time_det_k_plus_obj_s;

        int *dec_strength = malloc(sizeof(*dec_strength) * num_unique_classes);

        double time_verif_rule_obj_s = get_time_sec();
        calculate_decisions_strength(num_unique_classes, unique_classes, dec_strength, 
            sp[i].k_plus, num_attr, test_values[i], train_values, train_classes, indices, 
            values_classes_cnt, unique_values, unique_values_cnt, num_unique_values, attr_types,
            &(sp[i].k_plus_promising));
        tp->time_verif_rule += get_time_sec() - time_verif_rule_obj_s;

        free(dissim);
        free(indices);

        int dec;
        if(is_norm)
        {
            dec = get_decision_with_greatest_norm_support(num_unique_classes, dec_strength, 
                train_classes_cnt);
        }
        else
        {
            dec = get_decision_with_greatest_support(num_unique_classes, dec_strength);
        }
        test_classes_pred[i] = unique_classes[dec];

        free(dec_strength);
    }
    free(unique_values_cnt);
    free(values_classes_cnt);
    #ifdef VECT
    free(train_values_t);
    #endif
        
    #ifdef CUDA
    free_test_train_cuda();
    free(train_values_t);
    free(test_values_t);
    #endif


    tp->time_det_k_plus /= num_test;
    tp->time_sort_k_plus /= num_test; 
    tp->time_verif_rule /= num_test;

    counter++;
}


static void get_start_end(float curr_dissim_ref, int k_neigh, int num_train, float *dissim_ref,
    int *indices_ref, int *start, int *end)
{
    int pos = 0;
    while(pos < num_train)
    {
        if(curr_dissim_ref < dissim_ref[indices_ref[pos]])
        {
            break;
        }
        pos++;
    }

    *start = MAX(0, pos - k_neigh/2);
    *end = MIN(num_train - 1, k_neigh + *start - 1);
    *start = MAX(0, *end - k_neigh + 1);
}

static void update_cnt_and_max_dissim(int k_neigh, int idx, float *dissim, int *indices, int *cnt,
    float *max_dissim)
{
    if(dissim[idx] < *max_dissim)
    {
        int cnt_keep = *cnt;
        
        cnt_keep--;
        while(cnt_keep > 0 && dissim[indices[cnt_keep - 1]] == *max_dissim)
        {
            cnt_keep--;
        }
        if(cnt_keep < k_neigh - 1)
        {
            int m = *cnt - 1;
            while(m >= 0 && dissim[idx] < dissim[indices[m]])
            {
                indices[m + 1] = indices[m];
                m--;
            }
            indices[m + 1] = idx;
            (*cnt)++;
        }
        else
        {
            *cnt = cnt_keep;
            int m = *cnt - 1;
            while(m >= 0 && dissim[idx] < dissim[indices[m]])
            {
                indices[m + 1] = indices[m];
                m--;
            }
            indices[m + 1] = idx;
            (*cnt)++;
            *max_dissim = dissim[indices[*cnt - 1]];
        }

    }
    else if (dissim[idx] == *max_dissim)
    {
        indices[*cnt] = idx;
        (*cnt)++;
    }
}



void predict_triangle_ineq(int num_train, int num_test, int num_attr, int k_neigh, 
    float train_values[num_train][num_attr], int *train_classes, 
    float test_values[num_test][num_attr], char *attr_types, int num_unique_classes, 
    int *unique_classes, int *test_classes_pred, bool is_norm, stats_predict *sp, 
    timing_predict *tp)
{

    float min_values_train[num_attr];
    float max_values_train[num_attr];
    get_min_max_attributes(num_attr, num_train, train_values, min_values_train, 
    max_values_train);
    int train_classes_cnt[num_unique_classes];
    get_classes_cnt(num_unique_classes, unique_classes, num_train, train_classes,
        train_classes_cnt);
    int num_unique_values[num_attr];
    float unique_values[num_attr][MAX_UNIQUE_VALUES];
    get_unique_values(num_attr, attr_types, num_train, train_values, num_unique_values, unique_values);
    int (*unique_values_cnt)[MAX_UNIQUE_VALUES] = malloc(sizeof(int[num_attr][MAX_UNIQUE_VALUES]));
    int (*values_classes_cnt)[num_unique_classes][MAX_UNIQUE_VALUES] = malloc(sizeof(int[num_attr][num_unique_classes][MAX_UNIQUE_VALUES]));
    get_unique_values_cnt(num_attr, attr_types, num_unique_classes, unique_classes, train_classes, 
    num_unique_values, num_train, MAX_UNIQUE_VALUES, train_values, unique_values, values_classes_cnt, 
    unique_values_cnt);
    memset(tp, 0, sizeof(*tp));
    memset(sp, 0, sizeof(*sp) * num_test);

//////////////// calculate reference distances ///////////////////////////

    float *dissim_ref = malloc(sizeof(*dissim_ref) * num_train);
    memset(dissim_ref, 0, sizeof(*dissim_ref) * num_train);
    int *indices_ref = malloc(sizeof(*indices_ref) * num_train);
    float *ref_values = malloc(sizeof(*ref_values) * num_attr);
    for (int j = 0; j < num_attr; j++)
    {
        ref_values[j] = max_values_train[j];
    }
    

    double time_svdm_s = get_time_sec();
    for (int j = 0; j < num_train; j++)
    {
        calculate_dissimilairty(ref_values, train_values[j], num_attr, attr_types,
        min_values_train, max_values_train, num_unique_classes, values_classes_cnt, unique_values, 
        unique_values_cnt, num_unique_values, &dissim_ref[j]);
        indices_ref[j] = j;
    }
    tp->time_svdm += get_time_sec() - time_svdm_s;

    double time_sort_k_plus_obj_s = get_time_sec();
    sort_indices(num_train, dissim_ref, indices_ref);
    tp->time_sort_k_plus += get_time_sec() - time_sort_k_plus_obj_s;

///////////////////////////////////////////////////////////////

    #pragma omp parallel for 
    for (int i = 0; i < num_test; i++)
    {
        float *dissim = malloc(sizeof(*dissim) * num_train);
        memset(dissim, 0, sizeof(*dissim) * num_train);
        int *indices = malloc(sizeof(*indices) * num_train);

        float curr_dissim_ref;
        calculate_dissimilairty(ref_values, test_values[i], num_attr, attr_types,
        min_values_train, max_values_train, num_unique_classes, values_classes_cnt, unique_values, 
        unique_values_cnt, num_unique_values, &curr_dissim_ref);

        int start, end;
        get_start_end(curr_dissim_ref, k_neigh, num_train, dissim_ref, indices_ref, &start, &end);

        double time_svdm_s = get_time_sec();
        int cnt = 0;
        for (int j = start; j <= end; j++)
        {
            calculate_dissimilairty(test_values[i], train_values[indices_ref[j]], num_attr, attr_types,
            min_values_train, max_values_train, num_unique_classes, values_classes_cnt, unique_values, 
            unique_values_cnt, num_unique_values, &dissim[indices_ref[j]]);
            indices[cnt] = indices_ref[j];
            cnt++;
        }
        tp->time_svdm += get_time_sec() - time_svdm_s;


        double time_sort_k_plus_obj_s = get_time_sec();
        sort_indices(cnt, dissim, indices);
        tp->time_sort_k_plus += get_time_sec() - time_sort_k_plus_obj_s;

        float max_dissim = dissim[indices[cnt - 1]] ;

        // Backward
        start--;
        while(start >= 0) 
        {
            int idx = indices_ref[start];
            if(dissim_ref[idx] < curr_dissim_ref - max_dissim)
            {
                break;
            }
            start--;
        }
        start++;

        // Forward
        end++;
        while(end < num_train) 
        {
            int idx = indices_ref[end];
            if(dissim_ref[idx] > curr_dissim_ref + max_dissim)
            {
                break;
            }
            end++;
        }
        end--;


        time_svdm_s = get_time_sec();
        cnt = 0;
        for (int j = start; j <= end; j++)
        {
            calculate_dissimilairty(test_values[i], train_values[indices_ref[j]], num_attr, attr_types,
            min_values_train, max_values_train, num_unique_classes, values_classes_cnt, unique_values, 
            unique_values_cnt, num_unique_values, &dissim[indices_ref[j]]);
            indices[cnt] = indices_ref[j];
            cnt++;
        }
        tp->time_svdm += get_time_sec() - time_svdm_s;

        time_sort_k_plus_obj_s = get_time_sec();
        sort_indices(cnt, dissim, indices);
        tp->time_sort_k_plus += get_time_sec() - time_sort_k_plus_obj_s;


        double time_det_k_plus_obj_s = get_time_sec();
        sp[i].k_plus = get_k_plus(k_neigh, cnt, dissim, indices);
        tp->time_det_k_plus += get_time_sec() - time_det_k_plus_obj_s;

        int *dec_strength = malloc(sizeof(*dec_strength) * num_unique_classes);

        double time_verif_rule_obj_s = get_time_sec();
        calculate_decisions_strength(num_unique_classes, unique_classes, dec_strength, 
            sp[i].k_plus, num_attr, test_values[i], train_values, train_classes, indices, 
            values_classes_cnt, unique_values, unique_values_cnt, num_unique_values, attr_types,
            &(sp[i].k_plus_promising));
        tp->time_verif_rule += get_time_sec() - time_verif_rule_obj_s;

        free(dissim);
        free(indices);

        int dec;
        if(is_norm)
        {
            dec = get_decision_with_greatest_norm_support(num_unique_classes, dec_strength, 
                train_classes_cnt);
        }
        else
        {
            dec = get_decision_with_greatest_support(num_unique_classes, dec_strength);
        }
        test_classes_pred[i] = unique_classes[dec];

        free(dec_strength);

    }
    free(unique_values_cnt);
    free(values_classes_cnt);
            free(dissim_ref);
        free(indices_ref);

    tp->time_det_k_plus /= num_test;
    tp->time_sort_k_plus /= num_test; 
    tp->time_verif_rule /= num_test;
}


void predict_triangle_ineq_with_updating_eps_neigh(int num_train, int num_test, int num_attr, int k_neigh, 
    float train_values[num_train][num_attr], int *train_classes, 
    float test_values[num_test][num_attr], char *attr_types, int num_unique_classes, 
    int *unique_classes, int *test_classes_pred, bool is_norm, stats_predict *sp, 
    timing_predict *tp)
{

    float min_values_train[num_attr];
    float max_values_train[num_attr];
    get_min_max_attributes(num_attr, num_train, train_values, min_values_train, 
    max_values_train);
    int train_classes_cnt[num_unique_classes];
    get_classes_cnt(num_unique_classes, unique_classes, num_train, train_classes,
        train_classes_cnt);
    int num_unique_values[num_attr];
    float unique_values[num_attr][MAX_UNIQUE_VALUES];
    get_unique_values(num_attr, attr_types, num_train, train_values, num_unique_values, unique_values);
    int (*unique_values_cnt)[MAX_UNIQUE_VALUES] = malloc(sizeof(int[num_attr][MAX_UNIQUE_VALUES]));
    int (*values_classes_cnt)[num_unique_classes][MAX_UNIQUE_VALUES] = malloc(sizeof(int[num_attr][num_unique_classes][MAX_UNIQUE_VALUES]));
    get_unique_values_cnt(num_attr, attr_types, num_unique_classes, unique_classes, train_classes, 
    num_unique_values, num_train, MAX_UNIQUE_VALUES, train_values, unique_values, values_classes_cnt, 
    unique_values_cnt);
    memset(tp, 0, sizeof(*tp));
    memset(sp, 0, sizeof(*sp) * num_test);

//////////////// calculate reference distances ///////////////////////////

    float *dissim_ref = malloc(sizeof(*dissim_ref) * num_train);
    memset(dissim_ref, 0, sizeof(*dissim_ref) * num_train);
    int *indices_ref = malloc(sizeof(*indices_ref) * num_train);
    float *ref_values = malloc(sizeof(*ref_values) * num_attr);
    for (int j = 0; j < num_attr; j++)
    {
        ref_values[j] = max_values_train[j];
    }
    

    double time_svdm_s = get_time_sec();
    for (int j = 0; j < num_train; j++)
    {
        calculate_dissimilairty(ref_values, train_values[j], num_attr, attr_types,
        min_values_train, max_values_train, num_unique_classes, values_classes_cnt, unique_values, 
        unique_values_cnt, num_unique_values, &dissim_ref[j]);
        indices_ref[j] = j;
    }
    tp->time_svdm += get_time_sec() - time_svdm_s;

    double time_sort_k_plus_obj_s = get_time_sec();
    sort_indices(num_train, dissim_ref, indices_ref);
    tp->time_sort_k_plus += get_time_sec() - time_sort_k_plus_obj_s;

///////////////////////////////////////////////////////////////

    #pragma omp parallel for 
    for (int i = 0; i < num_test; i++)
    {
        float *dissim = malloc(sizeof(*dissim) * num_train);
        memset(dissim, 0, sizeof(*dissim) * num_train);
        int *indices = malloc(sizeof(*indices) * num_train);

        float curr_dissim_ref;
        calculate_dissimilairty(ref_values, test_values[i], num_attr, attr_types,
        min_values_train, max_values_train, num_unique_classes, values_classes_cnt, unique_values, 
        unique_values_cnt, num_unique_values, &curr_dissim_ref);

        int start, end;
        get_start_end(curr_dissim_ref, k_neigh, num_train, dissim_ref, indices_ref, &start, &end);

        double time_svdm_s = get_time_sec();
        int cnt = 0;
        for (int j = start; j <= end; j++)
        {
            calculate_dissimilairty(test_values[i], train_values[indices_ref[j]], num_attr, attr_types,
            min_values_train, max_values_train, num_unique_classes, values_classes_cnt, unique_values, 
            unique_values_cnt, num_unique_values, &dissim[indices_ref[j]]);
            indices[cnt] = indices_ref[j];
            cnt++;
        }
        tp->time_svdm += get_time_sec() - time_svdm_s;


        double time_sort_k_plus_obj_s = get_time_sec();
        sort_indices(cnt, dissim, indices);
        tp->time_sort_k_plus += get_time_sec() - time_sort_k_plus_obj_s;

        float max_dissim = dissim[indices[cnt - 1]] ;

        // Backward
        start--;
        while(start >= 0) 
        {
            int idx = indices_ref[start];
            if(dissim_ref[idx] < curr_dissim_ref - max_dissim)
            {
                break;
            }
            calculate_dissimilairty(test_values[i], train_values[idx], num_attr, attr_types,
            min_values_train, max_values_train, num_unique_classes, values_classes_cnt, unique_values, 
            unique_values_cnt, num_unique_values, &dissim[idx]);
            update_cnt_and_max_dissim(k_neigh, idx, dissim, indices, &cnt, &max_dissim);
            start--;
        }
        start++;

        // Forward
        end++;
        while(end < num_train) 
        {
            int idx = indices_ref[end];
            if(dissim_ref[idx] > curr_dissim_ref + max_dissim)
            {
                break;
            }
            calculate_dissimilairty(test_values[i], train_values[idx], num_attr, attr_types,
            min_values_train, max_values_train, num_unique_classes, values_classes_cnt, unique_values, 
            unique_values_cnt, num_unique_values, &dissim[idx]);
            update_cnt_and_max_dissim(k_neigh, idx, dissim, indices, &cnt, &max_dissim);
            end++;
        }
        end--;


        double time_det_k_plus_obj_s = get_time_sec();
        sp[i].k_plus = cnt;

        tp->time_det_k_plus += get_time_sec() - time_det_k_plus_obj_s;
        int *dec_strength = malloc(sizeof(*dec_strength) * num_unique_classes);

        double time_verif_rule_obj_s = get_time_sec();
        calculate_decisions_strength(num_unique_classes, unique_classes, dec_strength, 
            sp[i].k_plus, num_attr, test_values[i], train_values, train_classes, indices, 
            values_classes_cnt, unique_values, unique_values_cnt, num_unique_values, attr_types,
            &(sp[i].k_plus_promising));
        tp->time_verif_rule += get_time_sec() - time_verif_rule_obj_s;

        free(dissim);
        free(indices);

        int dec;
        if(is_norm)
        {
            dec = get_decision_with_greatest_norm_support(num_unique_classes, dec_strength, 
                train_classes_cnt);
        }
        else
        {
            dec = get_decision_with_greatest_support(num_unique_classes, dec_strength);
        }
        test_classes_pred[i] = unique_classes[dec];

        free(dec_strength);

    }
    free(unique_values_cnt);
    free(values_classes_cnt);
            free(dissim_ref);
        free(indices_ref);

    tp->time_det_k_plus /= num_test;
    tp->time_sort_k_plus /= num_test; 
    tp->time_verif_rule /= num_test;
}