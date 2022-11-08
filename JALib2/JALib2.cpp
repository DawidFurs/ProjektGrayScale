#include "pch.h"
#include "framework.h"
#include "JALib2.h"


// This is an example of an exported variable
JALIB2_API int nJALib2=0;

// This is an example of an exported function.
JALIB2_API int fnJALib2(void)
{
    return 0;
}

// This is the constructor of a class that has been exported.
CJALib2::CJALib2()
{
    return;
}
