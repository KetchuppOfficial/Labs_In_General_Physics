#include "Labs.h"

double Labs_Avg (const double *array, const size_t n_elems)
{    
    double sum = 0.0;
    
    for (size_t elem_i = 0; elem_i < n_elems; elem_i++)
        sum += array[elem_i];

    return (sum / (double)n_elems);
}

double Labs_Root_Mean_Square (const double *array, const size_t n_elems)
{    
    double sq_sum = 0.0;
    
    for (size_t elem_i = 0; elem_i < n_elems; elem_i++)
        sq_sum += array[elem_i] * array[elem_i];

    return sqrt(sq_sum / (double)n_elems);
}

double Labs_Avg_Err_Rand (const double *array, const size_t n_elems)
{
    assert (n_elems > 1);

    double avg = Labs_Avg (array, n_elems);

    double sum = 0.0;
    for (size_t elem_i = 0; elem_i < n_elems; elem_i++)
        sum += pow (array[elem_i] - avg, 2.0);

    double err_sq = sum / (n_elems * (n_elems - 1));
    
    return sqrt (err_sq);
}

struct Least_Sq Labs_Least_Squares (const double *arr_x, const double *arr_y, const size_t n_elems)
{
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

    struct Least_Sq mnk = {};
    
    mnk.k_coeff = (x_sum * y_sum  - n_elems * xy_sum) / (x_sum * x_sum - n_elems * x_sq_sum);
    mnk.b_coeff = (x_sum * xy_sum - x_sq_sum * y_sum) / (x_sum * x_sum - n_elems * x_sq_sum);

    double x_avg = x_sum / n_elems;
    double up_sum   = 0.0;  // sum [(y - <y>)^2]
    double down_sum = 0.0;  // sum [(x - <x>)^2]

    for (size_t elem_i = 0; elem_i < n_elems; elem_i++)
    {
        up_sum   += pow ( (arr_y[elem_i] - (mnk.k_coeff * arr_x[elem_i] + mnk.b_coeff)), 2 );

        down_sum += pow ( (arr_x[elem_i] - x_avg), 2 );
    }
    
    mnk.k_err = sqrt (up_sum / (down_sum * (n_elems - 2) ) );
    mnk.b_err = sqrt ( (up_sum * x_sq_sum) / (n_elems * (n_elems - 2) * down_sum) );

    return mnk;
}