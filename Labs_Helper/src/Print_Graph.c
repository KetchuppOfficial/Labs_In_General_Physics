#include "../include/Labs.h"
#include "My_Lib.h"

static void   Print_Arr (FILE *file_ptr, const char *name, const double arr[], const int acc, const size_t n_dots);
static double Find_Max  (const double *arr, const double n_elems);
static double Find_Min  (const double *arr, const double n_elems);

int Print_Graph (struct Graph *graph)
{ 
    FILE *file = fopen ("Graph.py", "wb");

    fprintf (file, "# ------------------------------------------------------ #\n"
                   "# Do not change this file. It was produced automatically #\n"
                   "# ------------------------------------------------------ #\n\n");

    fprintf (file, "from matplotlib import pyplot as plt\n"
                   "import numpy as np\n"
                   "from matplotlib import style\n\n");
                
    fprintf (file, "plt.figure (figsize = (10, 5.625), dpi = 80)\n\n"

                   "plt.title (\'%s\', fontsize = 16)\n\n", graph->title);

    struct Node *func_tree = Plant_Function_Tree (graph->function);

    struct Var *vars_arr = NULL;
    Get_X_Vals_And_Errors (func_tree, vars_arr, graph);
    Get_Y_Vals_And_Errors (func_tree, vars_arr, graph);

    Print_Arr (file, "x", graph->x_arr, graph->accuracy, graph->n_dots);
    Print_Arr (file, "y", graph->y_arr, graph->accuracy, graph->n_dots);
    fprintf   (file, "\n");

    Print_Arr (file, "x_err", graph->x_err, graph->accuracy, graph->n_dots);
    Print_Arr (file, "y_err", graph->y_err, graph->accuracy, graph->n_dots);
    fprintf   (file, "\n");

    fprintf (file, "plt.errorbar (x, y, xerr = x_err, yerr = y_err, color = \'blue\', ls = \'none\')\n\n");

    fprintf (file, "plt.scatter (x, y, s = 15, label = \'%s\', color = \'%s\')\n\n", 
                    graph->pt_label, graph->pt_colour);

    // Least squares method line
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if (graph->line_type != NO_LINE)
    {
        struct Chi_Sq chi = {};
        chi.coeff_arr = (double *)calloc (graph->approx_pow, sizeof (double));
        chi.coeff_err = (double *)calloc (graph->approx_pow, sizeof (double));

        chi.coeff_arr = Chi_Square (func_tree, graph);

        fprintf (file, "plt.plot (x, ");

        for (int coeff_i = graph->approx_pow; coeff_i > 0; coeff_i--)
            fprintf (file, "%+.*f * x**%u ", graph->accuracy, chi.coeff_arr[coeff_i], coeff_i);
        fprintf (file, "%*f ", graph->accuracy, chi.coeff_arr[0]);
        
        fprintf (file, ", linewidth = 1, color = \'%s\', label = \'Аппроксимирующая кривая ", graph->line_colour);

        fprintf (file, "y = ");
        for (int coeff_i = graph->approx_pow; coeff_i > 0; coeff_i--)
            fprintf (file, "%+.*f * x**%u ", graph->accuracy, chi.coeff_arr[coeff_i], coeff_i);
        fprintf (file, "%*f\'\n", graph->accuracy, chi.coeff_arr[0]);
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    // Axis labels
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    fprintf (file,  "plt.xlabel (\'%s\', fontsize = 16)\n"
                    "plt.xticks (fontsize = 16, ha = 'center', va = 'top')\n\n", graph->x_label);

    fprintf (file,  "plt.ylabel (\'%s\', fontsize = 16)\n"
                    "plt.yticks (fontsize = 16, rotation = 30, ha = 'right', va = 'top')\n\n", graph->y_label);

    fprintf (file,  "plt.grid (color     = \'black\',\n"                                      
                    "          linewidth = 0.45,\n"    
                    "          linestyle = \'dotted\')\n\n"

                    "plt.minorticks_on ()\n\n"

                    "plt.grid (which     = \'minor\',\n"
                    "          color     = \'grey\',\n"
                    "          linewidth = 0.25,\n"
                    "          linestyle = \'dashed\')\n\n"

                    "plt.legend (loc = \'best\')\n");
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                   
    fprintf (file, "plt.savefig (\'%s\')\n", graph->img_name);

    fclose (file);

    return 0;
}

static struct Node *Plant_Function_Tree (const char *function)
{
    MY_ASSERT (function, "const char *function", NULL_PTR, NULL);
    
    int n_tokens = 0;
    struct Token *token_arr = Lexer (function, &n_tokens);
    MY_ASSERT (token_arr, "Lexer ()", FUNC_ERROR, NULL);

    struct Node *root = Parser (token_arr, n_tokens);
    MY_ASSERT (root, "Parser ()", FUNC_ERROR, NULL);

    free (token_arr);

    return root;
}

static int Get_X_Vals_And_Errors (const struct Node *func_tree, struct Var *vars_arr, struct Graph *graph)
{
    MY_ASSERT (func_tree,  "const struct Node *tree", NULL_PTR,     ERROR);
    MY_ASSERT (vars_arr,   "struct Vars *vars_arr",   NOT_NULL_PTR, ERROR);
    MY_ASSERT (graph,      "struct Graph *graph",     NULL_PTR,     ERROR);
    
    vars_arr = Find_Vars (func_tree, &graph->n_dots);
    MY_ASSERT (vars_arr, "Find_Vars ()", FUNC_ERROR, ERROR);

    graph->x_arr = (double *)calloc (graph->n_dots, sizeof (double));
    graph->x_err = (double *)calloc (graph->n_dots, sizeof (double));
    
    for (int var_i = 0; var_i < graph->n_dots; var_i++)
    {
        graph->x_arr[var_i] = vars_arr->val;
        graph->x_err[var_i] = vars_arr->error;
    }

    return NO_ERRORS;
}

static int Get_Y_Vals_And_Errors (const struct Node *func_tree, const struct Var *vars_arr, struct Graph *graph)
{
    MY_ASSERT (func_tree,  "const struct Node *tree",     NULL_PTR, ERROR);
    MY_ASSERT (vars_arr,   "const struct Vars *vars_arr", NULL_PTR, ERROR);
    MY_ASSERT (graph,      "struct Graph *graph",         NULL_PTR, ERROR);
    
    graph->y_arr = (double *)calloc (graph->n_dots, sizeof (double));
    graph->y_err = (double *)calloc (graph->n_dots, sizeof (double));
    
    for (int var_i = 0; var_i < graph->n_dots; var_i++)
    {
        graph->y_arr[var_i] = Calculate_Tree (func_tree, vars_arr, graph->n_dots);
        graph->y_err[var_i] = Error_Calculator (func_tree, vars_arr, graph->n_dots);
    }

    return NO_ERRORS;
}

static void Print_Arr (FILE *file_ptr, const char *name, const double arr[], const int acc, const size_t n_dots)
{
    fprintf (file_ptr, "%s = np.array([", name);
    for (size_t dot_i = 0; dot_i < n_dots - 1; dot_i++)
        fprintf (file_ptr, "%.*f, ", acc, arr[dot_i]);
    fprintf (file_ptr, "%.*f])\n", acc, arr[n_dots - 1]);
}

static double Find_Max (const double *arr, const double n_elems)
{
    double max = arr[0];

    for (int i = 0; i < n_elems; i++)
        max = (max > arr[i]) ? max : arr[i];

    return max;
}

static double Find_Min (const double *arr, const double n_elems)
{
    double min = arr[0];

    for (int i = 0; i < n_elems; i++)
        min = (min < arr[i]) ? min : arr[i];

    return min;
}
