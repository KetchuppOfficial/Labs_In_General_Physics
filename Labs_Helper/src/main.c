#include "../include/Labs.h"

int main (void)
{
    struct Graph graph = 
    {
        "Q(\\Delta P) при ламинарном течении",
        "pi * R^4 * delta_P / (8 * eta * l)",

        7,
        "Эксперименатальные точки",
        "green",

        POLINOMICAL,
        "red",

        1,
        2,

        NULL,
        NULL,
        "\\Delta P, Па",

        NULL,
        NULL,
        "Q, мл/c",

        "./График 4.png"
    };
    
    Print_Graph (&graph);

    return 0;
}