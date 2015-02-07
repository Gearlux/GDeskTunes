#include "mediakey.h"

#ifdef Q_OS_WIN
#include "mediakeydialog.h"
#include "ui_mediakeydialog.h"

#include <QWidget>
#include <QSettings>
#include <QApplication>
#include <QDir>

#include <QAbstractEventDispatcher>

#include <windows.h>
#include <WinUser.h>
#include <strsafe.h>

#include "mmshellhook.h"

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
    QObject(parent),
    register_media_keys(false),
    register_app_commands(false),
    media_keys_warning(false),
    mmgdeskhook(0)
{
    QAbstractEventDispatcher::instance()->installNativeEventFilter(this);
}

MediaKey::~MediaKey()
{
    if (mmgdeskhook != 0)
    {
        qDebug() << "Terminating MMGDeskHook";

        mmgdeskhook->terminate();
        mmgdeskhook->waitForFinished(250);
    }
    QWidget* widget = dynamic_cast<QWidget*>(parent());
    if (widget != 0)
    {
        HWND hWND = (HWND)widget->winId();
        if (register_app_commands)
        {
            UnSetMMShellHook(hWND);
        }
        if (register_media_keys)
        {
            UnregisterHotKey(hWND,MOD_NOREPEAT ^0xB3);
            UnregisterHotKey(hWND,MOD_NOREPEAT ^0xB1);
            UnregisterHotKey(hWND,MOD_NOREPEAT ^0xB0);
        }
    }
}

void MediaKey::load()
{
    qDebug() << "MediaKey::load()";
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    this->media_keys_warning = settings.value("media_keys_warning", true).toBool();
    this->setRegisterMediaKeys(settings.value("register_media_keys", false).toBool());
    this->setRegisterAppCommands(settings.value("register_app_commands", false).toBool());
}

void MediaKey::save()
{
    qDebug() << "MediaKey::load()";
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    settings.setValue("register_media_keys", this->register_media_keys);
    settings.setValue("register_app_commands", this->register_app_commands);
    settings.setValue("media_keys_warning", this->media_keys_warning);
}

void MediaKey::setRegisterAppCommands(bool reg)
{
    qDebug() << "MediaKey::setRegisterAppCommands(" << reg << ")";
    if (reg == this->register_app_commands) return;

    QWidget* widget = dynamic_cast<QWidget*>(parent());
    if (widget != 0)
    {
        this->register_app_commands = reg;
        emit registerAppCommands(reg);

        HWND hWND = (HWND)widget->winId();
        if (reg)
        {
            setRegisterMediaKeys(false);
            SetMMShellHook(hWND);

            if (mmgdeskhook == 0)
            {
                QString program;
#ifdef Q_OS_WIN32
                program = QCoreApplication::applicationDirPath() + QDir::separator() + "MMGDeskHook64.exe";
#else
                program = QCoreApplication::applicationDirPath() + QDir::separator() + "MMGDeskHook.exe";
#endif
                qDebug() << program;
                if (QFile(program).exists())
                {
                    mmgdeskhook = new QProcess();
                    mmgdeskhook->setStandardOutputFile(QString("C:\\gdeskhook.txt"));
                    mmgdeskhook->start(program, QStringList() << QString::number((long)hWND));
                    // mmgdeskhook->start("cmd", QStringList() << "/k" << program << QString::number((long)hWND));
                    qDebug() << program << (long)hWND << hWND << (int)hWND;
                    mmgdeskhook->waitForStarted();
                }
            }
        }
        else
        {
            UnSetMMShellHook(hWND);
            if (mmgdeskhook != 0)
            {
                mmgdeskhook->terminate();
                mmgdeskhook = 0;
            }
        }
    }

    emit ignoreMediaKeys(this->register_app_commands || this->register_media_keys);
}

void MediaKey::setRegisterMediaKeys(bool reg)
{
    if (reg == this->register_media_keys) return;

    QWidget* widget = dynamic_cast<QWidget*>(parent());
    if (widget != 0)
    {
        this->register_media_keys = reg;
        emit registerMediaKeys(reg);

        HWND hWND = (HWND)widget->winId();
        if (reg)
        {
            setRegisterAppCommands(false);

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
        else
        {
            qDebug() << "Unregistering hot keys" << hWND;
            UnregisterHotKey(hWND,MOD_NOREPEAT ^0xB3);
            UnregisterHotKey(hWND,MOD_NOREPEAT ^0xB1);
            UnregisterHotKey(hWND,MOD_NOREPEAT ^0xB0);
            qDebug() << "Hot keys unregistered";
        }
    }

    emit ignoreMediaKeys(this->register_app_commands || this->register_media_keys);
}

bool MediaKey::nativeEventFilter(const QByteArray & eventType, void * message, long * result)
{
    Q_UNUSED(eventType);
    Q_UNUSED(result);

    MSG* msg = static_cast<MSG*>(message);
    if (msg->message == WM_HOTKEY)
    {
        qDebug() << "Received hotkey";
        if (register_media_keys)
        {
            const quint32 keycode = HIWORD(msg->lParam);
            const quint32 modifiers = LOWORD(msg->lParam);

            qDebug() << keycode << modifiers;

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
        }
    }
    if (msg->message == WM_APPCOMMAND)
    {
        qDebug() << "Received appcommand";
        if (register_app_commands)
        {
            qDebug() << msg->lParam;
            switch(GET_APPCOMMAND_LPARAM(msg->lParam))
            {
            case APPCOMMAND_MEDIA_NEXTTRACK:
                emit keyReceived(19, false, true);
                return true;
            case APPCOMMAND_MEDIA_PREVIOUSTRACK:
                emit keyReceived(20, false, true);
                return true;
            case APPCOMMAND_MEDIA_PLAY_PAUSE:
                emit keyReceived(16, false, true);
                return true;
            }
        }
    }
    return false;
}
#endif

