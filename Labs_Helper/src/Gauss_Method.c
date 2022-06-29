#include "../include/Labs.h"
#include "My_Lib.h"

const int DEGENERATE = INT_MAX;
const double EPSILON = 1E-6;

static void Direct_Passing     (double *matrix, const size_t n_rows);
static int  Check_Degeneration (const double *matrix, const size_t n_rows);
static void Reverse_Passing    (double *matrix, const size_t n_rows);

static int Compare_Double (const double first, const double second);

static inline void Mult_Row           (double *matrix, const size_t n_rows, const size_t row_i,    const double mult);
static inline void Mult_Then_Add_Rows (double *matrix, const size_t n_rows, const size_t receiver, const size_t source, const double mult);
static inline void Swap_Rows          (double *matrix, const size_t n_rows, const size_t row_1,    const size_t row_2);
static inline void Matrix_Dump        (double *matrix, const size_t n_rows);

int SLE_solver (double *matrix, const size_t n_rows, double *solution)
{    
    MY_ASSERT (matrix,   "double *matrix",   NULL_PTR, ERROR);
    MY_ASSERT (solution, "double *solution", NULL_PTR, ERROR);
    
    Direct_Passing (matrix, n_rows);

    if (Check_Degeneration (matrix, n_rows) == DEGENERATE)
        return ERROR;

    Reverse_Passing (matrix, n_rows);

    for (size_t i = 0; i < n_rows; i++)
        solution[i] = matrix[i * (n_rows + 1) + n_rows];

    return NO_ERRORS;
}

static void Direct_Passing (double *matrix, const size_t n_rows)
{
    for (size_t column_j = 0; column_j < n_rows - 1; column_j++)
    {
        size_t suitable_row_i = -1;

        for (size_t row_i = column_j; row_i < n_rows; row_i++)
        {
            if (Compare_Double (matrix[row_i * (n_rows + 1) + column_j], 0.0))
            {
                suitable_row_i = row_i;
                break;
            }
        }
        
        if (suitable_row_i > column_j)
            Swap_Rows (matrix, n_rows, suitable_row_i, column_j);

        Mult_Row (matrix, n_rows, column_j, 1 / matrix[column_j * (n_rows + 1) + column_j]);

        for (size_t i = column_j + 1; i < n_rows; i++)
            Mult_Then_Add_Rows (matrix, n_rows, i, column_j, -matrix[i * (n_rows + 1) + column_j]);
    }
}

static int Check_Degeneration (const double *matrix, const size_t n_rows)
{
    double check_product = *matrix;
    for (int i = 1; i < n_rows; i++)
        check_product *= matrix[i * (n_rows + 1) + i];

    if (Compare_Double (check_product, 0.0) == 0)
        return DEGENERATE;
    else
        return 0;
}

static void Reverse_Passing (double *matrix, const size_t n_rows)
{
    if (Compare_Double (matrix[(n_rows - 1) * (n_rows + 1) + (n_rows - 1)], 0.0))
        Mult_Row (matrix, n_rows, n_rows - 1, 1 / matrix[(n_rows - 1) * (n_rows + 1) + (n_rows - 1)]);
    
    for (int i = n_rows - 1; i > 0; i--)
    {
        for (int j = 1; j < i + 1; j++)
            Mult_Then_Add_Rows (matrix, n_rows, i - j, i, -matrix[(i - j) * (n_rows + 1) + i]);
    } 
}

static int Compare_Double (const double first, const double second)
{
    double absolute_value = fabs (first - second);

    if (absolute_value > EPSILON)
        return (first > second) ? 1 : -1;
    else
        return 0;
}

static inline void Mult_Row (double *matrix, const size_t n_rows, const size_t row_i, const double mult)
{   
    for (size_t j = 0; j < n_rows + 1; j++)
        matrix[row_i * (n_rows + 1) + j] *= mult;
}

static inline void Mult_Then_Add_Rows (double *matrix, const size_t n_rows, const size_t receiver, const size_t source, const double mult)
{
    for (size_t j = 0; j < n_rows + 1; j++)
        matrix[receiver * (n_rows + 1) + j] += matrix[source * (n_rows + 1) + j] * mult;
}

static inline void Swap_Rows (double *matrix, const size_t n_rows, const size_t row_1, const size_t row_2)
{    
    double *temp_row = (double *)calloc (n_rows + 1, sizeof (double));

    double *row_1_ptr = matrix + row_1 * (n_rows + 1);
    double *row_2_ptr = matrix + row_2 * (n_rows + 1);

    memcpy (temp_row,  row_2_ptr, (n_rows + 1) * sizeof (double));
    memcpy (row_2_ptr, row_1_ptr, (n_rows + 1) * sizeof (double));
    memcpy (row_1_ptr, temp_row,  (n_rows + 1) * sizeof (double));

    free (temp_row);
}

void Matrix_Dump (double *matrix, const size_t n_rows)
{
    for (size_t i = 0; i < n_rows; i++)
    {
        printf ("( ");

        for (size_t j = 0; j < n_rows; j++)
            printf ("%6.3f ", matrix[i * (n_rows + 1) + j]);
        
        printf ("| %.3f )\n", matrix[i * (n_rows + 1) + n_rows]);
    }
}
