#include <Cocoa/Cocoa.h>

#include "macutils.h"

void hideMac()
{
    ProcessSerialNumber pn;
    GetFrontProcess (&pn);
    ShowHideProcess(&pn,false);
}
