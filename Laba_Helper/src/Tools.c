#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "Tools.h"

#define MAX_FILE_NAME 50

struct Buffer Open_File (void)
{
    bool error_flag = true;
    struct Buffer buffer = {};
    
    while (error_flag)
    {
        printf ("Type the name of file specifying the directory: ");

        char file_name[MAX_FILE_NAME] = "";

        if (Get_String (file_name, MAX_FILE_NAME) == NULL)
        {
            printf ("Try again.\n\n");
            clearerr (stdin);
            continue;
        }

        printf ("\n");

        buffer = File_To_Buffer (file_name);
        if (buffer.str == NULL && buffer.n_symbs == 0)
        {
            printf ("Try again.\n\n");
            continue;
        }
        else
            error_flag = false;
    }

    return buffer;
}

char *Get_String (char *str, const int n_symbs)
{
    assert (str);
    
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
        printf ("ferror() detected error in last reading from stdin\n");
        return NULL;
    }
    else if (feof (stdin))
    {
        printf ("EOF was reached\n");
        return NULL;
    }

    return ret_val;
}

static char *Make_Buffer (FILE *file, const size_t n_symbs);

struct Buffer File_To_Buffer (const char *file_name)
{
    assert (file_name);
    
    struct Buffer buffer = {};
    
    FILE *file = fopen (file_name, "rb");
    if (file == NULL)
    {
        printf ("File \"%s\" does not exist or cannot be opened.\n", file_name);
        return buffer;
    }

    if ((buffer.n_symbs = Define_File_Size (file)) == 0)
    {
        printf ("File \"%s\" is empty.\n", file_name);
        return buffer;
    }

    if ((buffer.str = Make_Buffer (file, buffer.n_symbs)) == NULL)
        return buffer;

    fclose (file);

    return buffer;
}

size_t Define_File_Size (FILE *file)
{
    assert (file);
    
    long start_pos = ftell (file);
    fseek (file, 0L, SEEK_END);
    long n_symbs = ftell (file);
    fseek (file, start_pos, SEEK_SET);

    return (size_t)n_symbs;
}

static char *Make_Buffer (FILE *file, const size_t n_symbs)
{
    assert (file);
    
    char *buffer = (char *)Calloc_ (n_symbs + 1, sizeof (char));
    if (buffer == NULL)
        return NULL;

    if (fread (buffer, sizeof (char), n_symbs, file) != n_symbs)
    {
        if (ferror (file))
        {
            printf ("ferror() detected error in last reading from stream %p\n", file);
            free (buffer);
            return NULL;
        }
        if (feof (file))
            printf ("WARNING!: EOF was reached before chunk with number %ld was read\n", n_symbs);
    }

    return buffer;
}

static char *Buffer_Extra_Symbs (void);

int Get_Int ()
{
    int num = 0;
    bool error = true;

    while (error)
    {
        printf ("Desired mode: ");

        if (scanf ("%d", &num) != 1)
        {
            if (feof (stdin))
            {
                printf ("\nEOF was reached. Please, try again.\n\n");
                clearerr (stdin);
            }
            else
            {
                char *buffer = Buffer_Extra_Symbs ();

                if (feof (stdin))
                {
                    printf ("\n\"%s\" is not a number and EOF was reached. Please, try again.\n\n", buffer);
                    clearerr (stdin);
                }
                else
                    printf ("\"%s\" is not a number. Please, try again.\n\n", buffer);
                
                free (buffer);
            }
        }
        else
        {
            int next_char = getchar ();

            if (next_char == '\n')
                error = false;
            else if (next_char == EOF)
            {
                printf ("\nEOF was reached. Please, try again.\n\n");
                clearerr (stdin);
            }
            else
            {
                ungetc (next_char, stdin);

                char *buffer = Buffer_Extra_Symbs ();

                if (feof (stdin))
                {
                    printf ("\n\"%d%s\" is not a number and EOF was reached. Please, try again.\n\n", num, buffer);
                    clearerr (stdin);
                }
                else
                    printf ("\"%d%s\" is not a number. Please, try again.\n\n", num, buffer);

                free (buffer);
            }
        }
    }

    return num;
}

static char *Buffer_Extra_Symbs (void)
{
    size_t size = 20;
    char *buffer = (char *)Calloc_ (size, sizeof (char));
    
    size_t char_i = 0;
    int extra_symb = getchar ();
    while (extra_symb != '\n' && extra_symb != EOF)
    {
        if (char_i >= size)
        {
            Recalloc_ (buffer, size, size * 2);
            size *= 2;
        }

        buffer[char_i++] = (char)extra_symb;
        extra_symb = getchar ();
    }

    return buffer;
}

void *Calloc_ (const size_t n_elems, const size_t elem_size)
{
    void *ptr = calloc (n_elems, elem_size);
    
    if (ptr == NULL)
        printf ("Heap exhaused\n");

    return ptr;
}

void *Recalloc_ (void *ptr, size_t old_size, size_t new_size)
{
    void *ret = realloc (ptr, new_size);

    if (ret == NULL)
        printf ("Heap exhausted\n");
    else
        memset (ret + old_size, 0, new_size - old_size);
    
    return ret;
}
