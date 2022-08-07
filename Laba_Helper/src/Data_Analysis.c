#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "Data_Analysis.h"
#include "Tools.h"

struct Chi_Sq Linear_Chi_Square (const double *x_data, const double *x_err,
                                 const double *y_data, const double *y_err,
                                 const int n_dots)
{
    assert (x_data);
    assert (x_err);
    assert (y_data);
    assert (y_err);

    double A = 0.0, B = 0.0, C = 0.0, D_0 = 0.0, D_1 = 0.0;

    for (int dot_i = 0; dot_i < n_dots; dot_i++)
    {
        double err_sq = y_err[dot_i] * y_err[dot_i];
        
        A   += 1 / err_sq;
        B   += x_data[dot_i] / err_sq;
        C   += (x_data[dot_i] * x_data[dot_i]) / err_sq;
        D_0 += y_data[dot_i] / err_sq;
        D_1 += (x_data[dot_i] * y_data[dot_i]) / err_sq;  
    }

    double *der_b_by_x = (double *)Calloc_ (n_dots, sizeof (double));
    double *der_b_by_y = (double *)Calloc_ (n_dots, sizeof (double));
    double *der_k_by_x = (double *)Calloc_ (n_dots, sizeof (double));
    double *der_k_by_y = (double *)Calloc_ (n_dots, sizeof (double));

    double ac_min_b_sq   = A * C   - B * B;
    double c_d0_min_b_d1 = C * D_0 - B * D_1;
    double a_d1_min_b_d0 = A * D_1 - B * D_0;

    for (int dot_i = 0; dot_i < n_dots; dot_i++)
    {
        double err_sq = y_err[dot_i] * y_err[dot_i];
        
        der_b_by_x[dot_i] = ((2 * D_0 * x_data[dot_i] - B * y_data[dot_i] - D_1) * ac_min_b_sq - c_d0_min_b_d1 * 2 * (A * x_data[dot_i] - B)) / (err_sq * ac_min_b_sq * ac_min_b_sq);

        der_b_by_y[dot_i] = (C - B * x_data[dot_i]) / (err_sq * ac_min_b_sq);

        der_k_by_x[dot_i] = ((A * y_data[dot_i] - D_0) * ac_min_b_sq - a_d1_min_b_d0 * 2 * (A * x_data[dot_i] - B)) / (err_sq * ac_min_b_sq * ac_min_b_sq);

        der_k_by_y[dot_i] = (A * x_data[dot_i] - B) / (err_sq * ac_min_b_sq);
    }

    struct Chi_Sq result = {};

    result.b = c_d0_min_b_d1 / ac_min_b_sq;
    result.k = a_d1_min_b_d0 / ac_min_b_sq;

    for (int dot_i = 0; dot_i < n_dots; dot_i++)
    {        
        double temp_b_x = der_b_by_x[dot_i] * x_err[dot_i];
        double temp_b_y = der_b_by_y[dot_i] * y_err[dot_i];
        double temp_k_x = der_k_by_x[dot_i] * x_err[dot_i];
        double temp_k_y = der_k_by_y[dot_i] * y_err[dot_i];

        result.b_err += temp_b_x * temp_b_x + temp_b_y * temp_b_y;
        result.k_err += temp_k_x * temp_k_x + temp_k_y * temp_k_y;
    }

    free (der_b_by_x);
    free (der_b_by_y);
    free (der_k_by_x);
    free (der_k_by_y);

    result.b_err = sqrt (result.b_err);
    result.k_err = sqrt (result.k_err);

    return result;
}

double Avg (const double *array, const size_t n_elems)
{    
    assert (array);
    
    double sum = 0.0;
    
    for (size_t elem_i = 0; elem_i < n_elems; elem_i++)
        sum += array[elem_i];

    return (sum / (double)n_elems);
}

double Root_Mean_Square (const double *array, const size_t n_elems)
{    
    assert (array);
    
    double sq_sum = 0.0;
    
    for (size_t elem_i = 0; elem_i < n_elems; elem_i++)
        sq_sum += array[elem_i] * array[elem_i];

    return sqrt(sq_sum / (double)n_elems);
}

double Avg_Err_Rand (const double *array, const size_t n_elems)
{
    assert (array);

    double avg = Avg (array, n_elems);

    double sum = 0.0;
    for (size_t elem_i = 0; elem_i < n_elems; elem_i++)
        sum += pow (array[elem_i] - avg, 2.0);

    double err_sq = sum / (n_elems * (n_elems - 1));
    
    return sqrt (err_sq);
}

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
