#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

struct Least_Sq // y = kx + b
{
    double k_coeff;
    double k_err;
    double b_coeff;
    double b_err;
};

struct Graph
{
    const char *title;

    const size_t n_dots;
    const char *pt_label;
    const char *pt_colour;

    const char   *line_label;
    const char   *line_colour;

    const double *x_arr;
    const int    x_acc;
    const char   *x_label;

    const double *y_arr;
    const int    y_acc;
    const char   *y_label;

    const char *img_name;
};

double Labs_Avg (const double *array, const size_t n_elems);
double Labs_Root_Mean_Square (const double *array, const size_t n_elems);
double Labs_Avg_Err_Rand (const double *array, const size_t n_elems);
struct Least_Sq Labs_Least_Squares (const double *arr_x, const double *arr_y, const size_t n_elems);
int    Print_Graph (struct Graph *graph);
