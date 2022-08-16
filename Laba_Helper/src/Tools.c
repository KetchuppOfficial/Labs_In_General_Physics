#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "Tools.h"

struct Buffer Open_File (void)
{
    bool error_flag = true;
    struct Buffer buffer = {};
    
    while (error_flag)
    {
        printf ("Type the name of file specifying the directory: ");

        char *file_name = Get_String ();
        if (file_name == NULL)
            continue;
        else
        {
            buffer = File_To_Buffer (file_name);
            free (file_name);
            if (buffer.str == NULL && buffer.n_symbs == 0)
            {
                printf (" Please, try again.\n\n");
                continue;
            }
            else
                error_flag = false;
        }
    }

    return buffer;
}

static char *Buffer_Stdin (void);

char *Get_String (void)
{
    char *buffer = Buffer_Stdin ();
    if (buffer == NULL)
        return NULL;

    if (feof (stdin))
    {
        printf ("\nEOF was reached. Please, try again.\n\n");
        clearerr (stdin);
        free (buffer);

        return NULL;
    }

    return buffer;
}

static char *Buffer_Stdin (void)
{
    size_t size = 50;
    char *buffer = (char *)Calloc_ (size, sizeof (char));
    if (buffer == NULL)
        return NULL;

    size_t char_i = 0;
    int symb = getchar ();
    while (symb != '\n' && symb != EOF)
    {
        if (char_i >= size)
        {
            buffer = Recalloc_ (buffer, size, size * 2);
            if (buffer == NULL)
                return NULL;

            size *= 2;
        }

        buffer[char_i++] = (char)symb;
        symb = getchar ();
    }

    buffer[char_i] = '\0';

    return buffer;
}

static char *Make_Buffer (FILE *file, const size_t n_symbs);

struct Buffer File_To_Buffer (const char *file_name)
{
    assert (file_name);
    
    struct Buffer buffer = {};
    
    int is_dir = Is_Directory (file_name);
    if (is_dir == error)
        return buffer;
    else if (is_dir)
    {
        printf ("\"%s\" is a directory, not a file.", file_name);
        return buffer;
    }

    FILE *file = fopen (file_name, "rb");
    if (file == NULL)
    {
        printf ("File \"%s\" does not exist or cannot be opened.", file_name);
        return buffer;
    }

    if ((buffer.n_symbs = Define_File_Size (file)) == 0)
    {
        printf ("File \"%s\" is empty.", file_name);
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

int Is_Directory (const char *path)
{
   struct stat statbuf;

    if (stat(path, &statbuf) != 0)
    {
        printf ("Calling of stat() was not successful\n");
        return error;
    }

    if (S_ISDIR(statbuf.st_mode))
        return error + 1; // positive value that is not equal to "error"
    else
        return 0;
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
                char *buffer = Buffer_Stdin ();

                if (feof (stdin))
                {
                    printf ("\n\"%s\" is not a number and EOF was reached. "
                            "Please, try again.\n\n", buffer);
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

                char *buffer = Buffer_Stdin ();

                if (feof (stdin))
                {
                    printf ("\n\"%d%s\" is not a number and EOF was reached. "
                            "Please, try again.\n\n", num, buffer);
                    clearerr (stdin);
                }
                else
                    printf ("\"%d%s\" is not a number. Please, try again.\n\n", 
                            num, buffer);

                free (buffer);
            }
        }
    }

    return num;
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
