// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the JALIB2_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// JALIB2_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef JALIB2_EXPORTS
#define JALIB2_API __declspec(dllexport)
#else
#define JALIB2_API __declspec(dllimport)
#endif

// This class is exported from the dll
class JALIB2_API CJALib2 {
public:
	CJALib2(void);
	// TODO: add your methods here.
};

extern JALIB2_API int nJALib2;

JALIB2_API int fnJALib2(void);
