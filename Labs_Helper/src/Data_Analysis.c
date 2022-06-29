#include "../include/Labs.h"
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

    double avg = Labs_Avg (array, n_elems);

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

static double Calculate_Exam      (const double *arr_x, const double *y_err, const size_t n_exams, const size_t row_i, const size_t column_j, const size_t n_rows);
static double Calculate_Free_Elem (const struct Tree *tree, const double *arr_x, const double *y_err, const size_t n_exams, const size_t row_i, const size_t n_rows);

struct Tree
{
    struct Node *node_ptr;
    struct Var *vars_arr;
    int n_vars;
};

double *Chi_Square (const struct Tree *func_tree, struct Graph *graph)
{
    MY_ASSERT (func_tree, "const struct Tree *func_tree", NULL_PTR, NULL);
    MY_ASSERT (graph,     "const double *arr_x",          NULL_PTR, NULL);
    
    const n_rows = graph->approx_pow + 1;
    double *matrix = (double *)calloc (n_rows * (n_rows + 1), sizeof (double));

    for (int row_i = 0; row_i < n_rows; row_i++)
    {
        for (int column_j = 0; column_j < n_rows; column_j++)
            matrix[row_i * (n_rows + 1) + column_j] = Calculate_Exam (graph->x_arr, graph->y_err, graph->n_dots, row_i, column_j, n_rows);

        matrix[row_i * (n_rows + 1) + n_rows] = Calculate_Free_Elem (func_tree, graph->x_arr, graph->y_err, graph->n_dots, row_i, n_rows);
    }

    double *params = (double *)calloc (n_rows, sizeof (double));
    SLE_solver (matrix, n_rows, params);

    free (matrix);

    return params;
}

static double Calculate_Exam (const double *arr_x, const double *y_err, const size_t n_exams, const size_t row_i, const size_t column_j, const size_t n_rows)
{
    double elem = 0.0;

    if (column_j == n_rows - 1)
    {
        for (size_t exam_i = 0; exam_i < n_exams; exam_i++)
            elem += (1 / (y_err[exam_i] * y_err[exam_i]));
    }
    else
    {
        for (size_t exam_i = 0; exam_i < n_exams; exam_i++)
            elem += (pow (arr_x[exam_i], n_rows + row_i - column_j) / (y_err[exam_i] * y_err[exam_i]));
    }
    
    return elem;
}

static double Calculate_Free_Elem (const struct Tree *tree, const double *arr_x, const double *y_err, const size_t n_exams, const size_t row_i, const size_t n_rows)
{
    double elem = 0.0;

    if (row_i == 0)
    {
        for (int exam_i = 0; exam_i < n_exams; exam_i++)
            elem += (Calculate_Tree (tree->node_ptr, tree->vars_arr, tree->n_vars) / (y_err[exam_i] * y_err[exam_i]));
    }   
    else
    {
        for (int exam_i = 0; exam_i < n_exams; exam_i++)
            elem += (Calculate_Tree (tree->node_ptr, tree->vars_arr, tree->n_vars) / (y_err[exam_i] * y_err[exam_i]) * pow (arr_x[exam_i], row_i));
    }
        
}

// =============================================================================================================================================================== //

// ====================================================================== ERROR CALCULATION ====================================================================== //

static struct Var *Find_Vars                         (const struct Node *node_ptr, int *n_vars);
static int         Add_Vars                          (const struct Node* node_ptr, struct Var *vars_arr);
static bool        Find_Name                         (const struct Var *vars_arr, const char *var, const int n_vars);
static int         Get_Values                        (struct Var *vars_arr, const int n_vars);
static double      Get_Double                        (void);
static void        Print_Incorr_Symbs                (void);
static bool        Warn_About_Incorr_Symbs_After_Num (void);
static inline void Clear_Stdin                       (void);
static double      Calculate_Error                   (struct Node *const *forest, struct Var *vars_arr, const int n_vars);

const int MAX_N_VARS = 50;

double Error_Calculator (const struct Node *func_tree, const struct Var *vars_arr, const int n_vars)
{    
    MY_ASSERT (func_tree, "const struct Node *func_tree", NULL_PTR, (double)ERROR);
    MY_ASSERT (vars_arr,  "const struct Var *vars_arr",   NULL_PTR, (double)ERROR);

    struct Node **forest = Differentiator (func_tree, vars_arr, n_vars);

    double error = Calculate_Error (forest, vars_arr, n_vars);

    int FD_status = Forest_Dtor (forest, n_vars);
    MY_ASSERT (FD_status , "Forest_Dtor ()", FUNC_ERROR, ERROR);

    return error;
}

struct Var *Find_Vars (const struct Node *node_ptr, int *n_vars)
{
    MY_ASSERT (node_ptr, "struct Node *node_ptr", NULL_PTR, NULL);
    MY_ASSERT (n_vars,   "int *n_vars",           NULL_PTR, NULL);
    
    struct Var *vars_arr = (struct Var *)calloc (MAX_N_VARS, sizeof (struct Var));
    MY_ASSERT (vars_arr, "struct Var *vars_arr", NE_MEM, NULL);

    *n_vars = Add_Vars (node_ptr, vars_arr);
    MY_ASSERT (*n_vars != ERROR, "Add_Vars ()", FUNC_ERROR, NULL);

    int GV_status = Get_Values (vars_arr, *n_vars);
    MY_ASSERT (GV_status != ERROR, "Get_Values ()", FUNC_ERROR, NULL);

    return vars_arr;
}

static int Add_Vars (const struct Node* node_ptr, struct Var *vars_arr)
{
    MY_ASSERT (node_ptr, "struct Node *node_ptr", NULL_PTR, ERROR);
    MY_ASSERT (vars_arr, "struct Var *vars_arr",  NULL_PTR, ERROR);
    
    static int var_i = 0;
    
    if (node_ptr->type == VARIABLE && Find_Name (vars_arr, node_ptr->value.str, var_i) == false)
    {
        size_t str_len = strlen (node_ptr->value.str);
        vars_arr[var_i].name = (char *)calloc (str_len, sizeof (char));
        MY_ASSERT (vars_arr[var_i].name, "vars_arr[var_i].name", NE_MEM, ERROR);

        memcpy (vars_arr[var_i].name, node_ptr->value.str, str_len);
        var_i++;
    }

    if (node_ptr->left_son)
        Add_Vars (node_ptr->left_son, vars_arr);

    if (node_ptr->right_son)
        Add_Vars (node_ptr->right_son, vars_arr);

    return var_i;
}

static bool Find_Name (const struct Var *vars_arr, const char *var, const int n_vars)
{   
    for (int var_i = 0; var_i < n_vars; var_i++)
        if (strcmp (vars_arr[var_i].name, var) == 0)
            return true;

    return false;
}

static int Get_Values (struct Var *vars_arr, const int n_vars)
{
    MY_ASSERT (vars_arr,   "struct Var *vars_arr", NULL_PTR, ERROR);
    MY_ASSERT (n_vars > 0, "const int n_vars",     POS_VAL,  ERROR);
    
    for (int var_i = 0; var_i < n_vars; var_i++)
    {       
        printf ("\"%s\" value: ", vars_arr[var_i].name);
        vars_arr[var_i].val = Get_Double ();

        printf ("\"%s\" error: ", vars_arr[var_i].name);
        vars_arr[var_i].error = Get_Double ();
    }

    return NO_ERRORS;
}

static double Get_Double (void)
{
    bool error = true;

    double num = 0.0;

    while (error)
    {
        if (scanf ("%lf", &num) != 1)
        {
            Print_Incorr_Symbs ();
            printf (" is not a number. Try again\n");
        }
        else
            error = Warn_About_Incorr_Symbs_After_Num ();
    }

    return num;
}

static void Print_Incorr_Symbs (void)
{
    int incorr_symb = 0;

    printf ("\"");
    while ((incorr_symb = getchar ()) != '\n')
        putchar (incorr_symb);
    printf ("\"");
}

static bool Warn_About_Incorr_Symbs_After_Num (void)
{
    bool error = true;

    if (getchar() == '\n')
        error = false;
    else
    {
        Clear_Stdin ();     
        printf ("You have written a number and some inappropriate symbols after that. Try again\n");
    }

    return error;
}

static inline void Clear_Stdin (void)
{
    while (getchar () != '\n')
        ;
}

static double Calculate_Error (struct Node *const *forest, struct Var *vars_arr, const int n_vars)
{
    MY_ASSERT (forest,     "const struct Node **forest", NULL_PTR, (double)ERROR);
    MY_ASSERT (vars_arr,   "struct Var *vars_arr",       NULL_PTR, (double)ERROR);
    MY_ASSERT (n_vars > 0, "const int n_vars",           POS_VAL,  (double)ERROR);

    double error_sq = 0.0;

    for (int var_i = 0; var_i < n_vars; var_i++)
    {
        double partial_derivative = Calculate_Tree (forest[var_i], vars_arr, n_vars);

        double inter_num = partial_derivative * vars_arr[var_i].error;
        
        error_sq += inter_num * inter_num;
    }

    return sqrt (error_sq);
}

// =============================================================================================================================================================== //
