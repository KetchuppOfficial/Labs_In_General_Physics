#include <stdbool.h>

#include "Graph.h"
#include "Tools.h"

#define MAX_FILE_NAME 50

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

    bool error_flag = true;

    while (error_flag)
    {
        int choice = Get_Int ();

        switch (choice)
        {
            case BUILD_GRAPH:
            {
                struct Graph *graph = Graph_Ctor ();
                if (graph == NULL)
                {
                    #ifdef LABA_HELPER_DEBUG
                    printf ("Graph_Ctor () terminated with error\n");
                    #endif // LABA_HELPER_DEBUG
                    return error;
                }

                if (Graph_Compiler (graph) == error)
                {
                    #ifdef LABA_HELPER_DEBUG
                    printf ("Graph_Compiler () terminated with error\n");
                    #endif // LABA_HELPER_DEBUG
                    return error;
                }

                if (Print_Graph (graph) == error)
                {
                    #ifdef LABA_HELPER_DEBUG
                    printf ("Print_Graph () terminated with error\n");
                    #endif // LABA_HELPER_DEBUG
                    return error;
                }

                Graph_Dtor (graph);

                error_flag = false;
                break;
            }
            
            case CALC_ERROR:
                printf ("Sorry, calculating error is not supported now :(\n");
                error_flag = false;
                break;
            
            default:
                printf ("There is no mode with number %d. Please, try again.\n\n", choice);
        }
    }

    return success;
}
