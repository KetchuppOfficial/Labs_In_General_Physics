#include <stdbool.h>

#include "Graph.h"
#include "Tools.h"

#define MAX_FILE_NAME 50

static int          Get_Int                 (void);
static bool         Warn_About_Incorr_Symbs (void);
static inline void  Clear_Stdin             (void);

enum Modes
{
    BUILD_GRAPH = 1,
    CALC_ERROR,
};

int User_Interface (void)
{
    printf ("Laba_Helper - the best choice for labs in general physics\n\n"
            "Available modes:\n"
            "1) Build a graph;\n"
            "2) Calculate error\n\n");

    bool error = true;

    while (error)
    {
        int choice = Get_Int ();

        switch (choice)
        {
            case BUILD_GRAPH:
            {
                struct Graph *graph = Graph_Ctor ();
                Graph_Compiler (graph);
                Print_Graph (graph);
                Graph_Dtor (graph);

                error = false;
                break;
            }
            case CALC_ERROR:
                printf ("Sorry, calculating error is not supported now :(\n");
                error = false;
                break;
            
            default:
                printf ("There is no mode with number %d. Please, try again.\n\n", choice);
        }
    }

    return success;
}

static int Get_Int (void)
{
    int num = 0.0;
    bool error = true;

    while (error)
    {
        printf ("Desired mode: ");

        if (scanf ("%d", &num) != 1)
        {
            int incorr_symb = 0;

            printf ("\"");
            while ((incorr_symb = getchar ()) != '\n')
                putchar (incorr_symb);
            printf ("\"");

            printf (" is not a number. Please, try again.\n\n");
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
        printf ("You have written a number and some inappropriate symbols after that. Please, try again.\n\n");
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
