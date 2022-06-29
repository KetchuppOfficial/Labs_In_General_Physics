#include "Differentiator.h"
#include "Gauss_Method.h"

enum Lines
{
    POLINOMICAL,
    NO_LINE
};

struct Least_Sq
{
    double k_coeff;
    double k_err;
    double b_coeff;
    double b_err;
};

struct Chi_Sq
{
    double *coeff_arr;
    double *coeff_err;
};

struct Graph
{
    const char   *title;
    const char   *function;

    const size_t  n_dots;
    const char   *pt_label;
    const char   *pt_colour;

    enum Lines    line_type;
    const char   *line_colour;

    const size_t  approx_pow;
    const size_t  accuracy;

    double       *x_arr;
    double       *x_err;
    const char   *x_label;

    double       *y_arr;
    double       *y_err;
    const char   *y_label;

    const char   *img_name;
};

double      Avg              (const double *array, const size_t n_elems);
double      Root_Mean_Square (const double *array, const size_t n_elems);
double      Avg_Err_Rand     (const double *array, const size_t n_elems);
int         Least_Squares    (const double *arr_x, const double *arr_y, const size_t n_elems, struct Least_Sq *least_sq);
double     *Chi_Square       (const struct Tree *func_tree, struct Graph *graph);
double      Error_Calculator (const struct Node *func_tree, const struct Var *vars_arr, const int n_vars);
struct Var *Find_Vars        (const struct Node *node_ptr, int *n_vars);
int         Print_Graph      (struct Graph *graph);

int  SLE_solver  (double *matrix, const size_t n_rows, double *solution);
void Matrix_Dump (double *matrix, const size_t n_rows);
