#include "logitech.h"

#include <QSettings>

Logitech::Logitech()
{
}

int Logitech::check()
{
    QSettings logitech_settings( "HKEY_LOCAL_MACHINE\\SOFTWARE\\Logitech\\EvtMgr6", QSettings::NativeFormat );


    return 0;

    // taskkill /F /IM setpoint.exe


}
