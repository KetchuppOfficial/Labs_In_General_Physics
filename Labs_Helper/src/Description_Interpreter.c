#include "My_Lib.h"
#include "../include/Labs.h"

enum Token_Enum
{
    GRAPH_TITLE,
    FUNCTION,

    DOT_LABEL,
    DOT_COLOUR,     // Optional. Green by default

    NO_LINE,        // Optional. If it is used, no line will be printed
    LINE_COLOUR,    // Optional. Red by default

    APPROX_POW,     // Have to be 2 more than number of dots

    X_ASIX_DATA,
    X_ERR,
    X_ACCURACY,
    X_TITLE,

    Y_ASIX_DATA,
    Y_ACCURACY,
    Y_TITLE,

    IMG_NAME,

    INT_NUM,
    FP_NUM,
    STR
};

struct Label
{
    enum Token_Enum num;
    char *name;
};

struct Label Labels_Arr[] = 
{
    {GRAPH_TITLE, "Graph_Title"},
    {FUNCTION,    "Function"},

    {DOT_LABEL,   "Dot_Label"},
    {DOT_COLOUR,  "Dot_Colour"},

    {NO_LINE,     "No_Line"},
    {LINE_COLOUR, "Line_Colour"},

    {APPROX_POW,  "Approximation_Power"},

    {X_ASIX_DATA, "X_Asix_Data"},
    {X_ERR,       "X_Error"},
    {X_ACCURACY,  "X_Accuracy"},
    {X_TITLE,     "X_Title"},

    {Y_ASIX_DATA, "Y_Asix_Data"},
    {Y_ACCURACY,  "Y_Accuracy"},
    {Y_TITLE,     "Y_Title"},

    {IMG_NAME,    "Image_Name"}
};

#define N_LABELS (sizeof (Labels_Arr) / sizeof (struct Label))

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

struct Values
{
    int x;
    int y;
    int x_err;
};

static struct Graph   *Graph_Ctor     (void);
static void            Token_Arr_Dtor (struct D_Token *token_arr, const int n_tokens);

static int  D_Preprocessor (char *buffer, const long n_symbs);
static void Show_Error     (const char *buffer, const long n_symbs, const long err_symb_i, const char *err_descr);

static struct D_Token *D_Lexer        (const char *buffer, const long n_symbs, int *n_tokens);
static inline void     Skip_Spaces    (const char *buffer, const long n_symbs, long *symb_i);
static int             Get_Token      (struct D_Token *token_arr, const int token_i, const char *const buffer, const long n_symbs, long *symb_i);
static int             Get_Label_Name (struct D_Token *token_arr, const int token_i, const char *const buffer, const long n_symbs, long *symb_i);
static int             Get_String     (struct D_Token *token_arr, const int token_i, const char *const buffer, const long n_symbs, long *symb_i);
static int             Get_Number     (struct D_Token *token_arr, const int token_i, const char *const buffer, const long n_symbs, long *symb_i);

static int  D_Parser        (const struct D_Token *token_arr, const int n_tokens, const char *buffer, const long n_symbs, struct Graph *graph);
static int  One_Label_Check (const struct D_Token *token_arr, const int token_i, const char *buffer, const long n_symbs);
static int  Handle_Int_Num  (struct Graph *graph, const struct D_Token *token_arr, const int token_i, const char *buffer, const long n_symbs);
static int  Handle_FP_Num   (struct Graph *graph, const struct D_Token *token_arr, const int token_i, const char *buffer, const long n_symbs, struct Values *values);
static int  Handle_String   (struct Graph *graph, const struct D_Token *token_arr, const int token_i, const char *buffer, const long n_symbs);
static int  Check_Graph     (struct Graph *graph, const struct Values *values);
static void Set_Defaults    (struct Graph *graph);

#ifdef PARSER_DUMP
static void Parser_Dump (const struct D_Token *token_arr, const int n_tokens);
#endif

struct Graph *Description_Interpreter (const char *file_name)
{
    MY_ASSERT (file_name, "const char *file_name", NULL_PTR, NULL);

    long n_symbs = 0L;
    char *buffer = Make_File_Buffer (file_name, &n_symbs);
    MY_ASSERT (buffer, "Make_File_Buffer ()", FUNC_ERROR, NULL);

    int DPrep_status = D_Preprocessor (buffer, n_symbs);
    MY_ASSERT (DPrep_status != ERROR, "Preprocessor ()", FUNC_ERROR, NULL);

    int n_tokens = 0;
    struct D_Token *token_arr = D_Lexer (buffer, n_symbs, &n_tokens);
    MY_ASSERT (token_arr, "Lexer ()", FUNC_ERROR, NULL);
    
    struct Graph *graph = Graph_Ctor ();

    int DPar_status = D_Parser (token_arr, n_tokens, buffer, n_symbs, graph);
    MY_ASSERT (DPar_status != ERROR, "Parser ()", FUNC_ERROR, NULL);

    free (buffer);
    Token_Arr_Dtor (token_arr, n_tokens);

    return graph;
}

static struct Graph *Graph_Ctor (void)
{
    struct Graph *graph = (struct Graph *)calloc (1, sizeof (struct Graph));

    graph->line_type = DEFAULT;
    graph->approx_pow = -1;
    graph->x_acc = -1;
    graph->y_acc = -1;

    return graph;
}

void Graph_Dtor (struct Graph *graph)
{
    free (graph->title);
    free (graph->function);
    free (graph->dot_label);
    free (graph->dot_colour);
    free (graph->line_colour);
    free (graph->x_arr);
    free (graph->x_err);
    free (graph->x_title);
    free (graph->y_arr);
    free (graph->y_err);
    free (graph->y_title);
    free (graph->img_name);

    free (graph);
}

static void Token_Arr_Dtor (struct D_Token *token_arr, const int n_tokens)
{
    for (int token_i = 0; token_i < n_tokens; token_i++)
    {
        if (token_arr[token_i].name == STR)
            free (token_arr[token_i].val.str);
    }

    free (token_arr);
}

// ========================== PREPROCESSOR ========================== //

static int D_Preprocessor (char *buffer, const long n_symbs)
{
    bool in_string = false;
    
    for (long symb_i = 0L; symb_i < n_symbs; symb_i++)
    {
        if (buffer[symb_i] == '\"')
            in_string = (in_string == true) ? false : true;
        
        if (in_string == false && buffer[symb_i] == '/')
        {
            if (symb_i + 1 < n_symbs && buffer[symb_i + 1] == '/')
            {
                while (buffer[symb_i] != '\n')
                    buffer[symb_i++] = ' ';
            }
            else
            {
                Show_Error (buffer, n_symbs, symb_i, "Incorrect symbol");
                return ERROR;
            }
        }
    }

    return NO_ERRORS;
}

static void Show_Error (const char *buffer, const long n_symbs, const long err_symb_i, const char *err_descr)
{
    printf ("************ ERROR REPORT ************\n");

    long symb_i = err_symb_i;
    while (symb_i > 0 && buffer[symb_i] != '\n')
        symb_i--;
    symb_i++;
    
    int n_lines = 0;
    for (long i = 0; i < symb_i; i++)
    {
        if (buffer[i] == '\n')
            n_lines++;
    }

    printf ("LINE %d:\n", n_lines);
    
    for (long i = 0; symb_i + i < n_symbs && buffer[symb_i + i] != '\n'; i++)
        printf ("%c", buffer[symb_i + i]);
    printf ("\n");
    
    for (long i = symb_i; i < err_symb_i; i++)
        printf (" ");

    printf ("^~~~~~~~~~ %s\n", err_descr);

    printf ("**************************************\n");
}

// ================================================================== //

// ============================= LEXER ============================== //

static struct D_Token *D_Lexer (const char *buffer, const long n_symbs, int *n_tokens)
{
    MY_ASSERT (buffer, "const char *buffer", NULL_PTR, NULL);

    struct D_Token *token_arr = (struct D_Token *)calloc (n_symbs, sizeof (struct D_Token));
    int token_i = 0;

    for (long symb_i = 0L; symb_i < n_symbs; )
    {        
        Skip_Spaces (buffer, n_symbs, &symb_i);

        if (symb_i >= n_symbs)
            break;

        int GT_status = Get_Token (token_arr, token_i, buffer, n_symbs, &symb_i);
        if (GT_status == ERROR)
        {
            free (token_arr);
            MY_ASSERT (false, "Get_Token ()", FUNC_ERROR, NULL);
        }  
        token_i++;
    }

    *n_tokens = token_i;

    return token_arr;
}

static inline void Skip_Spaces (const char *buffer, const long n_symbs, long *symb_i)
{
    while (*symb_i < n_symbs && isspace (buffer[*symb_i]))
        (*symb_i)++;
}

static int Get_Token (struct D_Token *token_arr, const int token_i, const char *const buffer, const long n_symbs, long *symb_i)
{
    if (('a' <= buffer[*symb_i] && buffer[*symb_i] <= 'z') ||
        ('A' <= buffer[*symb_i] && buffer[*symb_i] <= 'Z'))
    {
        int GLN_status = Get_Label_Name (token_arr, token_i, buffer, n_symbs, symb_i);
        MY_ASSERT (GLN_status != ERROR, "Get_Label_Name ()", FUNC_ERROR, ERROR);
    }
    else if (buffer[*symb_i] == '\"')
    {
        (*symb_i)++;
        
        int GS_status = Get_String (token_arr, token_i, buffer, n_symbs, symb_i);
        MY_ASSERT (GS_status != ERROR, "Get_String ()", FUNC_ERROR, ERROR);
    }
    else if (buffer[*symb_i] == '-' || ('0' <= buffer[*symb_i] && buffer[*symb_i] <= '9'))
    {
        int GN_status = Get_Number (token_arr, token_i, buffer, n_symbs, symb_i);
        MY_ASSERT (GN_status != ERROR, "Get_Number ()", FUNC_ERROR, ERROR);
    }
    else
    {
        Show_Error (buffer, n_symbs, *symb_i, "Not allowed type of argument");
        return ERROR;
    }

    return NO_ERRORS;
}

static int Get_Label_Name (struct D_Token *token_arr, const int token_i, const char *const buffer, const long n_symbs, long *symb_i)
{   
    for (size_t label_i = 0; label_i < N_LABELS; label_i++)
    {       
        size_t label_name_len = strlen (Labels_Arr[label_i].name);
        
        if (strncmp (buffer + *symb_i, Labels_Arr[label_i].name, label_name_len) == 0)
        {
            *symb_i += label_name_len;

            if (*symb_i < n_symbs && buffer[*symb_i] == ':')
            {
                (*symb_i)++;
                token_arr[token_i].name = label_i;
                token_arr[token_i].buff_pos = *symb_i - sizeof (Labels_Arr[label_i].name);
                return NO_ERRORS;
            }
            else
            {
                Show_Error (buffer, n_symbs, *symb_i, "Missing colon");
                return ERROR;
            }
        }
    }

    Show_Error (buffer, n_symbs, *symb_i, "Unknown label");
    return ERROR;
}

static int Get_String (struct D_Token *token_arr, const int token_i, const char *const buffer, const long n_symbs, long *symb_i)
{       
    int str_len = 0;
    while (*symb_i + str_len < n_symbs && buffer[*symb_i + str_len] != '\"')
        str_len++;

    if (str_len == 0)
    {
        Show_Error (buffer, n_symbs, *symb_i + 1, "Empty strings are not allowed");
        return ERROR;
    }
    else
    {
        char *str = (char *)calloc (str_len + 1, sizeof (char));
        memcpy (str, buffer + *symb_i, str_len);

        token_arr[token_i].name = STR;
        token_arr[token_i].val.str = str;
        token_arr[token_i].buff_pos = *symb_i;

        *symb_i += str_len + 1;

        return NO_ERRORS;
    }
}

static int Get_Number (struct D_Token *token_arr, const int token_i, const char *const buffer, const long n_symbs, long *symb_i)
{    
    int num = 0;

    token_arr[token_i].buff_pos = *symb_i;

    bool positive = true;
    if (buffer[*symb_i] == '-')
    {
        positive = false;
        (*symb_i)++;
    }
    
    if ('0' <= buffer[*symb_i] && buffer[*symb_i] <= '9')
    {
        while ('0' <= buffer[*symb_i] && buffer[*symb_i] <= '9')
        {
            num = num * 10 + (buffer[*symb_i] - '0');
            (*symb_i)++;
        }
    }
    else
    {
        Show_Error (buffer, n_symbs, *symb_i, "Minus without a number is not allowed");
        return ERROR;
    }

    if (buffer[*symb_i] == '.')
    {
        double fp_num = (double)num;
        
        (*symb_i)++;
        const char *old_str = buffer + *symb_i;

        int degree = -1;

        while ('0' <= buffer[*symb_i] && buffer[*symb_i] <= '9')
        {
            fp_num += (buffer[*symb_i] - '0') * pow (10, degree);
            degree--;
            (*symb_i)++;
        }

        if (old_str != buffer + *symb_i)
        {
            token_arr[token_i].name = FP_NUM;
            token_arr[token_i].val.fp_num = (positive == true) ? fp_num : -fp_num;
        }
        else
        {
            Show_Error (buffer, n_symbs, *symb_i, "No digits after the decimal point");
            return ERROR;
        }
    }
    else
    {
        token_arr[token_i].name = INT_NUM;
        token_arr[token_i].val.int_num = (positive == true) ? num : -num;
    }

    return NO_ERRORS;
}

// ================================================================================================================================== //

// ============================================================= PARSER ============================================================= //

static int D_Parser (const struct D_Token *token_arr, const int n_tokens, const char *buffer, const long n_symbs, struct Graph *graph)
{
    struct Values values = {};

    int OLC_status = 0;
    
    for (int token_i = 0; token_i < n_tokens; token_i++)
    {       
        switch (token_arr[token_i].name)
        {
            case X_ASIX_DATA:
                graph->x_arr = (double *)calloc (n_tokens, sizeof (double));
                OLC_status = One_Label_Check (token_arr, token_i, buffer, n_symbs);
                MY_ASSERT (OLC_status != ERROR, "One_Label_Check ()", FUNC_ERROR, ERROR);
                break;

            case X_ERR:
                graph->x_err = (double *)calloc (n_tokens, sizeof (double));
                OLC_status = One_Label_Check (token_arr, token_i, buffer, n_symbs);
                MY_ASSERT (OLC_status != ERROR, "One_Label_Check ()", FUNC_ERROR, ERROR);
                break;

            case Y_ASIX_DATA:
                graph->y_arr = (double *)calloc (n_tokens, sizeof (double));
                OLC_status = One_Label_Check (token_arr, token_i, buffer, n_symbs);
                MY_ASSERT (OLC_status != ERROR, "One_Label_Check ()", FUNC_ERROR, ERROR);
                break;
            
            case NO_LINE:
                graph->line_type = DOTS;
                OLC_status = One_Label_Check (token_arr, token_i, buffer, n_symbs);
                MY_ASSERT (OLC_status != ERROR, "One_Label_Check ()", FUNC_ERROR, ERROR);
                break;

            case GRAPH_TITLE:
            case FUNCTION:
            case DOT_LABEL:
            case DOT_COLOUR:
            case LINE_COLOUR:
            case APPROX_POW:
            case X_ACCURACY:
            case X_TITLE:
            case Y_ACCURACY:
            case Y_TITLE:
            case IMG_NAME:
                OLC_status = One_Label_Check (token_arr, token_i, buffer, n_symbs);
                MY_ASSERT (OLC_status != ERROR, "One_Label_Check ()", FUNC_ERROR, ERROR);
                break;

            case INT_NUM:
            {
                int HIN_status = Handle_Int_Num (graph, token_arr, token_i, buffer, n_symbs);
                MY_ASSERT (HIN_status != ERROR, "Handle_Int_Num ()", FUNC_ERROR, ERROR);
                break;
            } 

            case FP_NUM:
            {
                int HFPN_status = Handle_FP_Num (graph, token_arr, token_i, buffer, n_symbs, &values);
                MY_ASSERT (HFPN_status != ERROR, "Handle_FP_Num ()", FUNC_ERROR, ERROR);
                break;
            }

            case STR:
            {               
                int HS_status = Handle_String (graph, token_arr, token_i, buffer, n_symbs);
                MY_ASSERT (HS_status != ERROR, "Handle_String ()", FUNC_ERROR, ERROR);
                break;
            }

            default:
                MY_ASSERT (false, "token_arr[token_i].name", UNEXP_VAL, ERROR);
        }
    }

    #ifdef PARSER_DUMP
    Parser_Dump (token_arr, n_tokens);
    #endif

    int CG_status = Check_Graph (graph, &values);
    MY_ASSERT (CG_status != ERROR, "Check_Graph ()", FUNC_ERROR, ERROR);

    Set_Defaults (graph);
    
    return NO_ERRORS;
}

static int One_Label_Check (const struct D_Token *token_arr, const int token_i, const char *buffer, const long n_symbs)
{
    for (int i = 0; i < token_i; i++)
    {
        if (token_arr[i].name == token_arr[token_i].name)
        {
            size_t err_str_len = sizeof ("Only one \"") + sizeof ("\" label is allowed") + strlen (Labels_Arr[token_arr[i].name].name);
            char *err_str = (char *)calloc (err_str_len, sizeof (char));
            sprintf (err_str, "Only one \"%s\" label is allowed\n", Labels_Arr[token_arr[i].name].name);
            
            Show_Error (buffer, n_symbs, token_arr[token_i].buff_pos, err_str);
            free (err_str);
            return ERROR;
        }
    }

    return NO_ERRORS;
}

static int Handle_Int_Num (struct Graph *graph, const struct D_Token *token_arr, const int token_i, const char *buffer, const long n_symbs)
{
    if (token_i > 0)
    {
        switch (token_arr[token_i - 1].name)
        {
            case APPROX_POW:
                graph->approx_pow = token_arr[token_i].val.int_num;
                break;
            case X_ACCURACY:
                graph->x_acc = token_arr[token_i].val.int_num;
                break;
            case Y_ACCURACY:
                graph->y_acc = token_arr[token_i].val.int_num;
                break;

            default:
                Show_Error (buffer, n_symbs, token_arr[token_i].buff_pos, "Previous token should be \"X_Accurace\" or \"Y_Accuracy\" or an integer number");
                return ERROR;
        }
    }
    else
    {
        Show_Error (buffer, n_symbs, token_arr[0].buff_pos, "First token cannot be a number");
        return ERROR;
    }

    return NO_ERRORS;
}

static int Handle_FP_Num (struct Graph *graph, const struct D_Token *token_arr, const int token_i, const char *buffer, const long n_symbs, struct Values *values)
{
    if (token_i > 0)
    {
        switch (token_arr[token_i - 1].name)
        {
            case X_ASIX_DATA:
                graph->x_arr[values->x++] = token_arr[token_i].val.fp_num;
                break;
            case X_ERR:
                graph->x_err[values->x_err++] = token_arr[token_i].val.fp_num;
                break;
            case Y_ASIX_DATA:
                graph->y_arr[values->y++] = token_arr[token_i].val.fp_num;
                break;
            case FP_NUM:
                for (int i = token_i - 2; token_arr[i].name == FP_NUM; i--)
                {
                    switch (token_arr[i].name)
                    {
                        case X_ASIX_DATA:
                            graph->x_arr[values->x++] = token_arr[i].val.fp_num;
                            break;
                        case X_ERR:
                            graph->x_err[values->x_err++] = token_arr[i].val.fp_num;
                            break;
                        case Y_ASIX_DATA:
                            graph->y_arr[values->y++] = token_arr[i].val.fp_num;
                            break;
                        case FP_NUM:
                        default:
                            break;
                    }
                }
                break;

            default:
                Show_Error (buffer, n_symbs, token_arr[token_i].buff_pos, "Previous token should be \"X_Asix_Data\" or \"Y_Asix_Data\" or a floating point num");
                return ERROR;
        }
    }
    else
    {
        Show_Error (buffer, n_symbs, token_arr[0].buff_pos, "First token cannot be a number");
        return ERROR;
    }

    return NO_ERRORS;
}

static int Handle_String (struct Graph *graph, const struct D_Token *token_arr, const int token_i, const char *buffer, const long n_symbs)
{
    size_t str_len = strlen (token_arr[token_i].val.str);
                
    if (token_i > 0)
    {
        switch (token_arr[token_i - 1].name)
        {
            case GRAPH_TITLE:
                graph->title = (char *)calloc (str_len + 1, sizeof (char));
                memcpy (graph->title, token_arr[token_i].val.str, str_len);
                break;
            case FUNCTION:
                graph->function = (char *)calloc (str_len + 1, sizeof (char));
                memcpy (graph->function, token_arr[token_i].val.str, str_len);
                break;
            case DOT_LABEL:
                graph->dot_label = (char *)calloc (str_len + 1, sizeof (char));
                memcpy (graph->dot_label, token_arr[token_i].val.str, str_len);
                break;
            case DOT_COLOUR:
                graph->dot_colour = (char *)calloc (str_len + 1, sizeof (char));
                memcpy (graph->dot_colour, token_arr[token_i].val.str, str_len);
                break;  
            case LINE_COLOUR:
                graph->line_colour = (char *)calloc (str_len + 1, sizeof (char));
                memcpy (graph->line_colour, token_arr[token_i].val.str, str_len);
                break;
            case X_TITLE:
                graph->x_title = (char *)calloc (str_len + 1, sizeof (char));
                memcpy (graph->line_colour, token_arr[token_i].val.str, str_len);
                break;
            case Y_TITLE:
                graph->y_title = (char *)calloc (str_len + 1, sizeof (char));
                memcpy (graph->line_colour, token_arr[token_i].val.str, str_len);
                break;
            case IMG_NAME:
                graph->img_name = (char *)calloc (str_len + 1, sizeof (char));
                memcpy (graph->img_name, token_arr[token_i].val.str, str_len);
                break;

            default:
                Show_Error (buffer, n_symbs, token_arr[token_i - 1].buff_pos, "This label cannot have a string argument");
                return ERROR;
        }
    }
    else
    {
        Show_Error (buffer, n_symbs, token_arr[0].buff_pos, "First token cannot be a string");
        return ERROR;
    }

    return NO_ERRORS;
}

static int Check_Graph (struct Graph *graph, const struct Values *values)
{      
    if (graph->title == NULL)
    {
        printf ("************ ERROR REPORT ************\n");
        printf ("\"Graph_Title\" label is forgotten\n");
        printf ("**************************************\n");
        return ERROR;
    }
    else if (graph->function == NULL)
    {
        printf ("************ ERROR REPORT ************\n");
        printf ("\"Function\" label is forgotten\n");
        printf ("**************************************\n");
        return ERROR;
    }
    else if (graph->dot_label == NULL)
    {
        printf ("************ ERROR REPORT ************\n");
        printf ("\"Dot_Label\" label is forgotten\n");
        printf ("**************************************\n");
        return ERROR;
    }
    else if (graph->approx_pow == -1)
    {
        printf ("************ ERROR REPORT ************\n");
        printf ("\"Approximation_Power\" label is forgotten\n");
        printf ("**************************************\n");
        return ERROR;
    }
    else if (graph->x_arr == NULL)
    {
        printf ("************ ERROR REPORT ************\n");
        printf ("\"X_Asix_Data\" label is forgotten\n");
        printf ("**************************************\n");
        return ERROR;
    }
    else if (graph->x_err == NULL)
    {
        printf ("************ ERROR REPORT ************\n");
        printf ("\"X_Error\" label is forgotten\n");
        printf ("**************************************\n");
        return ERROR;
    }
    else if (graph->x_acc == -1)
    {
        printf ("************ ERROR REPORT ************\n");
        printf ("\"X_Accuracy\" label is forgotten\n");
        printf ("**************************************\n");
        return ERROR;
    }
    else if (graph->x_title == NULL)
    {
        printf ("************ ERROR REPORT ************\n");
        printf ("\"X_Title\" label is forgotten\n");
        printf ("**************************************\n");
        return ERROR;
    }
    else if (graph->y_arr == NULL)
    {
        printf ("************ ERROR REPORT ************\n");
        printf ("\"Y_Asix_Data\" label is forgotten\n");
        printf ("**************************************\n");
        return ERROR;
    }
    else if (graph->y_acc == -1)
    {
        printf ("************ ERROR REPORT ************\n");
        printf ("\"Y_Accuracy\" label is forgotten\n");
        printf ("**************************************\n");
        return ERROR;
    }
    else if (graph->y_title == NULL)
    {
        printf ("************ ERROR REPORT ************\n");
        printf ("\"Y_Title\" label is forgotten\n");
        printf ("**************************************\n");
        return ERROR;
    }
    else if (graph->img_name == NULL)
    {
        printf ("************ ERROR REPORT ************\n");
        printf ("\"Image_Name\" label is forgotten\n");
        printf ("**************************************\n");
        return ERROR;
    }
    else if (graph->approx_pow + 2 <= graph->n_dots)
    {
        printf ("************ ERROR REPORT ************\n");
        printf ("\"Approximation_Power\" has to be 2 more than number of dots or they have to be equal\n");
        printf ("**************************************\n");
        return ERROR;
    }

    if (values->x != values->y || values->x != values->x_err)
    {       
        printf ("The number of \"X_Asix_Data\" and \"Y_Asix_Data\" and \"X_Error\" arguments should be equal\n");
        return ERROR;
    }
    else
        graph->n_dots = values->x;

    return NO_ERRORS;
}

static void Set_Defaults (struct Graph *graph)
{
    if (graph->dot_colour == NULL)
    {
        graph->img_name = (char *)calloc (sizeof ("green"), sizeof (char));
        memcpy (graph->img_name, "green", sizeof ("green"));
    }

    if (graph->line_type == DEFAULT)
        graph->line_type = POLINOMICAL;

    if (graph->line_colour == NULL)
    {
        graph->line_colour = (char *)calloc (sizeof ("red"), sizeof (char));
        memcpy (graph->line_colour, "red", sizeof ("red"));
    }
}

#ifdef PARSER_DUMP
static void Parser_Dump (const struct D_Token *token_arr, const int n_tokens)
{
    for (int token_i = 0; token_i < n_tokens; token_i++)
    {
        switch (token_arr[token_i].name)
        {
            case GRAPH_TITLE:
                printf ("GRAPH_TITLE\n");
                break;

            case FUNCTION:
                printf ("FUNCTION\n");
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

            case APPROX_POW:
                printf ("APPROX_POW\n");
                break;

            case X_ASIX_DATA:
                printf ("X_ASIX_DATA\n");
                break;

            case X_ERR:
                printf ("X_ERR\n");
                break;

            case X_ACCURACY:
                printf ("X_ACCURACY\n");
                break;

            case X_TITLE:
                printf ("X_TITLE\n");
                break;

            case Y_ASIX_DATA:
                printf ("Y_ASIX_DATA\n");
                break;

            case Y_ACCURACY:
                printf ("Y_ACCURACY\n");
                break;

            case Y_TITLE:
                printf ("Y_TITLE\n");
                break;

            case IMG_NAME:
                printf ("IMG_NAME\n");
                break;

            case INT_NUM:
                printf ("INT_NUM: %d\n", token_arr[token_i].val.int_num);
                break;

            case FP_NUM:
                printf ("FP_NUM: %f\n", token_arr[token_i].val.fp_num);
                break;

            case STR:
                printf ("STR: %s\n", token_arr[token_i].val.str);
                break;
            
            default:
                break;
        }
    }
}
#endif

// ================================================================================================================================== //
