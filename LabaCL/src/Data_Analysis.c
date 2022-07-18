#include "../include/LabaCL.h"
#include "My_Lib.h"

double Avg (const double *array, const size_t n_elems)
{    
    MY_ASSERT (array, "const double *array", NULL_PTR, (double)ERROR);
    
    double sum = 0.0;
    
    for (size_t elem_i = 0; elem_i < n_elems; elem_i++)
        sum += array[elem_i];

    return (sum / (double)n_elems);
}

double Root_Mean_Square (const double *array, const size_t n_elems)
{    
    MY_ASSERT (array, "const double *array", NULL_PTR, (double)ERROR);
    
    double sq_sum = 0.0;
    
    for (size_t elem_i = 0; elem_i < n_elems; elem_i++)
        sq_sum += array[elem_i] * array[elem_i];

    return sqrt(sq_sum / (double)n_elems);
}

double Avg_Err_Rand (const double *array, const size_t n_elems)
{
    MY_ASSERT (array,       "const double *array",  NULL_PTR,  (double)ERROR);
    MY_ASSERT (n_elems > 1, "const size_t n_elems", UNEXP_VAL, (double)ERROR);

    double avg = Avg (array, n_elems);

    double sum = 0.0;
    for (size_t elem_i = 0; elem_i < n_elems; elem_i++)
        sum += pow (array[elem_i] - avg, 2.0);

    double err_sq = sum / (n_elems * (n_elems - 1));
    
    return sqrt (err_sq);
}

int Least_Squares (const double *arr_x, const double *arr_y, const size_t n_elems, struct Least_Sq *least_sq)
{
    MY_ASSERT (arr_x,    "const double *arr_x",       NULL_PTR, ERROR);
    MY_ASSERT (arr_y,    "const double *arr_y",       NULL_PTR, ERROR);
    MY_ASSERT (least_sq, "struct Least_Sq *least_sq", NULL_PTR, ERROR);
    
    double x_sum    = 0.0;  // sum [x]
    double y_sum    = 0.0;  // sum [y]
    double x_sq_sum = 0.0;  // sum [x^2]
    double xy_sum   = 0.0;  // sum [xy]

    for (size_t elem_i = 0; elem_i < n_elems; elem_i++)
    {
        x_sum    += arr_x[elem_i];

        y_sum    += arr_y[elem_i];

        x_sq_sum += arr_x[elem_i] * arr_x[elem_i];

        xy_sum   += arr_x[elem_i] * arr_y[elem_i];
    }
    
    least_sq->k_coeff = (x_sum * y_sum  - n_elems * xy_sum) / (x_sum * x_sum - n_elems * x_sq_sum);
    least_sq->b_coeff = (x_sum * xy_sum - x_sq_sum * y_sum) / (x_sum * x_sum - n_elems * x_sq_sum);

    double x_avg = x_sum / n_elems;
    double up_sum   = 0.0;  // sum [(y - <y>)^2]
    double down_sum = 0.0;  // sum [(x - <x>)^2]

    for (size_t elem_i = 0; elem_i < n_elems; elem_i++)
    {
        up_sum   += pow ( (arr_y[elem_i] - (least_sq->k_coeff * arr_x[elem_i] + least_sq->b_coeff)), 2 );

        down_sum += pow ( (arr_x[elem_i] - x_avg), 2 );
    }
    
    least_sq->k_err = sqrt (up_sum / (down_sum * (n_elems - 2) ) );
    least_sq->b_err = sqrt ( (up_sum * x_sq_sum) / (n_elems * (n_elems - 2) * down_sum) );

    return NO_ERRORS;
}

// ====================================================================== CHI-SQUARE METHOD ====================================================================== //

#if 0
static double Calculate_Exam      (const struct Graph *graph, const size_t row_i, const size_t column_j);
static double Calculate_Free_Elem (const struct Graph *graph, const size_t row_i, const struct Node *func_tree, const struct Var *vars_arr, const int n_vars);

double *Chi_Square (const struct Node *func_tree, const struct Var *vars_arr, const int n_vars, struct Graph *graph)
{
    MY_ASSERT (func_tree, "const struct Tree *func_tree", NULL_PTR, NULL);
    MY_ASSERT (graph,     "const double *arr_x",          NULL_PTR, NULL);
    
    const size_t n_rows = graph->approx_pow + 1;
    double *matrix = (double *)calloc (n_rows * (n_rows + 1), sizeof (double));

    for (size_t row_i = 0; row_i < n_rows; row_i++)
    {
        for (size_t column_j = 0; column_j < n_rows; column_j++)
            matrix[row_i * (n_rows + 1) + column_j] = Calculate_Exam (graph, row_i, column_j);

        matrix[row_i * (n_rows + 1) + n_rows] = Calculate_Free_Elem (graph, row_i, func_tree, vars_arr, n_vars);
    }

    double *params = (double *)calloc (n_rows, sizeof (double));
    SLE_solver (matrix, n_rows, params);

    free (matrix);

    return params;
}

static double Calculate_Exam (const struct Graph *graph, const size_t row_i, const size_t column_j)
{
    double elem = 0.0;
    const size_t n_rows = graph->approx_pow + 1;

    if (column_j == n_rows - 1)
    {
        for (int exam_i = 0; exam_i < graph->n_dots; exam_i++)
            elem += (1 / (graph->y_err[exam_i] * graph->y_err[exam_i]));
    }
    else
    {
        for (int exam_i = 0; exam_i < graph->n_dots; exam_i++)
            elem += (pow (graph->x_arr[exam_i], n_rows + row_i - column_j) / (graph->y_err[exam_i] * graph->y_err[exam_i]));
    }
    
    return elem;
}

static double Calculate_Free_Elem (const struct Graph *graph, const size_t row_i, const struct Node *func_tree, const struct Var *vars_arr, const int n_vars)
{
    double elem = 0.0;

    if (row_i == 0)
    {
        for (int exam_i = 0; exam_i < graph->n_dots; exam_i++)
            elem += (Calculate_Tree (func_tree, vars_arr, n_vars) / (graph->y_err[exam_i] * graph->y_err[exam_i]));
    }   
    else
    {
        for (int exam_i = 0; exam_i < graph->n_dots; exam_i++)
            elem += (Calculate_Tree (func_tree, vars_arr, n_vars) / (graph->y_err[exam_i] * graph->y_err[exam_i]) * pow (graph->x_arr[exam_i], row_i));
    }

    return elem;
}
#endif

struct Chi_Sq *Linear_Chi_Square (const struct Graph *graph)
{
    MY_ASSERT (graph, "struct Graph *graph", NULL_PTR, NULL);

    double *y = graph->y_arr;
    double *x = graph->x_arr;
    double *x_err = graph->x_err;
    double *y_err = graph->y_err;

    double A = 0.0, B = 0.0, C = 0.0, D_0 = 0.0, D_1 = 0.0;

    for (int dot_i = 0; dot_i < graph->n_dots; dot_i++)
    {
        double err_sq = y_err[dot_i] * y_err[dot_i];
        
        A   += 1 / err_sq;
        B   += x[dot_i] / err_sq;
        C   += (x[dot_i] * x[dot_i]) / err_sq;
        D_0 += y[dot_i] / err_sq;
        D_1 += (x[dot_i] * y[dot_i]) / err_sq;  
    }

    double *der_b_by_x = (double *)calloc (graph->n_dots, sizeof (double)); 
    double *der_b_by_y = (double *)calloc (graph->n_dots, sizeof (double));
    double *der_k_by_x = (double *)calloc (graph->n_dots, sizeof (double));
    double *der_k_by_y = (double *)calloc (graph->n_dots, sizeof (double));

    double ac_min_b_sq   = A * C   - B * B;
    double c_d0_min_b_d1 = C * D_0 - B * D_1;
    double a_d1_min_b_d0 = A * D_1 - B * D_0;

    for (int dot_i = 0; dot_i < graph->n_dots; dot_i++)
    {
        double err_sq = y_err[dot_i] * y_err[dot_i];
        
        der_b_by_x[dot_i] = ((2 * D_0 * x[dot_i] - B * y[dot_i] - D_1) * ac_min_b_sq - c_d0_min_b_d1 * 2 * (A * x[dot_i] - B)) / (err_sq * ac_min_b_sq * ac_min_b_sq);

        der_b_by_y[dot_i] = (C - B * x[dot_i]) / (err_sq * ac_min_b_sq);

        der_k_by_x[dot_i] = ((A * y[dot_i] - D_0) * ac_min_b_sq - a_d1_min_b_d0 * 2 * (A * x[dot_i] - B)) / (err_sq * ac_min_b_sq * ac_min_b_sq);

        der_k_by_y[dot_i] = (A * x[dot_i] - B) / (err_sq * ac_min_b_sq);
    }

    struct Chi_Sq *result = (struct Chi_Sq *)calloc (1, sizeof (struct Chi_Sq));

    result->b = c_d0_min_b_d1 / ac_min_b_sq;
    result->k = a_d1_min_b_d0 / ac_min_b_sq;

    for (int dot_i = 0; dot_i < graph->n_dots; dot_i++)
    {        
        double temp_b_x = der_b_by_x[dot_i] * x_err[dot_i];
        double temp_b_y = der_b_by_y[dot_i] * y_err[dot_i];
        double temp_k_x = der_k_by_x[dot_i] * x_err[dot_i];
        double temp_k_y = der_k_by_y[dot_i] * y_err[dot_i];

        result->b_err += temp_b_x * temp_b_x + temp_b_y * temp_b_y;
        result->k_err += temp_k_x * temp_k_x + temp_k_y * temp_k_y;
    }

    free (der_b_by_x);
    free (der_b_by_y);
    free (der_k_by_x);
    free (der_k_by_y);

    result->b_err = sqrt (result->b_err);
    result->k_err = sqrt (result->k_err);

    return result;
}

// =============================================================================================================================================================== //

// ====================================================================== ERROR CALCULATION ====================================================================== //

#if 0
static double Calculate_Error (const struct Forest *forest);
static double Get_Var_Value   (const char *var_name, const struct Var *vars_arr, const int n_vars);

int Error_Calculator (struct Forest *forest, struct Graph *graph)
{    
    MY_ASSERT (forest, "struct Forest *forest", NULL_PTR, ERROR);
    MY_ASSERT (graph,  "struct Graph *graph",   NULL_PTR, ERROR);

    int D_status = Differentiator (graph->function, forest);
    MY_ASSERT (D_status != ERROR, "Differentiator ()", FUNC_ERROR, ERROR);

    int x_var_i = -1;
    for (int var_i = 0; var_i < forest->n_vars; var_i++)
    {
        if (strncmp (graph->x_name, forest->vars_arr[var_i].name, strlen (graph->x_name) + 1) == 0)
            x_var_i = var_i;
    }
    MY_ASSERT (x_var_i != -1, "x_var_i", UNEXP_VAL, ERROR);

    for (int dot_i = 0; dot_i < graph->n_dots; dot_i++)
    {
        forest->vars_arr[x_var_i].value = graph->x_arr[dot_i];
        forest->vars_arr[x_var_i].error = graph->x_err[dot_i];

        graph->y_err[dot_i] = Calculate_Error (forest);
    }
        
    return NO_ERRORS;
}

static double Calculate_Error (const struct Forest *forest)
{
    MY_ASSERT (forest, "const struct Forest *forest", NULL_PTR, (double)ERROR);

    double error_sq = 0.0;

    for (int var_i = 0; var_i < forest->n_vars; var_i++)
    {
        double partial_derivative = Calculate_Tree (forest->tree_arr[var_i], forest->vars_arr, forest->n_vars);

        double inter_num = partial_derivative * forest->vars_arr[var_i].error;
        
        error_sq += inter_num * inter_num;
    }

    return sqrt (error_sq);
}
#endif

// =============================================================================================================================================================== //
