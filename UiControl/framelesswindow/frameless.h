#ifndef FRAMELESS_H
#define FRAMELESS_H

#if defined (WIN32)
#include "win/winlesswindow.h"
#else
#include "unix/framelesshelper.h"
#endif

#endif // FRAMELESS_H
