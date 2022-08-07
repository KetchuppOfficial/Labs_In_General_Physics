#ifndef DATA_ANALYSIS_H_INCLUDED
#define DATA_ANALYSIS_H_INCLUDED

struct Chi_Sq
{
    double k;
    double b;
    double k_err;
    double b_err;
};

struct Chi_Sq Linear_Chi_Square (const double *x_data, const double *x_err,
                                 const double *y_data, const double *y_err,
                                 const int n_dots);

double Avg              (const double *array, const size_t n_elems);
double Root_Mean_Square (const double *array, const size_t n_elems);
double Avg_Err_Rand     (const double *array, const size_t n_elems);

#endif // DATA_ANALYSIS_H_INCLUDED
