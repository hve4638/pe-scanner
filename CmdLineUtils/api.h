#ifdef _CMDLINE_LIB
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API 
#endif