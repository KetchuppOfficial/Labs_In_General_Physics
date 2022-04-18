#include "Labs.h"

static void Print_Arr (FILE *file_ptr, const char *name, const double arr[], const int acc, const size_t n_dots)
{
    fprintf (file_ptr, "%s = [", name);
    for (size_t dot_i = 0; dot_i < n_dots - 1; dot_i++)
        fprintf (file_ptr, "%.*f, ", acc, arr[dot_i]);
    fprintf (file_ptr, "%.*f]\n", acc, arr[n_dots - 1]);
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

int Print_Graph (struct Graph *graph)
{ 
    FILE *file = fopen ("Graph.py", "wb");

    fprintf (file, "# ------------------------------------------------------ #\n"
                   "# Do not change this file. It was produced automatically #\n"
                   "# ------------------------------------------------------ #\n\n");

    fprintf (file, "from matplotlib import pyplot as plt\n\n"

                    "from matplotlib import style\n"
                    "style.use(\'ggplot\')\n");

    Print_Arr (file, "x", graph->x_arr, graph->x_acc, graph->n_dots);
    Print_Arr (file, "y", graph->y_arr, graph->y_acc, graph->n_dots);
    fprintf (file, "\n");

    fprintf (file, "plt.scatter(x, y, s = 15, label = \'%s\', color = \'%s\')\n\n", graph->pt_label, graph->pt_colour);

    struct Least_Sq mnk = Labs_Least_Squares (graph->x_arr, graph->y_arr, graph->n_dots);

    double x_min = Find_Min (graph->x_arr, graph->n_dots);
    double x_max = Find_Max (graph->x_arr, graph->n_dots);

    double y_x_min = mnk.k_coeff * x_min + mnk.b_coeff;
    double y_x_max = mnk.k_coeff * x_max + mnk.b_coeff;

    fprintf (file, "line_x = [%.*f, %.*f]\n", graph->x_acc, x_min, graph->x_acc, x_max);
    fprintf (file, "line_y = [%.*f, %.*f]\n\n", graph->y_acc, y_x_min, graph->y_acc, y_x_max);

    fprintf (file, "plt.plot(line_x, line_y, linewidth = 1, color = \'%s\', label=\'%s\')\n\n", graph->line_colour, graph->line_label);

    fprintf (file, "plt.legend()\n"
                   "plt.title(\'%s\')\n"
                   "plt.xlabel(\'%s\')\n"
                   "plt.ylabel(\'%s\')\n\n", graph->title, graph->x_label, graph->y_label);
                   
    fprintf (file, "plt.savefig(\'%s\')\n", graph->img_name);

    fclose (file);

    return 0;
}

