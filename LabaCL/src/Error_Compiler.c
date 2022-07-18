#include "My_Lib.h"
#include "../include/LabaCL.h"

enum Token_Enum
{
    FUNCTION,
    VAR,
    STR,
    NUM,
};

struct Label
{
    enum Token_Enum num;
    char *name;
};

#define N_LABELS (sizeof (Labels_Arr) / sizeof (struct Label))

union E_Value
{
    double num;
    char *str;
};

struct E_Token
{
    enum Token_Enum name;
    union E_Value val;
    size_t buff_pos;
};

static struct Graph   *Graph_Ctor     (void);
static void            Token_Arr_Dtor (struct E_Token *token_arr, const int n_tokens);

static int  D_Preprocessor (char *buffer, const long n_symbs);
static void Show_Error     (const char *buffer, const long n_symbs, const long err_symb_i, const char *err_descr);

static struct E_Token *E_Lexer        (const char *buffer, const long n_symbs, int *n_tokens);
static inline void     Skip_Spaces    (const char *buffer, const long n_symbs, long *symb_i);
static int             Get_Token      (struct E_Token *token_arr, const int token_i, const char *const buffer, const long n_symbs, long *symb_i);
static int             Get_Label_Name (struct E_Token *token_arr, const int token_i, const char *const buffer, const long n_symbs, long *symb_i);
static int             Get_String     (struct E_Token *token_arr, const int token_i, const char *const buffer, const long n_symbs, long *symb_i);
static int             Get_Number     (struct E_Token *token_arr, const int token_i, const char *const buffer, const long n_symbs, long *symb_i);

static int  E_Parser        (const struct E_Token *token_arr, const int n_tokens, const char *buffer, const long n_symbs, struct Graph *graph);
static int  One_Label_Check (const struct E_Token *token_arr, const int token_i, const char *buffer, const long n_symbs);
static int  Handle_FP_Num   (struct Graph *graph, const struct E_Token *token_arr, const int token_i, const char *buffer, const long n_symbs, struct Values *values);
static int  Handle_String   (struct Graph *graph, const struct E_Token *token_arr, const int token_i, const char *buffer, const long n_symbs);
static int  Check_Graph     (struct Graph *graph, const struct Values *values);
static void Set_Defaults    (struct Graph *graph);

#ifdef DI_PARSER_DUMP
static int Parser_Dump (const struct E_Token *token_arr, const int n_tokens);
#endif // DI_PARSER_DUMP

struct Graph *Error_Compiler (char *buffer, const long n_symbs)
{
    MY_ASSERT (buffer,      "const char *file_name", NULL_PTR, NULL);
    MY_ASSERT (n_symbs > 0, "const long n_symbs",    POS_VAL,  NULL);

    int DPrep_status = D_Preprocessor (buffer, n_symbs);
    MY_ASSERT (DPrep_status != ERROR, "Preprocessor ()", FUNC_ERROR, NULL);

    int n_tokens = 0;
    struct E_Token *token_arr = E_Lexer (buffer, n_symbs, &n_tokens);
    MY_ASSERT (token_arr, "Lexer ()", FUNC_ERROR, NULL);

    int DPar_status = E_Parser (token_arr, n_tokens, buffer, n_symbs, graph);
    MY_ASSERT (DPar_status != ERROR, "Parser ()", FUNC_ERROR, NULL);

    Token_Arr_Dtor (token_arr, n_tokens);

    return graph;
}

static void Token_Arr_Dtor (struct E_Token *token_arr, const int n_tokens)
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

static struct E_Token *E_Lexer (const char *buffer, const long n_symbs, int *n_tokens)
{
    MY_ASSERT (buffer, "const char *buffer", NULL_PTR, NULL);

    struct E_Token *token_arr = (struct E_Token *)calloc (n_symbs, sizeof (struct E_Token));
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

static int Get_Token (struct E_Token *token_arr, const int token_i, const char *const buffer, const long n_symbs, long *symb_i)
{
    if (('a' <= buffer[*symb_i] && buffer[*symb_i] <= 'z') ||
        ('A' <= buffer[*symb_i] && buffer[*symb_i] <= 'Z'))
    {
        long temp = 0;
        while (*symb_i + temp < n_symbs && buffer[*symb_i + temp] != '=' && buffer[*symb_i + temp] != ':')
            temp++;

        if (*symb_i + temp >= n_symbs)
        {
            Show_Error (buffer, n_symbs, *symb_i, "Unknown token");
            return ERROR;
        }
        else if (buffer[*symb_i + temp] == '=')
        {
            int GVN_status = Get_Var_Name (token_arr, token_i, buffer, n_symbs, symb_i);
            MY_ASSERT (GVN_status != ERROR, "Get_Var_Name ()", FUNC_ERROR, ERROR);
        }
        else if (buffer[*symb_i + temp] == ':')
        {
            int GLN_status = Get_Label_Name (token_arr, token_i, buffer, n_symbs, symb_i);
            MY_ASSERT (GLN_status != ERROR, "Get_Label_Name ()", FUNC_ERROR, ERROR);
        }
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

static int Get_Var_Name (struct E_Token *token_arr, const int token_i, const char *const buffer, const long n_symbs, long *symb_i)
{
    
}

static int Get_Label_Name (struct E_Token *token_arr, const int token_i, const char *const buffer, const long n_symbs, long *symb_i)
{   
    int str_len = 0;
    while (*symb_i + str_len < n_symbs && buffer[*symb_i + str_len] != ':')
        str_len++;

    if (*symb_i + str_len >= n_symbs)
    {
        Show_Error (buffer, n_symbs, *symb_i, "Missing colon");
        return ERROR;
    }
    
    if (strncmp (buffer + *symb_i, "Function", sizeof ("Function") - 1) == 0)
    {
        token_arr[token_i].name = FUNCTION;
    }
    else
    {
        char *str = (char *)calloc (str_len + 1, sizeof (char));
        memcpy (str, buffer + *symb_i, str_len);

        token_arr[token_i].name = VAR;
        token_arr[token_i].val.str = str;
    }

    token_arr[token_i].buff_pos = *symb_i;
    *symb_i += str_len + 1;

    return NO_ERRORS;
}

static int Get_String (struct E_Token *token_arr, const int token_i, const char *const buffer, const long n_symbs, long *symb_i)
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

static int Get_Number (struct E_Token *token_arr, const int token_i, const char *const buffer, const long n_symbs, long *symb_i)
{    
    double num = 0.0;
    
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
        (*symb_i)++;
        const char *old_str = buffer + *symb_i;

        int degree = -1;

        while ('0' <= buffer[*symb_i] && buffer[*symb_i] <= '9')
        {
            num += (buffer[*symb_i] - '0') * pow (10, degree);
            degree--;
            (*symb_i)++;
        }

        if (old_str != buffer + *symb_i)
        {
            token_arr[token_i].name = NUM;
            token_arr[token_i].val.num = num;
        }
        else
        {
            Show_Error (buffer, n_symbs, *symb_i, "No digits after the decimal point");
            return ERROR;
        }
    }
    else
    {
        token_arr[token_i].name = NUM;
        token_arr[token_i].val.num = num;
    }

    return NO_ERRORS;
}

// ================================================================================================================================== //

// ============================================================= PARSER ============================================================= //

static struct Node *E_Parser (const struct E_Token *token_arr, const int n_tokens, const char *buffer, const long n_symbs)
{
    Handle_Function (token_arr, n_tokens, buffer, n_symbs);

    int OLC_status = 0;
    
    for (int token_i = 0; token_i < n_tokens; token_i++)
    {       
        switch (token_arr[token_i].name)
        {
            case FUNCTION:
                token_i += 2;
                break;

            case NUM:
            {
                int HFPN_status = Handle_Num (token_arr, token_i, buffer, n_symbs);
                MY_ASSERT (HFPN_status != ERROR, "Handle_FP_Num ()", FUNC_ERROR, ERROR);
                break;
            }

            case STR:
                Show_Error (buffer, n_symbs, token_arr[token_i].buff_pos, "A string can follow only \"Function\" label");
                return ERROR;        
    
            default:
                MY_ASSERT (false, "token_arr[token_i].name", UNEXP_VAL, ERROR);
        }
    }
    
    return NO_ERRORS;
}

static struct Forest *Handle_Function (const struct E_Token *token_arr, const int n_tokens, const char *buffer, const long n_symbs)
{
    struct Forest *forest = NULL;

    bool function_found = false;

    for (int token_i = 0; token_i < n_tokens; token_i++)
    {
        if (token_arr[token_i].name == FUNCTION)
        {
            if (function_found)
            {
                Show_Error (buffer, n_symbs, token_arr[token_i].buff_pos, "Only one \"Function\" label is allowed");
                return ERROR;
            }
            else
                function_found = true;

            token_i++;

            if (token_i < n_tokens && token_arr[token_i].name == STR)
            {
                struct Node *tree = Plant_Tree (token_arr[token_i].val.str, strlen (token_arr[token_i].val.str));
                if (!tree)
                {
                    Show_Error (buffer, n_symbs, token_arr[token_i].buff_pos, "Error in parsing funtion. Look in the log file");
                    MY_ASSERT (false, "Plant_Tree ()", FUNC_ERROR, NULL);
                }

                forest = Forest_Ctor (tree);

                Match_Vars (token_arr, n_tokens, forest);
            }
            else
            {
                Show_Error (buffer, n_symbs, token_arr[token_i - 1].buff_pos, "\"Function\" label has to be followed by a string");
                return ERROR;
            }
        }
        
    }

    if (!function_found)
    {
        printf ("************ ERROR REPORT ************\n"
                "\"Function\" label is forgotten\n"
                "**************************************\n");

        return ERROR;
    }

    return forest;
}

static int Match_Vars (const struct E_Token *token_arr, const int n_tokens, struct Forest *forest)
{
    for (int token_i = 0; token_i < n_tokens; token_i++)
    {
        if (token_arr[token_i].name == VAR)
        {
            for (int var_i = 0; var_i < forest->n_vars; var_i++)
            {
                if (strncmp (token_arr[token_i].val.str, forest->vars_arr->name))
            }
        }
    }
}

static int Handle_Num (const struct E_Token *token_arr, const int token_i, const char *buffer, const long n_symbs)
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
            case Y_ERR:
                graph->y_err[values->y_err++] = token_arr[token_i].val.fp_num;
                break;
            case FP_NUM:
            {
                int i = token_i - 2;

                while (token_arr[i].name == FP_NUM)
                    i--;  

                switch (token_arr[i].name)
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
                    case Y_ERR:
                        graph->y_err[values->y_err++] = token_arr[token_i].val.fp_num;
                        break;
                    default:
                        Show_Error (buffer, n_symbs, token_arr[i].buff_pos, "This token cannot have floating point number as argument");
                }

                break;
            }
                    
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

static int Check_Graph (struct Graph *graph, const struct Values *values)
{      
    if (graph->title == NULL)
    {
        printf ("************ ERROR REPORT ************\n");
        printf ("\"Graph_Title\" label is forgotten\n");
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

    #ifdef POLINOMICAL_APPROX
    else if (graph->approx_pow == -1)
    {
        printf ("************ ERROR REPORT ************\n");
        printf ("\"Approximation_Power\" label is forgotten\n");
        printf ("**************************************\n");
        return ERROR;
    }
    #endif // POLINOMICAL_APPROX

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
    else if (graph->y_err == NULL)
    {
        printf ("************ ERROR REPORT ************\n");
        printf ("\"Y_Error\" label is forgotten\n");
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
    #ifdef POLINOMICAL_APPROX
    else if (graph->approx_pow + 2 <= graph->n_dots)
    {
        printf ("************ ERROR REPORT ************\n");
        printf ("\"Approximation_Power\" has to be 2 more than number of dots or they have to be equal\n");
        printf ("**************************************\n");
        return ERROR;
    }
    #endif // POLINOMICAL_APPROX

    if (values->x != values->y || values->x != values->x_err || values->y != values->y_err)
    {       
        printf ("The number of \"X_Asix_Data\", \"Y_Asix_Data\", \"X_Error\" and \"Y_Error\" arguments should be equal\n");
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
        graph->dot_colour = (char *)calloc (sizeof ("blue"), sizeof (char));
        memcpy (graph->dot_colour, "blue", sizeof ("blue"));
    }

    if (graph->line_colour == NULL)
    {
        graph->line_colour = (char *)calloc (sizeof ("green"), sizeof (char));
        memcpy (graph->line_colour, "green", sizeof ("green"));
    }

    if (graph->err_colour == NULL)
    {
        graph->err_colour = (char *)calloc (sizeof ("red"), sizeof (char));
        memcpy (graph->err_colour, "red", sizeof ("red"));
    }

    if (graph->line_type == DEFAULT)
        graph->line_type = POLINOMICAL;
}

#ifdef DI_PARSER_DUMP
static int Parser_Dump (const struct E_Token *token_arr, const int n_tokens)
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

            case X_ASIX_DATA:
                printf ("X_ASIX_DATA\n");
                break;

            case X_ERR:
                printf ("X_ERR\n");
                break;

            case X_TITLE:
                printf ("X_TITLE\n");
                break;

            case Y_ASIX_DATA:
                printf ("Y_ASIX_DATA\n");
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
                MY_ASSERT (false, "token_arr[token_i].name", UNEXP_VAL, ERROR);
                break;
        }
    }

    return NO_ERRORS;
}
#endif // DI_PARSER_DUMP

// ================================================================================================================================== //
