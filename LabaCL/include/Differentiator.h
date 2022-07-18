#ifndef DIFFERENTIATOR_H_INCLUDED
#define DIFFERENTIATOR_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_NAME_SIZE 10

enum Types
{  
    SQRT,
    LN,
    SIN,
    COS,
    TAN,
    COT,
    ARCSIN,
    ARCCOS,
    ARCTAN,
    ARCCOT,
    SINH,
    COSH,
    TANH,
    COTH,

    NUMBER,
    VARIABLE,

    PI,
    E_NUM,

    PLUS,
    MINUS,
    MULT,
    DIV,
    POW,

    L_PARANTHESIS,
    R_PARANTHESIS,
};

struct Function
{
    enum Types num;
    const char *name;
};

static const struct Function Functions_Data_Base[] =
{
    {SQRT, "sqrt"},

    {LN, "ln"},

    {SIN, "sin"},
    {COS, "cos"},
    {TAN, "tan"},
    {COT, "cot"},

    {ARCSIN, "arcsin"},
    {ARCCOS, "arccos"},
    {ARCTAN, "arctan"},
    {ARCCOT, "arccot"},

    {SINH, "sinh"},
    {COSH, "cosh"},
    {TANH, "tanh"},
    {COTH, "coth"},
    {-1, ""}
};

union Value
{
    double num;
    char   str[MAX_NAME_SIZE];
};

struct Node
{
    enum   Types type;
    union  Value value;
    struct Node *left_son;
    struct Node *right_son;
    struct Node *parent;
};

struct Var
{
    char *name;
    double value;
    double error;
};

struct Forest
{
    struct Node **tree_arr;
    struct Var  *vars_arr;
    int n_vars;
};

struct Node  *Plant_Tree      (const char *buffer, const long n_symbs);
int           Tree_Destructor (struct Node *node_ptr);
double        Calculate_Tree  (const struct Node *node_ptr, const struct Var *vars_arr, const int n_vars);

int            Differentiator (const struct Node *root, struct Forest *forest);
struct Forest *Forest_Ctor    (const struct Node *root);
int            Forest_Dtor    (struct Forest *forest);

int Tree_Dump (const struct Node *root_ptr, const char *text_file_name, const char *image_file_name, const char *var);

#endif
