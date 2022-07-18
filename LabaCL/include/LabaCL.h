#ifndef LABACL_H_INCLUDED
#define LABACL_H_INCLUDED

#include "Differentiator.h"
#include <inttypes.h>
#include <stdbool.h>

enum Lines
{
    DEFAULT = -1,
    POLINOMICAL,
    DOTS
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
    double k;
    double b;
    double k_err;
    double b_err;
};

struct Graph
{
    char        *title;

    int          n_dots;
    char        *dot_label;
    char        *dot_colour;

    enum Lines   line_type;
    char        *line_colour;

    #ifdef POLINOMICAL_APPROX
    int          approx_pow;
    #endif // POLINOMICAL_APPROX

    double      *x_arr;
    double      *x_err;
    char        *x_title;

    double      *y_arr;
    double      *y_err;
    char        *y_title;

    char        *err_colour;

    char        *img_name;
};

int User_Interface (void);

// =========================== BUILDING_GRAPH =========================== //

struct Graph *Graph_Compiler (char *buffer, const long n_symbs);
void          Graph_Dtor     (struct Graph *graph);
int           Print_Graph    (struct Graph *graph);
#ifdef GRAPH_DUMP
int Graph_Dump (const struct Graph *graph);
#endif // GRAPH_DUMP

// ====================================================================== //

double  Avg              (const double *array, const size_t n_elems);
double  Root_Mean_Square (const double *array, const size_t n_elems);
double  Avg_Err_Rand     (const double *array, const size_t n_elems);
int     Least_Squares    (const double *arr_x, const double *arr_y, const size_t n_elems, struct Least_Sq *least_sq);
#
double *Chi_Square       (const struct Node *func_tree, const struct Var *vars_arr, const int n_vars, struct Graph *graph);
struct Chi_Sq *Linear_Chi_Square (const struct Graph *graph);
int     Error_Calculator (struct Forest *forest, struct Graph *graph);

int SLE_solver  (double *matrix, const size_t n_rows, double *solution);
#ifdef MATRIX_DUMP
void Matrix_Dump (double *matrix, const size_t n_rows);
#endif // MATRIX_DUMP

#endif // LABACL_H_INCLUDED
