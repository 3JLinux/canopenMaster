#ifndef CANOPEN_APP_H
#define CANOPEN_APP_H

#ifdef USE_XENO
#define eprintf(...)
#else
#define eprintf(...) printf (__VA_ARGS__)
#endif

#include "canfestival.h"


#endif
