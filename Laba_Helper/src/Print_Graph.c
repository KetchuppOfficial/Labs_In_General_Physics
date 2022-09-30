#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "Graph.h"
#include "Data_Analysis.h"
#include "Tools.h"

static inline void Print_Arr (FILE *file, const char *name, const double *arr, const int n_dots);
static void Print_Formula (FILE *file, const double k_coeff, const double b_coeff);
static void Print_Line (FILE *file, const char *line_colour, const double *x_data, const double *x_err,
                        const double *y_data, const double *y_err, const int n_dots);

int Print_Graph (const struct Graph *graph)
{ 
    assert (graph);

    const char   *title       = Graph_Title       (graph);
    const char   *img_name    = Graph_Image_Name  (graph);
    const char   *dot_label   = Graph_Dot_Label   (graph);
    const char   *dot_colour  = Graph_Dot_Colour  (graph);
    const char   *line_colour = Graph_Line_Colour (graph);
    const int     line_type   = Graph_Line_Type   (graph);
    const char   *err_colour  = Graph_Err_Colour  (graph);
    const double *x_data      = Graph_X_Data      (graph);
    const double *x_err       = Graph_X_Err       (graph);
    const char   *x_title     = Graph_X_Title     (graph);
    const double *y_data      = Graph_Y_Data      (graph);
    const double *y_err       = Graph_Y_Err       (graph);
    const char   *y_title     = Graph_Y_Title     (graph);
    const int     n_dots      = Graph_N_Dots      (graph);
    
    FILE *file = fopen ("graph.py", "wb");
    if (file == NULL)
    {
        printf ("Opening \"graph.py\" failed\n");
        return error;
    }

    fprintf (file, "# ------------------------------------------------------ #\n"
                   "# Do not change this file. It was produced automatically #\n"
                   "# ------------------------------------------------------ #\n\n");

    fprintf (file, "from matplotlib import pyplot as plt\n"
                   "import numpy as np\n"
                   "from matplotlib import style\n\n");
                
    fprintf (file, "plt.figure (figsize = (16, 9), dpi = 80)\n\n"

                   "plt.title (\'%s\', fontsize = 24)\n\n", title);

    Print_Arr (file, "x", x_data, n_dots);
    Print_Arr (file, "y", y_data, n_dots);
    fprintf   (file, "\n");

    Print_Arr (file, "x_err", x_err, n_dots);
    Print_Arr (file, "y_err", y_err, n_dots);
    fprintf   (file, "\n");

    fprintf (file, "plt.errorbar (x, y, xerr = x_err, yerr = y_err, color = \'%s\', ls = \'none\')\n\n",
                    err_colour);

    fprintf (file, "plt.scatter (x, y, s = 15, label = \'%s\', color = \'%s\')\n\n", 
                    dot_label, dot_colour);

    if (line_type == POLINOMICAL)
        Print_Line (file, line_colour, x_data, x_err, y_data, y_err, n_dots);

    fprintf (file,  "plt.xlabel (\'%s\', fontsize = 24)\n"
                    "plt.xticks (fontsize = 20, ha = 'center', va = 'top')\n\n", x_title);

    fprintf (file,  "plt.ylabel (\'%s\', fontsize = 24)\n"
                    "plt.yticks (fontsize = 20, rotation = 30, ha = 'right', va = 'top')\n\n", y_title);

    fprintf (file,  "plt.grid (color     = \'black\',\n"                                      
                    "          linewidth = 0.45,\n"    
                    "          linestyle = \'dotted\')\n\n"

                    "plt.minorticks_on ()\n\n"

                    "plt.grid (which     = \'minor\',\n"
                    "          color     = \'grey\',\n"
                    "          linewidth = 0.25,\n"
                    "          linestyle = \'dashed\')\n\n"

                    "plt.legend (loc = \'best\', fontsize = 16)\n");
                   
    fprintf (file, "plt.savefig (\'%s\')\n", img_name);

    fclose (file);

    system ("echo Printing graph...");
    system ("python3 graph.py");

    return success;
}

static inline void Print_Arr (FILE *file, const char *name, const double *arr, const int n_dots)
{
    fprintf (file, "%s = np.array([", name);

    for (int dot_i = 0; dot_i < n_dots - 1; dot_i++)
        fprintf (file, "%.6f, ", arr[dot_i]);

    fprintf (file, "%.6f])\n", arr[n_dots - 1]);
}

void Print_Line (FILE *file, const char *line_colour, const double *x_data, const double *x_err,
                 const double *y_data, const double *y_err, const int n_dots)
{
    assert (file);
    assert (line_colour);
    assert (x_data);
    assert (x_err);
    assert (y_data);
    assert (y_err);

    struct Chi_Sq chi = Linear_Chi_Square (x_data, x_err, y_data, y_err, n_dots);

    printf ("\n*********** Chi-square coefficients ***********\n");
    printf ("Approximation line: y = kx + b\n");
    printf ("k = %f +- %f;\n"
            "b = %f +- %f\n", chi.k, fabs (chi.k_err), chi.b, fabs(chi.b_err));
    printf ("***********************************************\n\n");

    fprintf (file, "plt.plot (x, ");
    Print_Formula (file, chi.k, chi.b);
    
    fprintf (file, ", linewidth = 1, color = \'%s\', label = \'Аппроксимирующая прямая ", line_colour);

    fprintf (file, "y = ");
    Print_Formula (file, chi.k, chi.b);

    fprintf (file, "\')\n");
}

static void Print_Formula (FILE *file, const double k_coeff, const double b_coeff)
{
    if (k_coeff < 0)
        fprintf (file, "-");

    fprintf (file, "%f * x", fabs (k_coeff));

    if (b_coeff < 0)
        fprintf (file, " - ");
    else
        fprintf (file, " + ");

    fprintf (file, "%f", fabs (b_coeff));
}

#ifdef POLINOMICAL_APPROX
static void Print_Formula (FILE *file, const struct Graph *graph, const struct Chi_Sq *chi)
{
    if (graph->approx_pow > 2)
    {
        if (chi->coeff_arr[graph->approx_pow] < 0)
            fprintf (file, "-");

        fprintf (file, "%.*f * x**%u", graph->y_acc, chi->coeff_arr[graph->approx_pow], graph->approx_pow);

        for (int coeff_i = graph->approx_pow - 1; coeff_i > 1; coeff_i--)
        {
            if (chi->coeff_arr[coeff_i] > 0)
                fprintf (file, " + ");
            else
                fprintf (file, " - ");
            
            fprintf (file, "%.*f * x**%u", graph->y_acc, chi->coeff_arr[coeff_i], coeff_i);
        }

        if (chi->coeff_arr[1] > 0)
            fprintf (file, " + ");
        else
            fprintf (file, " - ");
    }

    fprintf (file, "%.*f * x", graph->y_acc, chi->coeff_arr[1]);

    if (chi->coeff_arr[0] > 0)
        fprintf (file, " + ");
    else
        fprintf (file, " - ");

    fprintf (file, "%*f ", graph->y_acc, chi->coeff_arr[0]);
}

static void Print_Line (FILE *file, struct Graph *graph, struct Forest *forest)
{
    struct Chi_Sq chi = {};

    chi.coeff_arr = Chi_Square (graph->function, forest->vars_arr, forest->n_vars, graph);

    fprintf (file, "plt.plot (x, ");
    Print_Formula (file, graph, &chi);
    
    fprintf (file, ", linewidth = 1, color = \'%s\', label = \'Аппроксимирующая ", graph->line_colour);
    if (graph->approx_pow == 1)
        fprintf (file, "прямая ");
    else
        fprintf (file, "кривая ");

    fprintf (file, "y = ");
    Print_Formula (file, graph, &chi);

    fprintf (file, "\')\n");

    free (chi.coeff_arr);
}
#endif

#ifdef FOR_FUTURE
static int         Get_Non_X_Values        (struct Forest *forest, const struct Graph *graph);
static double      Get_Double              (void);
static inline void Clear_Stdin             (void);
static bool        Warn_About_Incorr_Symbs (void);

static int Get_Non_X_Values (struct Forest *forest, const struct Graph *graph)
{
    MY_ASSERT (forest, "struct Forest *forest",     NULL_PTR, ERROR);
    MY_ASSERT (graph,  "const struct Graph *graph", NULL_PTR, ERROR);

    bool x_found = false;
    
    for (int var_i = 0; var_i < forest->n_vars; var_i++)
    {       
        if (strncmp (graph->x_name, forest->vars_arr[var_i].name, strlen (graph->x_name) + 1) != 0)
        {
            printf ("\"%s\" value: ", forest->vars_arr[var_i].name);
            forest->vars_arr[var_i].value = Get_Double ();

            printf ("\"%s\" error: ", forest->vars_arr[var_i].name);
            forest->vars_arr[var_i].error = Get_Double ();
        }
        else
            x_found = true;
    }

    if (x_found == false)
    {
        printf ("************ ERROR REPORT ************\n");
        printf ("Variable \"%s\" does not match to any variable from \"Function\" label", graph->x_name);
        printf ("**************************************\n");

        return ERROR;
    }

    return NO_ERRORS;
}

static double Get_Double (void)
{
    double num = 0.0;
    bool error = true;

    while (error)
    {
        if (scanf ("%lf", &num) != 1)
        {
            int incorr_symb = 0;

            while ((incorr_symb = getchar ()) != '\n')
                putchar (incorr_symb);

            printf (" is not a number. Try again\n");
        }
        else
            error = Warn_About_Incorr_Symbs ();
    }

    return num;
}

static bool Warn_About_Incorr_Symbs (void)
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
    {
        ;
    }
}
#endif
