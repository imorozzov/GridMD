// gmd library definitions for internal use

#include <memory.h>

#ifdef __WIN32
#define gmdZeroMemory(obj)  ZeroMemory(&obj, sizeof(obj))
#else
#define gmdZeroMemory(obj)  memset((void*) & obj, 0, sizeof(obj))
#endif

inline bool gmdIsEmpty(const char *p) { return !p || !*p; }
