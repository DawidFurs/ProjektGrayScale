// JALib.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "JALib.h"


// This is an example of an exported variable
JALIB_API int nJALib=0;

// This is an example of an exported function.
JALIB_API int fnJALib(void)
{
    return 0;
}

// This is the constructor of a class that has been exported.
CJALib::CJALib()
{
    return;
}
