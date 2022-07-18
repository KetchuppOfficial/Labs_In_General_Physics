#include "../include/LabaCL.h"
#include "My_Lib.h"

#define MAX_FILE_NAME 50

static int          Get_Int                 (void);
static bool         Warn_About_Incorr_Symbs (void);
static inline void  Clear_Stdin             (void);
static char        *Get_String              (char *str, const int n_symbs);
static char        *Make_Buffer            (long *n_symbs);

enum Modes
{
    BUILD_GRAPH = 1,
    CALC_ERROR,
};

int User_Interface (void)
{
    printf ("LabaCL - the best choice for labs in general physics\n\n"
            "I want to (type a number):\n"
            "1) Build a graph;\n"
            "2) Calculate error\n");

    bool error = true;

    while (error)
    {
        int choice = Get_Int ();

        switch (choice)
        {
            case BUILD_GRAPH:
            {
                long n_symbs = 0L;
                char *buffer = Make_Buffer (&n_symbs);
                
                struct Graph *graph = Graph_Compiler (buffer, n_symbs);
                MY_ASSERT (graph, "Graph_Compiler ()", FUNC_ERROR, ERROR);

                free (buffer);

                int PG_status = Print_Graph (graph);
                MY_ASSERT (PG_status != ERROR, "Print_Graph ()", FUNC_ERROR, ERROR);

                Graph_Dtor (graph);

                error = false;
                break;
            }
            case CALC_ERROR:
            {
                printf ("Sorry, calculating error is not supported now :(\n");
                error = false;
                break;
            }
            default:
                printf ("There is no mode with number %d. Please, try again.\n", choice);
        }
    }

    return NO_ERRORS;
}

static int Get_Int (void)
{
    int num = 0.0;
    bool error = true;

    while (error)
    {
        if (scanf ("%d", &num) != 1)
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

static char *Make_Buffer (long *n_symbs)
{
    bool error = true;
    char *buffer = NULL;
    
    while (error)
    {
        printf ("Type the name of file specifying the directory: ");

        char file_name[MAX_FILE_NAME] = "";
        char *ret_str = Get_String (file_name, MAX_FILE_NAME);
        if (!ret_str)
            continue;
            
        buffer = Make_File_Buffer (file_name, n_symbs);
        if (buffer)
            error = false;
        else
            printf ("There is no file with name \"%s\". Please, try again.\n\n", file_name);
    }

    return buffer;
}


static char *Get_String (char *str, const int n_symbs)
{
    int symb_i = 0;
    char *ret_val = fgets (str, n_symbs, stdin);

    if (ret_val)
    {
        while (str[symb_i] != '\n' && str[symb_i] != '\0')
            symb_i++;

        if (str[symb_i] == '\n')
            str[symb_i] = '\0';
        else
        {
            while (getchar () != '\n')
                continue;
        }
    }
    else if (ferror (stdin))
    {
        printf ("Reading from stdin error\n");
        return NULL;
    }
    else if (feof (stdin))
    {
        printf ("EOF reached\n");
        return NULL;
    }

    return ret_val;
}
