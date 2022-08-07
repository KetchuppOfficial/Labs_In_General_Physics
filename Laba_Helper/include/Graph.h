#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED

enum Line_Types
{
    DEFAULT = -1,
    POLINOMICAL,
    DOTS
};

struct Graph;

struct Graph *Graph_Ctor (void);
void          Graph_Dtor (struct Graph *graph);

const char   *Graph_Title       (const struct Graph *graph);
const char   *Graph_Image_Name  (const struct Graph *graph);
const char   *Graph_Dot_Label   (const struct Graph *graph);
const char   *Graph_Dot_Colour  (const struct Graph *graph);
int           Graph_Line_Type   (const struct Graph *graph);
const char   *Graph_Line_Colour (const struct Graph *graph);
const char   *Graph_Err_Colour  (const struct Graph *graph);
const double *Graph_X_Data      (const struct Graph *graph);
const double *Graph_X_Err       (const struct Graph *graph);
const char   *Graph_X_Title     (const struct Graph *graph);
const double *Graph_Y_Data      (const struct Graph *graph);
const double *Graph_Y_Err       (const struct Graph *graph);
const char   *Graph_Y_Title     (const struct Graph *graph);
int           Graph_N_Dots      (const struct Graph *graph);

int Graph_Compiler (struct Graph *graph);
int Print_Graph    (const struct Graph *graph);
#ifdef GRAPH_DUMP
int Graph_Dump (const struct Graph *graph);
#endif // GRAPH_DUMP

#endif // GRAPH_H_INCLUDED
