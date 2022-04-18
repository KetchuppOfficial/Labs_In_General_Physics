#include <stdio.h>
#include "Labs.h"

int main (void)
{
    #if 1
    double data_x[] = {3.41, 3.30, 3.19, 3.096, 3.00};
    double data_y[] = {1.18, 1.11, 1.03, 0.885, 0.96};

    struct Graph graph = 
    {
        "k(1/T)",

        sizeof data_x / sizeof (double),
        "Эксперименатальные точки",
        "green",

        "Аппроксимирующая прямая",
        "red",

        data_x,
        3,
        "1/T, 10^3/K",

        data_y,
        3,
        "k, К/атм",

        "../../../Общая физика/Семестр 2/Лабы/2.1.6. Эффект Джоуля-Томсона/Van_der_Waals_2.png"
    };
    
    Print_Graph (&graph);

    struct Least_Sq mnk = Labs_Least_Squares (data_x, data_y, sizeof data_x / sizeof (double));

    printf ("LEAST SQUARE METHOD:\n");

    printf ("k = %g +- %g\n", mnk.k_coeff, mnk.k_err);
    printf ("b = %g +- %g\n\n", mnk.b_coeff, mnk.b_err);
    #endif
#if 0
    double arr[] = {0.05, 0.069011};
    double root_mean_sq = Labs_Root_Mean_Square (arr, 2);
    printf ("root_mean_sq = %f\n", root_mean_sq);
#endif
    return 0;
}