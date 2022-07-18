#include "../include/LabaCL.h"
#include "My_Lib.h"

static inline void Print_Arr     (FILE *file_ptr, const char *name, const double *arr, const size_t n_dots);
static void        Print_Formula (FILE *file, const struct Chi_Sq *chi);
static void        Print_Line    (FILE *file, struct Graph *graph);

int Print_Graph (struct Graph *graph)
{ 
    FILE *file = Open_File ("Graph.py", "wb");

    fprintf (file, "# ------------------------------------------------------ #\n"
                   "# Do not change this file. It was produced automatically #\n"
                   "# ------------------------------------------------------ #\n\n");

    fprintf (file, "from matplotlib import pyplot as plt\n"
                   "import numpy as np\n"
                   "from matplotlib import style\n\n");
                
    fprintf (file, "plt.figure (figsize = (10, 5.625), dpi = 80)\n\n"

                   "plt.title (\'%s\', fontsize = 16)\n\n", graph->title);

    Print_Arr (file, "x", graph->x_arr, graph->n_dots);
    Print_Arr (file, "y", graph->y_arr, graph->n_dots);
    fprintf   (file, "\n");

    Print_Arr (file, "x_err", graph->x_err, graph->n_dots);
    Print_Arr (file, "y_err", graph->y_err, graph->n_dots);
    fprintf   (file, "\n");

    fprintf (file, "plt.errorbar (x, y, xerr = x_err, yerr = y_err, color = \'%s\', ls = \'none\')\n\n",
                    graph->err_colour);

    fprintf (file, "plt.scatter (x, y, s = 15, label = \'%s\', color = \'%s\')\n\n", 
                    graph->dot_label, graph->dot_colour);

    if (graph->line_type == POLINOMICAL)
        Print_Line (file, graph);

    fprintf (file,  "plt.xlabel (\'%s\', fontsize = 16)\n"
                    "plt.xticks (fontsize = 16, ha = 'center', va = 'top')\n\n", graph->x_title);

    fprintf (file,  "plt.ylabel (\'%s\', fontsize = 16)\n"
                    "plt.yticks (fontsize = 16, rotation = 30, ha = 'right', va = 'top')\n\n", graph->y_title);

    fprintf (file,  "plt.grid (color     = \'black\',\n"                                      
                    "          linewidth = 0.45,\n"    
                    "          linestyle = \'dotted\')\n\n"

                    "plt.minorticks_on ()\n\n"

                    "plt.grid (which     = \'minor\',\n"
                    "          color     = \'grey\',\n"
                    "          linewidth = 0.25,\n"
                    "          linestyle = \'dashed\')\n\n"

                    "plt.legend (loc = \'best\')\n");
                   
    fprintf (file, "plt.savefig (\'%s\')\n", graph->img_name);

    Close_File (file, "Graph.py");

    system ("echo Printing graph...");
    system ("python3 Graph.py");

    return 0;
}

static inline void Print_Arr (FILE *file, const char *name, const double *arr, const size_t n_dots)
{
    fprintf (file, "%s = np.array([", name);

    for (size_t dot_i = 0; dot_i < n_dots - 1; dot_i++)
        fprintf (file, "%.6f, ", arr[dot_i]);

    fprintf (file, "%.6f])\n", arr[n_dots - 1]);
}

static void Print_Line (FILE *file, struct Graph *graph)
{
    struct Chi_Sq *chi = Linear_Chi_Square (graph);

    printf ("*********** Chi-square coefficients ***********\n");
    printf ("Approximation line: y = kx + b\n");
    printf ("k = %f +- %f;\n"
            "b = %f +- %f\n", chi->k, fabs (chi->k_err), chi->b, fabs(chi->b_err));
    printf ("***********************************************\n");

    fprintf (file, "plt.plot (x, ");
    Print_Formula (file, chi);
    
    fprintf (file, ", linewidth = 1, color = \'%s\', label = \'Аппроксимирующая прямая ", graph->line_colour);

    fprintf (file, "y = ");
    Print_Formula (file, chi);

    fprintf (file, "\')\n");

    free (chi);
}

static void Print_Formula (FILE *file, const struct Chi_Sq *chi)
{
    if (chi->k < 0)
        fprintf (file, "-");

    fprintf (file, "%f * x", fabs (chi->k));

    if (chi->b < 0)
        fprintf (file, " - ");
    else
        fprintf (file, " + ");

    fprintf (file, "%f", fabs (chi->b));
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

#ifdef GRAPH_DUMP
int Graph_Dump (const struct Graph *graph)
{
    MY_ASSERT (graph, "const struct Graph *graph", NULL_PTR, ERROR);
    
    system ("mkdir -p graph_dump");
    
    FILE *dump_file = Open_File ("graph_dump/dump.txt", "wb");
    MY_ASSERT (dump_file, "dump_file", OPEN_ERR, ERROR);

    fprintf (dump_file, "Graph_Title:\n\t%s\n\n", graph->title);

    fprintf (dump_file, "N_Dots:\n\t%d\n\n", graph->n_dots);
    fprintf (dump_file, "Dot_Label:\n\t%s\n\n", graph->dot_label);
    fprintf (dump_file, "Dot_Colour:\n\t%s\n\n", graph->dot_colour);

    switch (graph->line_type)
    {
        case DEFAULT:
            fprintf (dump_file, "Line_Type:\n\tDEFAULT\n\n");
            break;
        case POLINOMICAL:
            fprintf (dump_file, "Line_Type:\n\tPOLINOMICAL\n\n");
            break;
        case DOTS:
            fprintf (dump_file, "Line_Type:\n\tDOTS\n\n");
            break;

        default:
            MY_ASSERT (false, "graph->line_type", UNEXP_VAL, ERROR);
    }
    fprintf (dump_file, "Line_Colour:\n\t%s\n\n", graph->line_colour);

    #ifdef APPROX_POW
    fprintf (dump_file, "Approximation_Power:\n\t%d\n\n", graph->approx_pow);
    #endif

    fprintf (dump_file, "X_Asix_Data:\n");
    for (int dot_i = 0; dot_i < graph->n_dots; dot_i++)
        fprintf (dump_file, "\t%f\n", graph->x_arr[dot_i]);
    fprintf (dump_file, "\n");

    fprintf (dump_file, "X_Error:\n");
    for (int dot_i = 0; dot_i < graph->n_dots; dot_i++)
        fprintf (dump_file, "\t%f\n", graph->x_err[dot_i]);
    fprintf (dump_file, "\n");

    fprintf (dump_file, "X_Title:\n\t%s\n\n", graph->x_title);

    fprintf (dump_file, "Y_Asix_Data:\n");
    for (int dot_i = 0; dot_i < graph->n_dots; dot_i++)
        fprintf (dump_file, "\t%f\n", graph->y_arr[dot_i]);
    fprintf (dump_file, "\n");

    fprintf (dump_file, "Y_Error:\n");
    for (int dot_i = 0; dot_i < graph->n_dots; dot_i++)
        fprintf (dump_file, "\t%f\n", graph->y_err[dot_i]);
    fprintf (dump_file, "\n");

    fprintf (dump_file, "Y_Title:\n\t%s\n\n", graph->y_title);

    fprintf (dump_file, "Image_Name:\n\t%s\n", graph->img_name);

    Close_File (dump_file, "graph_dump/dump.txt");

    return NO_ERRORS;
}
#endif
