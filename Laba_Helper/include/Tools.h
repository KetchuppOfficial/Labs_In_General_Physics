#ifndef TOOLS_H_INCLUDED
#define TOOLS_H_INCLUDED

#include <stdio.h>
#include <sys/stat.h>

enum Error_Codes
{
    success,
    error
};

struct Buffer
{
    char *str;
    size_t n_symbs;
};

struct Buffer  Open_File        (void);
struct Buffer  File_To_Buffer   (const char *file_name);
int            Is_Directory     (const char *path);
void          *Calloc_          (const size_t n_elems, const size_t elem_size);
void          *Recalloc_        (void *ptr, size_t old_size, size_t new_size);
size_t         Define_File_Size (FILE *file);
int            Get_Int          (void);
char          *Get_String       (void);

#endif // TOOLS_H_INCLUDED
