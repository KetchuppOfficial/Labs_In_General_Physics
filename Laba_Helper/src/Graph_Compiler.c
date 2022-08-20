#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

#include "Graph.h"
#include "Tools.h"

// ======================== GRAPH "CLASS" AND ITS METHODS ======================== //

struct Graph
{
    char            *title;
    char            *img_name;

    char            *dot_label;
    char            *dot_colour;

    enum Line_Types  line_type;
    char            *line_colour;

    char            *err_colour;

    #ifdef POLINOMICAL_APPROX
    int              approx_pow;
    #endif // POLINOMICAL_APPROX

    double          *x_data;
    double          *x_err;
    char            *x_title;

    double          *y_data;
    double          *y_err;
    char            *y_title;

    int              n_dots;
};

const char default_dot_colour[]  = "blue";
const char default_err_colour[]  = "red";
const char default_line_colour[] = "green";

struct Graph *Graph_Ctor (void)
{
    struct Graph *graph = (struct Graph *)Calloc_ (1, sizeof (struct Graph));
    if (graph == NULL)
        return NULL;

    graph->line_type = DEFAULT;

    graph->dot_colour = (char *)Calloc_ (sizeof default_dot_colour, sizeof (char));
    if (graph->dot_colour == NULL)
    {
        free (graph);
        return NULL;
    }
    memcpy (graph->dot_colour, default_dot_colour, sizeof default_dot_colour);

    graph->line_colour = (char *)Calloc_ (sizeof default_line_colour, sizeof (char));
    if (graph->line_colour == NULL)
    {
        free (graph->dot_colour);
        free (graph);
        return NULL;
    }
    memcpy (graph->line_colour, default_line_colour, sizeof default_line_colour);

    graph->err_colour = (char *)Calloc_ (sizeof default_err_colour, sizeof (char));
    if (graph->err_colour == NULL)
    {
        free (graph->dot_colour);
        free (graph->line_colour);
        free (graph);
        return NULL;
    }
    memcpy (graph->err_colour, default_err_colour, sizeof default_err_colour);

    #ifdef POLINOMICAL_APPROX
    graph->approx_pow = -1;
    #endif // POLINOMICAL_APPROX

    return graph;
}

void Graph_Dtor (struct Graph *graph)
{
    assert (graph);
    
    free (graph->title);
    free (graph->img_name);
    free (graph->dot_label);
    free (graph->dot_colour);
    free (graph->line_colour);
    free (graph->x_data);
    free (graph->x_err);
    free (graph->x_title);
    free (graph->y_data);
    free (graph->y_err);
    free (graph->y_title);
    free (graph->err_colour);

    free (graph);
}

const char *Graph_Title (const struct Graph *graph)
{
    return graph->title;
}

const char *Graph_Image_Name (const struct Graph *graph)
{
    return graph->img_name;
}

const char *Graph_Dot_Label (const struct Graph *graph)
{
    return graph->dot_label;
}

const char *Graph_Dot_Colour (const struct Graph *graph)
{
    return graph->dot_colour;
}

int Graph_Line_Type (const struct Graph *graph)
{
    return graph->line_type;
}

const char *Graph_Line_Colour (const struct Graph *graph)
{
    return graph->line_colour;
}

const char *Graph_Err_Colour (const struct Graph *graph)
{
    return graph->err_colour;
}

const double *Graph_X_Data (const struct Graph *graph)
{
    return graph->x_data;
}

const double *Graph_X_Err (const struct Graph *graph)
{
    return graph->x_err;
}

const char *Graph_X_Title (const struct Graph *graph)
{
    return graph->x_title;
}

const double *Graph_Y_Data (const struct Graph *graph)
{
    return graph->y_data;
}

const double *Graph_Y_Err (const struct Graph *graph)
{
    return graph->y_err;
}

const char *Graph_Y_Title (const struct Graph *graph)
{
    return graph->y_title;
}

int Graph_N_Dots (const struct Graph *graph)
{
    return graph->n_dots;
}

// ========================================= GRAPH COMPILER ========================================= //

static int             D_Preprocessor (struct Buffer *buffer);
static struct D_Token *D_Lexer        (const struct Buffer buffer, int *n_tokens);
static int             D_Parser       (const struct D_Token *token_arr, const int n_tokens, 
                                       const struct Buffer buffer, struct Graph *graph);

enum Token_Enum
{
    GRAPH_TITLE,

    DOT_LABEL,
    DOT_COLOUR,     // Optional. "Blue" by default

    NO_LINE,        // Optional. If it is used, no line will be printed
    LINE_COLOUR,    // Optional. "Green" by default

    #ifdef POLINOMICAL_APPROX
    APPROX_POW,     // Have to be 2 more than number of dots
    #endif // POLINOMICAL_APPROX

    X_DATA,
    X_ERR,
    X_TITLE,

    Y_DATA,
    Y_ERR,
    Y_TITLE,

    ERR_COLOUR,     // Optional. "Red" by default

    IMG_NAME,

    INT_NUM,
    FP_NUM,
    STR
};

struct Label
{
    enum Token_Enum num;
    char *name;
    size_t name_len;
};

const struct Label Labels_Arr[] = 
{
    {GRAPH_TITLE, "Graph_Title",  sizeof "Graph_Title"  - 1},

    {DOT_LABEL,   "Dot_Label",    sizeof "Dot_Label"    - 1},
    {DOT_COLOUR,  "Dot_Colour",   sizeof "Dot_Colour"   - 1},

    {NO_LINE,     "No_Line",      sizeof "No_Line"      - 1},
    {LINE_COLOUR, "Line_Colour",  sizeof "Line_Colour"  - 1},

    #ifdef POLINOMICAL_APPROX
    {APPROX_POW,  "Approximation_Power", sizeof "Approximation_Power" - 1},
    #endif // POLINOMICAL_APPROX

    {X_DATA,      "X_Data",       sizeof "X_Data"       - 1},
    {X_ERR,       "X_Error",      sizeof "X_Error"      - 1},
    {X_TITLE,     "X_Title",      sizeof "X_Title"      - 1},

    {Y_DATA,      "Y_Data",       sizeof "Y_Data"       - 1},
    {Y_ERR,       "Y_Error",      sizeof "Y_Error"      - 1},
    {Y_TITLE,     "Y_Title",      sizeof "Y_Title"      - 1},

    {ERR_COLOUR,  "Error_Colour", sizeof "Error_Colour" - 1},

    {IMG_NAME,    "Image_Name",   sizeof "Image_Name"   - 1}
};

const size_t N_LABELS = sizeof (Labels_Arr) / sizeof (struct Label);

union D_Value
{
    int int_num;
    double fp_num;
    char *str;
};

struct D_Token
{
    enum Token_Enum name;
    union D_Value val;
    size_t buff_pos;
};

int Graph_Compiler (struct Graph *graph)
{
    assert (graph);

    struct Buffer buffer = Open_File ();

    if (D_Preprocessor (&buffer) == error)
    {
        #ifdef LABA_HELPER_DEBUG
        printf ("D_Preprocessor ()) terminated with error\n");
        #endif // LABA_HELPER_DEBUG
        return error;
    }

    int n_tokens = 0;
    struct D_Token *token_arr = D_Lexer (buffer, &n_tokens);
    if (token_arr == NULL)
    {
        #ifdef LABA_HELPER_DEBUG
        printf ("D_Lexer () terminated with error\n");
        #endif // LABA_HELPER_DEBUG
        return error;
    }

    if (D_Parser (token_arr, n_tokens, buffer, graph) == error)
    {
        #ifdef LABA_HELPER_DEBUG
        printf ("D_Parser () terminated with error\n");
        #endif // LABA_HELPER_DEBUG
        return error;
    }

    for (int token_i = 0; token_i < n_tokens; token_i++)
        if (token_arr[token_i].name == STR)
            free (token_arr[token_i].val.str);

    free (token_arr);

    free (buffer.str);

    return success;
}

// ========================== PREPROCESSOR ========================== //

static void Show_Error (const struct Buffer buffer, const size_t err_symb_i, const char *err_descr);

static int D_Preprocessor (struct Buffer *buffer)
{
    assert (buffer);
    assert (buffer->str);
    
    bool in_string = false;
    
    for (size_t symb_i = 0L; symb_i < buffer->n_symbs; symb_i++)
    {
        if (buffer->str[symb_i] == '\"')
            in_string = (in_string == true) ? false : true;
        
        if (in_string == false && buffer->str[symb_i] == '/')
        {
            if (symb_i + 1 < buffer->n_symbs && buffer->str[symb_i + 1] == '/')
            {
                while (buffer->str[symb_i] != '\n')
                    buffer->str[symb_i++] = ' ';
            }
            else
            {
                Show_Error (*buffer, symb_i, "Incorrect symbol");
                return error;
            }
        }
    }

    return success;
}

static void Show_Error (const struct Buffer buffer, const size_t err_symb_i, const char *err_descr)
{
    assert (buffer.str);
    assert (err_descr);
    
    printf ("************ ERROR REPORT ************\n");

    size_t symb_i = err_symb_i;
    while (symb_i > 0 && buffer.str[symb_i] != '\n')
        symb_i--;
    symb_i++;
    
    int n_lines = 1;
    for (size_t i = 0; i < symb_i; i++)
    {
        if (buffer.str[i] == '\n')
            n_lines++;
    }

    printf ("LINE %d:\n", n_lines);
    
    for (size_t i = 0; symb_i + i < buffer.n_symbs && buffer.str[symb_i + i] != '\n'; i++)
        printf ("%c", buffer.str[symb_i + i]);
    printf ("\n");
    
    for (size_t i = symb_i; i < err_symb_i; i++)
        printf (" ");

    printf ("^~~~~~~~~~ %s\n", err_descr);

    printf ("**************************************\n");
}

// ================================================================== //

// ============================= LEXER ============================== //

static inline void Skip_Spaces    (const struct Buffer buffer, size_t *symb_i);
static int         Get_Token      (struct D_Token *token_arr, const int token_i, const struct Buffer buffer, size_t *symb_i);
static int         Get_Label_Name (struct D_Token *token_arr, const int token_i, const struct Buffer buffer, size_t *symb_i);
static int         Get_Str        (struct D_Token *token_arr, const int token_i, const struct Buffer buffer, size_t *symb_i);
static int         Get_Number     (struct D_Token *token_arr, const int token_i, const struct Buffer buffer, size_t *symb_i);

static struct D_Token *D_Lexer (const struct Buffer buffer, int *n_tokens)
{
    assert (buffer.str);
    assert (n_tokens);

    struct D_Token *token_arr = (struct D_Token *)Calloc_ (buffer.n_symbs, sizeof (struct D_Token));
    if (token_arr == NULL)
        return NULL;

    int token_i = 0;

    for (size_t symb_i = 0L; symb_i < buffer.n_symbs; token_i++)
    {        
        Skip_Spaces (buffer, &symb_i);

        if (symb_i >= buffer.n_symbs)
            break;

        if (Get_Token (token_arr, token_i, buffer, &symb_i) == error)
        {
            #ifdef LABA_HELPER_DEBUG
            printf ("Get_Token () terminated with error\n");
            #endif // LABA_HELPER_DEBUG
            free (token_arr);
            return NULL;
        }
    }

    *n_tokens = token_i;

    return token_arr;
}

static inline void Skip_Spaces (const struct Buffer buffer, size_t *symb_i)
{
    assert (buffer.str);
    
    while (*symb_i < buffer.n_symbs && isspace (buffer.str[*symb_i]))
        (*symb_i)++;
}

static int Get_Token (struct D_Token *token_arr, const int token_i, const struct Buffer buffer, size_t *symb_i)
{
    assert (token_arr);
    assert (buffer.str);
    assert (symb_i);
    
    if (('a' <= buffer.str[*symb_i] && buffer.str[*symb_i] <= 'z') ||
        ('A' <= buffer.str[*symb_i] && buffer.str[*symb_i] <= 'Z'))
    {
        if (Get_Label_Name (token_arr, token_i, buffer, symb_i) == error)
        {
            #ifdef LABA_HELPER_DEBUG
            printf ("Get_Label_Name () terminated with error\n");
            #endif // LABA_HELPER_DEBUG
            return error;
        }
    }
    else if (buffer.str[*symb_i] == '\"')
    {
        (*symb_i)++;
        
        if (Get_Str (token_arr, token_i, buffer, symb_i) == error)
        {
            #ifdef LABA_HELPER_DEBUG
            printf ("Get_String () terminated with error\n");
            #endif // LABA_HELPER_DEBUG
            return error;
        }
    }
    else if (buffer.str[*symb_i] == '-' || 
             ('0' <= buffer.str[*symb_i] && buffer.str[*symb_i] <= '9'))
    {
        if (Get_Number (token_arr, token_i, buffer, symb_i) == error)
        {
            #ifdef LABA_HELPER_DEBUG
            printf ("Get_Number () terminated with error\n");
            #endif // LABA_HELPER_DEBUG
            return error;
        }
    }
    else
    {
        Show_Error (buffer, *symb_i, "Not allowed type of argument");
        return error;
    }

    return success;
}

static int Get_Label_Name (struct D_Token *token_arr, const int token_i, const struct Buffer buffer, size_t *symb_i)
{   
    assert (token_arr);
    assert (buffer.str);
    assert (symb_i);
    
    for (size_t label_i = 0; label_i < N_LABELS; label_i++)
    {       
        size_t label_name_len = Labels_Arr[label_i].name_len;
        
        if (strncmp (buffer.str + *symb_i, Labels_Arr[label_i].name, label_name_len) == 0)
        {
            *symb_i += label_name_len;

            if (*symb_i < buffer.n_symbs && buffer.str[*symb_i] == ':')
            {
                (*symb_i)++;
                token_arr[token_i].name = label_i;
                token_arr[token_i].buff_pos = *symb_i - label_name_len - 1;
                return success;
            }
            else
            {
                Show_Error (buffer, *symb_i, "Missing colon");
                return error;
            }
        }
    }

    Show_Error (buffer, *symb_i, "Unknown label");
    return error;
}

static int Get_Str (struct D_Token *token_arr, const int token_i, const struct Buffer buffer, size_t *symb_i)
{       
    assert (token_arr);
    assert (buffer.str);
    assert (symb_i);
    
    int str_len = 0;
    while (*symb_i + str_len < buffer.n_symbs && buffer.str[*symb_i + str_len] != '\"')
        str_len++;

    if (str_len == 0)
    {
        Show_Error (buffer, *symb_i + 1, "Empty strings are not allowed");
        return error;
    }
    else
    {
        char *str = (char *)Calloc_ (str_len + 1, sizeof (char));
        if (str == NULL)
            return error;

        memcpy (str, buffer.str + *symb_i, str_len);

        token_arr[token_i].name     = STR;
        token_arr[token_i].val.str  = str;
        token_arr[token_i].buff_pos = *symb_i;

        *symb_i += str_len + 1;

        return success;
    }
}

static int Get_Number (struct D_Token *token_arr, const int token_i, const struct Buffer buffer, size_t *symb_i)
{    
    assert (token_arr);
    assert (buffer.str);
    assert (symb_i);
    
    int num = 0;

    token_arr[token_i].buff_pos = *symb_i;

    bool positive = true;
    if (buffer.str[*symb_i] == '-')
    {
        positive = false;
        (*symb_i)++;
    }
    
    if ('0' <= buffer.str[*symb_i] && buffer.str[*symb_i] <= '9')
    {
        while ('0' <= buffer.str[*symb_i] && buffer.str[*symb_i] <= '9')
        {
            num = num * 10 + (buffer.str[*symb_i] - '0');
            (*symb_i)++;
        }
    }
    else
    {
        Show_Error (buffer, *symb_i, "Minus without a number is not allowed");
        return error;
    }

    if (buffer.str[*symb_i] == '.')
    {
        double fp_num = (double)num;
        
        (*symb_i)++;
        const char *old_str = buffer.str + *symb_i;

        int degree = -1;

        while ('0' <= buffer.str[*symb_i] && buffer.str[*symb_i] <= '9')
        {
            fp_num += (buffer.str[*symb_i] - '0') * pow (10, degree);
            degree--;
            (*symb_i)++;
        }

        if (old_str != buffer.str + *symb_i)
        {
            token_arr[token_i].name = FP_NUM;
            token_arr[token_i].val.fp_num = (positive == true) ? fp_num : -fp_num;
        }
        else
        {
            Show_Error (buffer, *symb_i, "No digits after the decimal point");
            return error;
        }
    }
    else
    {
        token_arr[token_i].name = INT_NUM;
        token_arr[token_i].val.int_num = (positive == true) ? num : -num;
    }

    return success;
}

// ================================================================================================================================== //

// ============================================================= PARSER ============================================================= //

struct Values
{
    int x_data;
    int y_data;
    int x_err;
    int y_err;
};

static int  One_Label_Check (const struct D_Token *token_arr, const int token_i, const struct Buffer buffer);
#ifdef POLINOMICAL_APPROX
static int  Handle_Int_Num  (struct Graph *graph, const struct D_Token *token_arr, const int token_i, 
                             const struct Buffer buffer);
#endif // POLINOMICAL_APPROX
static int  Handle_FP_Num   (struct Graph *graph, const struct D_Token *token_arr, const int token_i, 
                             const struct Buffer buffer, struct Values *values);
static int  Handle_String   (struct Graph *graph, const struct D_Token *token_arr, const int token_i, 
                             const struct Buffer buffer);
static int  Check_Graph     (struct Graph *graph, const struct Values *values);
#ifdef DC_PARSER_DUMP
static int Parser_Dump (const struct D_Token *token_arr, const int n_tokens);
#endif // DC_PARSER_DUMP

static int D_Parser (const struct D_Token *token_arr, const int n_tokens,
                     const struct Buffer buffer, struct Graph *graph)
{
    assert (token_arr);
    assert (buffer.str);
    assert (graph);
    
    struct Values values = {};
    
    for (int token_i = 0; token_i < n_tokens; token_i++)
    {       
        switch (token_arr[token_i].name)
        {
            case X_DATA:
                graph->x_data = (double *)Calloc_ (n_tokens, sizeof (double));
                if (graph->x_data == NULL)
                    return error;
                if (One_Label_Check (token_arr, token_i, buffer) == error)
                {
                    #ifdef LABA_HELPER_DEBUG
                    printf ("One_Label_Check () terminated with error\n");
                    #endif // LABA_HELPER_DEBUG
                    free (graph->x_data);
                    return error;
                }
                break;

            case X_ERR:
                graph->x_err = (double *)Calloc_ (n_tokens, sizeof (double));
                if (graph->x_err == NULL)
                    return error;
                if (One_Label_Check (token_arr, token_i, buffer) == error)
                {
                    #ifdef LABA_HELPER_DEBUG
                    printf ("One_Label_Check () terminated with error\n");
                    #endif // LABA_HELPER_DEBUG
                    free (graph->x_err);
                    return error;
                }
                break;

            case Y_DATA:
                graph->y_data = (double *)Calloc_ (n_tokens, sizeof (double));
                if (graph->y_data == NULL)
                    return error;
                if (One_Label_Check (token_arr, token_i, buffer) == error)
                {
                    #ifdef LABA_HELPER_DEBUG
                    printf ("One_Label_Check () terminated with error\n");
                    #endif // LABA_HELPER_DEBUG
                    free (graph->y_data);
                    return error;
                }
                break;

            case Y_ERR:
                graph->y_err = (double *)Calloc_ (n_tokens, sizeof (double));
                if (graph->y_err == NULL)
                if (One_Label_Check (token_arr, token_i, buffer) == error)
                {
                    #ifdef LABA_HELPER_DEBUG
                    printf ("One_Label_Check () terminated with error\n");
                    #endif // LABA_HELPER_DEBUG
                    free (graph->y_err);
                    return error;
                }
                break;
            
            case NO_LINE:
                graph->line_type = DOTS;
                if (One_Label_Check (token_arr, token_i, buffer) == error)
                {
                    #ifdef LABA_HELPER_DEBUG
                    printf ("One_Label_Check () terminated with error\n");
                    #endif // LABA_HELPER_DEBUG
                    return error;
                }
                break;

            case GRAPH_TITLE:
            case DOT_LABEL:
            case DOT_COLOUR:
            case LINE_COLOUR:
            case ERR_COLOUR:
            #ifdef POLINOMICAL_APPROX
            case APPROX_POW:
            #endif // POLINOMICAL_APPROX
            case X_TITLE:
            case Y_TITLE:
            case IMG_NAME:
                if (One_Label_Check (token_arr, token_i, buffer) == error)
                {
                    #ifdef LABA_HELPER_DEBUG
                    printf ("One_Label_Check () terminated with error\n");
                    #endif // LABA_HELPER_DEBUG
                    return error;
                }
                break;

            #ifdef POLINOMICAL_APPROX
            case INT_NUM:
            {
                if (Handle_Int_Num (graph, token_arr, token_i, buffer) == error)
                {
                    #ifdef LABA_HELPER_DEBUG
                    printf ("Handle_Int_Num () terminated with error\n");
                    #endif // LABA_HELPER_DEBUG
                }
                break;
            }
            #endif // POLINOMICAL_APPROX

            case FP_NUM:
                if (Handle_FP_Num (graph, token_arr, token_i, buffer, &values) == error)
                {
                    #ifdef LABA_HELPER_DEBUG
                    printf ("Handle_FP_Num () terminated with error\n");
                    #endif // LABA_HELPER_DEBUG
                    return error;
                }
                break;

            case STR:               
                if (Handle_String (graph, token_arr, token_i, buffer) == error)
                {
                    #ifdef LABA_HELPER_DEBUG
                    printf ("Handle_String () terminated with error\n");
                    #endif // LABA_HELPER_DEBUG
                    return error;
                }
                break;

            default:
                #ifdef LABA_HELPER_DEBUG
                printf ("Unknown value found: token_arr[token_i].name = %d\n",
                        token_arr[token_i].name);
                #endif // LABA_HELPER_DEBUG
                return error;
        }
    }

    if (Check_Graph (graph, &values) == error)
    {
        #ifdef LABA_HELPER_DEBUG
        printf ("Check_Graph () terminated with error\n");
        #endif // LABA_HELPER_DEBUG
        return error;
    }

    if (graph->line_type == DEFAULT)
        graph->line_type = POLINOMICAL;
    
    return success;
}

static int One_Label_Check (const struct D_Token *token_arr, const int token_i, const struct Buffer buffer)
{
    assert (token_arr);
    assert (buffer.str);
    
    for (int j = 0; j < token_i; j++)
    {
        if (token_arr[j].name == token_arr[token_i].name)
        {
            size_t err_str_len = sizeof ("Only one \"") + sizeof ("\" label is allowed") + Labels_Arr[token_arr[j].name].name_len;
            char *err_str = (char *)Calloc_ (err_str_len, sizeof (char));
            if (err_str == NULL)
                return error;
            sprintf (err_str, "Only one \"%s\" label is allowed\n", Labels_Arr[token_arr[j].name].name);
            
            Show_Error (buffer, token_arr[token_i].buff_pos, err_str);
            free (err_str);
            return error;
        }
    }

    return success;
}

#ifdef POLINOMICAL_APPROX
static int Handle_Int_Num (struct Graph *graph, const struct D_Token *token_arr, const int token_i, 
                           const struct Buffer buffer)
{
    assert (graph);
    assert (token_arr);
    assert (buffer.str);
    
    if (token_i > 0)
    {
        switch (token_arr[token_i - 1].name)
        {
            case APPROX_POW:
                graph->approx_pow = token_arr[token_i].val.int_num;
                break;

            default:
                Show_Error (buffer, token_arr[token_i].buff_pos, 
                            "Previous token should be \"Approx_Pow\", \"X_Accurace\" or \"Y_Accuracy\"");
                return error;
        }
    }
    else
    {
        Show_Error (buffer, token_arr[0].buff_pos, "First token cannot be a number");
        return error;
    }

    return success;
}
#endif // POLINOMICAL_APPROX

static int Handle_FP_Num (struct Graph *graph, const struct D_Token *token_arr, const int token_i, 
                          const struct Buffer buffer, struct Values *values)
{
    assert (graph);
    assert (token_arr);
    assert (buffer.str);
    assert (values);
    
    if (token_i > 0)
    {
        switch (token_arr[token_i - 1].name)
        {
            case X_DATA:
                graph->x_data[values->x_data++] = token_arr[token_i].val.fp_num;
                break;
            case X_ERR:
                graph->x_err[values->x_err++]   = token_arr[token_i].val.fp_num;
                break;
            case Y_DATA:
                graph->y_data[values->y_data++] = token_arr[token_i].val.fp_num;
                break;
            case Y_ERR:
                graph->y_err[values->y_err++]   = token_arr[token_i].val.fp_num;
                break;
            case FP_NUM:
            {
                int i = token_i - 2;

                while (token_arr[i].name == FP_NUM)
                    i--;  

                switch (token_arr[i].name)
                {
                    case X_DATA:
                        graph->x_data[values->x_data++] = token_arr[token_i].val.fp_num;
                        break;
                    case X_ERR:
                        graph->x_err[values->x_err++]   = token_arr[token_i].val.fp_num;
                        break;
                    case Y_DATA:
                        graph->y_data[values->y_data++] = token_arr[token_i].val.fp_num;
                        break;
                    case Y_ERR:
                        graph->y_err[values->y_err++]   = token_arr[token_i].val.fp_num;
                        break;
                    default:
                        Show_Error (buffer, token_arr[i].buff_pos, 
                                    "This token cannot have floating point number as argument");
                }

                break;
            }
                    
            default:
                Show_Error (buffer, token_arr[token_i].buff_pos, 
                            "Previous token should be \"X_Data\" or \"Y_Data\" or a floating point num");
                return error;
        }
    }
    else
    {
        Show_Error (buffer, token_arr[0].buff_pos, "First token cannot be a number");
        return error;
    }

    return success;
}

#define HANDLE_STRING_MACRO(field)                          \
do                                                          \
{                                                           \
    field = (char *)Calloc_ (str_len + 1, sizeof (char));   \
    if (field == NULL)                                      \
        return error;                                       \
    memcpy (field, token_arr[token_i].val.str, str_len);    \
}                                                           \
while (0)

static int Handle_String (struct Graph *graph, const struct D_Token *token_arr, const int token_i, 
                          const struct Buffer buffer)
{
    assert (graph);
    assert (token_arr);
    assert (buffer.str);
    
    size_t str_len = strlen (token_arr[token_i].val.str);
                
    if (token_i > 0)
    {
        switch (token_arr[token_i - 1].name)
        {
            case GRAPH_TITLE:
                HANDLE_STRING_MACRO (graph->title);
                break;
            case DOT_LABEL:
                HANDLE_STRING_MACRO (graph->dot_label);
                break;
            case DOT_COLOUR:
                free (graph->dot_colour);
                HANDLE_STRING_MACRO (graph->dot_colour);
                break;  
            case LINE_COLOUR:
                free (graph->line_colour);
                HANDLE_STRING_MACRO (graph->line_colour);
                break;
            case X_TITLE:
                HANDLE_STRING_MACRO (graph->x_title);
                break;
            case Y_TITLE:
                HANDLE_STRING_MACRO (graph->y_title);
                break;
            case ERR_COLOUR:
                free (graph->err_colour);
                HANDLE_STRING_MACRO (graph->err_colour);
                break;
            case IMG_NAME:
                HANDLE_STRING_MACRO (graph->img_name);
                break;

            default:
                Show_Error (buffer, token_arr[token_i - 1].buff_pos, 
                            "This label cannot have a string argument");
                return error;
        }
    }
    else
    {
        Show_Error (buffer, token_arr[0].buff_pos, 
                    "First token cannot be a string");
        return error;
    }

    return success;
}

static int Check_Graph (struct Graph *graph, const struct Values *values)
{      
    assert (graph);
    assert (values);
    
    if (graph->title == NULL)
    {
        printf ("\n"
                "************ ERROR REPORT ************\n"
                "*  \"Graph_Title\" label is forgotten   *\n"
                "**************************************\n");
        return error;
    }
    
    if (graph->dot_label == NULL)
    {
        printf ("\n"
                "************ ERROR REPORT ************\n"
                "*   \"Dot_Label\" label is forgotten   *\n"
                "**************************************\n");
        return error;
    }

    #ifdef POLINOMICAL_APPROX
    if (graph->approx_pow == -1)
    {
        printf ("************ ERROR REPORT ************\n"
                "\"Approximation_Power\" label is forgotten\n"
                "**************************************\n");
        return error;
    }
    #endif // POLINOMICAL_APPROX

    if (graph->x_data == NULL)
    {
        printf ("\n"
                "************ ERROR REPORT ************\n"
                "*    \"X_Data\" label is forgotten     *\n"
                "**************************************\n");
        return error;
    }
    
    if (graph->x_err == NULL)
    {
        printf ("\n"
                "************ ERROR REPORT ************\n"
                "*    \"X_Error\" label is forgotten    *\n"
                "**************************************\n");
        return error;
    }
    
    if (graph->x_title == NULL)
    {
        printf ("\n"
                "************ ERROR REPORT ************\n"
                "*    \"X_Error\" label is forgotten    *\n"
                "**************************************\n");
        return error;
    }
    
    if (graph->y_data == NULL)
    {
        printf ("\n"
                "************ ERROR REPORT ************\n"
                "*    \"Y_Data\" label is forgotten     *\n"
                "**************************************\n");
        return error;
    }
    
    if (graph->y_err == NULL)
    {
        printf ("\n"
                "************ ERROR REPORT ************\n"
                "*    \"Y_Error\" label is forgotten    *\n"
                "**************************************\n");
        return error;
    }
    
    if (graph->y_title == NULL)
    {
        printf ("\n"
                "************ ERROR REPORT ************\n"
                "*    \"Y_Error\" label is forgotten    *\n"
                "**************************************\n");
        return error;
    }
    
    if (graph->img_name == NULL)
    {
        printf ("\n"
                "************ ERROR REPORT ************\n"
                "*  \"Image_Name\" label is forgotten   *\n"
                "**************************************\n");
        return error;
    }

    #ifdef POLINOMICAL_APPROX
    if (graph->approx_pow + 2 <= graph->n_dots)
    {
        printf ("************ ERROR REPORT ************\n"
                "\"Approximation_Power\" has to be 2 more than number of dots or they have to be equal\n"
                "**************************************\n");
        return error;
    }
    #endif // POLINOMICAL_APPROX

    if (values->x_data != values->y_data || 
        values->x_data != values->x_err  || 
        values->y_data != values->y_err)
    {       
        printf ("************************************** ERROR REPORT ***************************************\n"
                "The number of \"X_Data\", \"Y_Data\", \"X_Error\" and \"Y_Error\" arguments should be equal\n"
                "*******************************************************************************************\n");
        return error;
    }
    else
        graph->n_dots = values->x_data;
    
    if (graph->n_dots < 3)
    {
        printf ("******************** ERROR REPORT ********************\n"
                "The quantity of dots on the graph has to be at least 3\n"
                "******************************************************\n");
        return error;
    }

    return success;
}

#ifdef DC_PARSER_DUMP
static int Parser_Dump (const struct D_Token *token_arr, const int n_tokens)
{
    for (int token_i = 0; token_i < n_tokens; token_i++)
    {
        switch (token_arr[token_i].name)
        {
            case GRAPH_TITLE:
                printf ("GRAPH_TITLE\n");
                break;
                
            case DOT_LABEL:
                printf ("DOT_LABEL\n");
                break;

            case DOT_COLOUR:
                printf ("DOT_COLOUR\n");
                break;

            case NO_LINE:
                printf ("NO_LINE\n");
                break;

            case LINE_COLOUR:
                printf ("LINE_COLOUR\n");
                break;

            #ifdef POLINOMICAL_APPROX
            case APPROX_POW:
                printf ("APPROX_POW\n");
                break;
            #endif

            case X_DATA:
                printf ("X_DATA\n");
                break;

            case X_ERR:
                printf ("X_ERR\n");
                break;

            case X_TITLE:
                printf ("X_TITLE\n");
                break;

            case Y_DATA:
                printf ("Y_DATA\n");
                break;

            case Y_ERR:
                printf ("Y_ERR\n");
                break;

            case Y_TITLE:
                printf ("Y_TITLE\n");
                break;

            case ERR_COLOUR:
                printf ("Error_Colour\n");

            case IMG_NAME:
                printf ("IMG_NAME\n");
                break;

            #ifdef POLINOMICAL_APPROX
            case INT_NUM:
                printf ("INT_NUM: %d\n", token_arr[token_i].val.int_num);
                break;
            #endif // POLINOMICAL_APPROX

            case FP_NUM:
                printf ("FP_NUM: %f\n", token_arr[token_i].val.fp_num);
                break;

            case STR:
                printf ("STR: %s\n", token_arr[token_i].val.str);
                break;
            
            default:
                #ifdef GRAPH_COMPILER_DEBUG
                printf ("Unknown value found: token_arr[token_i].name = %d\n",
                        token_arr[token_i].name);
                #endif
                return error;
        }
    }

    return NO_ERRORS;
}
#endif // DC_PARSER_DUMP

// ================================================================================================================================== //

#ifdef GRAPH_DUMP
static int Graph_Dump (const struct Graph *graph)
{
    assert (graph);
    
    system ("mkdir -p graph_dump");
    
    FILE *dump_file = fopen ("graph_dump/dump.txt", "wb");
    if (dump_file == NULL)
    {
        #ifdef GRAPH_COMPILER_DEBUG
        printf ("Opening \"graph_dump/dump.txt\" failed\n");
        #endif
        return error;
    }

    fprintf (dump_file, "Graph_Title:\n\t%s\n\n", graph->title);

    fprintf (dump_file, "N_Dots:\n\t%d\n\n",     graph->n_dots);
    fprintf (dump_file, "Dot_Label:\n\t%s\n\n",  graph->dot_label);
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
            fprintf (dump_file, "Line_Type:\n\tUnknown value %d\n\n", graph->line_type);
            break;
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

    fclose (dump_file);

    return success;
}
#endif // GRAPH_DUMP

// ==================================================================================================== //
