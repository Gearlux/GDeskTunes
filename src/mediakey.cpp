#include "mediakey.h"

#ifdef Q_OS_WIN
#include <QAbstractEventDispatcher>

#include <windows.h>
#include <WinUser.h>

MediaKey::MediaKey()
{
    QAbstractEventDispatcher::instance()->installNativeEventFilter(this);

    if (RegisterHotKey(NULL,2,MOD_NOREPEAT,0xB0))
        {
            qDebug() << "Hotkey 'NEXT' registered, using MOD_NOREPEAT flag\n";
        }
    if (RegisterHotKey(NULL,3,MOD_NOREPEAT,0xB1))
        {
            qDebug() << "Hotkey 'STOP' registered, using MOD_NOREPEAT flag\n";
        }
    if (RegisterHotKey(NULL,4,MOD_NOREPEAT,0xB3))
        {
            qDebug() << "Hotkey 'PLAY/PAUSE' registered, using MOD_NOREPEAT flag\n";
        }
}

bool MediaKey::nativeEventFilter(const QByteArray & eventType, void * message, long * result)
{
    Q_UNUSED(eventType);
    Q_UNUSED(result);

    MSG* msg = static_cast<MSG*>(message);
    if (msg->message == WM_HOTKEY)
    {
        const quint32 keycode = HIWORD(msg->lParam);
        const quint32 modifiers = LOWORD(msg->lParam);

        if (modifiers == 0)
        {
            switch(keycode)
            {
            case 0xb3:
                emit keyReceived(16, false, true);
                return true;
            case 0xb0:
                emit keyReceived(19, false, true);
                return true;
            case 0xb1:
                emit keyReceived(20, false, true);
                return true;
            }
        }
        // activateShortcut(keycode, modifiers);
        qDebug() << keycode << modifiers;
    }

    return false;
}

#endif

