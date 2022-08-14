#include "Tools.h"

int User_Interface (void);

int main (void)
{
    if (User_Interface () == error)
    {
        #ifdef LABA_HELPER_DEBUG
        printf ("User_Interface () terminated with error\n");
        #endif // LABA_HELPER_DEBUG
        return 1;
    }

    return 0;
}
