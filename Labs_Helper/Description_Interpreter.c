#include "Labs.h"
#include "../C/My_Lib/My_Lib.h"

enum Labels
{
    GRAPH_TITLE,

    N_DOTS,
    DOT_LABEL,
    DOT_COLOUR,

    LINE_LABEL,
    LINE_COLOUR,
    
    X_ASIX_DATA,
    X_ACCURACY,
    X_TITLE,

    Y_ASIX_DATA,
    Y_ACCURACY,
    Y_TITLE,

    FILE_NAME
};

static const char *labels_arr[] = 
{
    "Graph_Title",
    "N_Dots",
    "Dot_Lable",
    "Dot_Colour",
    "Line_Label",
    "Line_Colour",
    "X_Asix_Data",
    "X_Accuracy",
    "X_Title",
    "Y_Asix_Data",
    "Y_Accuracy",
    "Y_Title",
    "File_Name"
};

enum Token
{
    STRING, 
    INT_NUM,
    DOUBLE_NUM,
};

union Token
{
    char *str;
    int i_num;
    double d_num;
};

struct Token
{
    union Token value;
    enum  Token type;
};

static int Delete_Comments (char *buffer, const long n_symbs)
{
    bool in_string = false;
    
    for (long symb_i = 0L; symb_i < n_symbs; symb_i++)
    {
        if (buffer[symb_i] == '\"')
            in_string = (in_string == true) ? false : true;
        
        if (in_string == false && buffer[symb_i] == '/')
        {
            if (symb_i + 1 < n_symbs && buffer[symb_i + 1] = '/')
                while (buffer[symb_i] != '\n')
                    buffer[symb_i] == ' ';
            else
                return ERROR; // <=== Сделай дамп в лог файл, как в процессоре
        }
    }

    return NO_ERRORS;
}

static char *Preprocessor (const char *file_name)
{
    FILE *file = Open_File (file_name, "rb");

    long n_symbs = Define_File_Size (file);
    MY_ASSERT (n_symbs != ERROR, "Define_File_Size ()", FUNC_ERROR, NULL);

    char *buffer = Make_Buffer (file, n_symbs);
    MY_ASSERT (buffer, "Make_Buffer ()", FUNC_ERROR, NULL);

    Close_File (file, file_name);

    if (Delete_Comments (buffer, n_symbs) == ERROR)
        MY_ASSERT (false, "Delete_Comments ()", FUNC_ERROR, NULL);

    return buffer;
}

static void Skip_Spaces

static struct Token *Lexer (const char *buffer, const long n_symbs)
{
    long couter = 0L;
    char temp_buff[n_symbs] = "";
    
    for (long symb_i = 0L; symb_i < n_symbs; symb_i++)
    {
        if (isspace (symb_i))
        {
            Check_Buffer (temp_buff);
        }
        else
            temp_buff[counter] = buffer[symb_i];
        
    }
}

static struct Graph *Parser (struct Token *tokens_arr, const long n_tokens)
{
    struct Graph *graph = (struct Graph *)calloc (1, sizeof (struct Graph));
    MY_ASSERT (graph, "struct Graph *graph", NE_MEM, NULL);
}

struct Graph *Labs_Main (const char *file_name)
{
    MY_ASSERT (file_name, "const char *file_name", NULL_PTR, NULL);

    char *buffer = Preprocessor (file_name);
    MY_ASSERT (buffer, "Preprocessor ()", FUNC_ERROR, NULL);

    Lexer (buffer, n_symbs);

    struct Graph *graph = Parser (tokens_arr, n_tokens);

    return graph;
}