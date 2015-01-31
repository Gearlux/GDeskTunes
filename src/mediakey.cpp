#include "mediakey.h"

#ifdef Q_OS_WIN
#include "mediakeydialog.h"
#include "ui_mediakeydialog.h"

#include <QWidget>
#include <QSettings>
#include <QApplication>

#include <QAbstractEventDispatcher>

#include <windows.h>
#include <WinUser.h>
#include <strsafe.h>

void ErrorExit(LPTSTR lpszFunction)
{
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                dw,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR) &lpMsgBuf,
                0, NULL );

    // Display the error message

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
                                      (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf,
                    LocalSize(lpDisplayBuf) / sizeof(TCHAR),
                    TEXT("%s failed with error %d: %s"),
                    lpszFunction, dw, lpMsgBuf);
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}



MediaKey::MediaKey(QWidget *parent) :
    QObject(parent)
{
    QAbstractEventDispatcher::instance()->installNativeEventFilter(this);
}

void MediaKey::load()
{
    qDebug() << "MediaKey::load()";
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    this->media_keys_warning = settings.value("media_keys_warning", true).toBool();
    this->setRegisterMediaKeys(settings.value("register_media_keys", false).toBool());
}

void MediaKey::save()
{
    qDebug() << "MediaKey::load()";
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    settings.setValue("register_media_keys", this->register_media_keys);
    settings.setValue("media_keys_warning", this->media_keys_warning);

    if (this->register_media_keys)
        setRegisterMediaKeys(false);
}

void MediaKey::setRegisterMediaKeys(bool reg)
{
    QWidget* widget = dynamic_cast<QWidget*>(parent());
    if (reg == this->register_media_keys) return;

    if (widget != 0)
    {
        this->register_media_keys = reg;
        emit registerMediaKeys(reg);

        HWND hWND = (HWND)widget->winId();
        if (!reg)
        {
            qDebug() << "Unregistering hot keys" << hWND;
            UnregisterHotKey(hWND,MOD_NOREPEAT ^0xB3);
            UnregisterHotKey(hWND,MOD_NOREPEAT ^0xB1);
            UnregisterHotKey(hWND,MOD_NOREPEAT ^0xB0);
            qDebug() << "Hot keys unregistered";
        }
        else
        {
            bool warning = false;
            if (RegisterHotKey(hWND,MOD_NOREPEAT ^0xB0,MOD_NOREPEAT,0xB0))
            {
                qDebug() << "Hotkey 'NEXT' registered, using MOD_NOREPEAT flag";
            }
            else
            {
                qDebug() << "Hotkey 'NEXT' could not be registered";
               warning = true;
            }
            if (RegisterHotKey(hWND,MOD_NOREPEAT ^0xB1,MOD_NOREPEAT,0xB1))
            {
                qDebug() << "Hotkey 'STOP' registered, using MOD_NOREPEAT flag";
            }
            else
            {
                qDebug() << "Hotkey 'STOP' could not be registered";
                warning = true;
            }
            if (RegisterHotKey(hWND,MOD_NOREPEAT ^0xB3,MOD_NOREPEAT,0xB3))
            {
                qDebug() << "Hotkey 'PLAY/PAUSE' registered, using MOD_NOREPEAT flag";
            }
            else
            {
                qDebug() << "Hotkey 'PLAY/PAUSE' could not be registered";
                warning = true;
            }
            if (warning)
            {
                if (this->media_keys_warning)
                {
                    MediaKeyDialog* dlg = new MediaKeyDialog(widget);
                    int exit_code = dlg->exec();

                    if (exit_code == QDialog::Accepted)
                    {
                        this->media_keys_warning = !dlg->ui->do_not_show_waring->isChecked();
                    }
               }
            }
        }
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
    if (msg->message == WM_APPCOMMAND)
    {
        qDebug() << "app command";
    }

    return false;
}

#endif

