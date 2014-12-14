#define QT_NO_DEBUG_OUTPUT

#include "settings.h"
#include "ui_settings.h"

#include <QDir>
#include <QDebug>
#include <QKeyEvent>
#include <QSystemTrayIcon>

#include "gdesktunes.h"
#include "ui_mainwindow.h"
#include "lastfm.h"
#include "cookiejar.h"

Settings::Settings(GDeskTunes *parent) :
    QDialog(parent),
    ui(new Ui::Settings),
    last_fm_authorized(false)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog
                   | Qt::WindowCloseButtonHint
#if QT_VERSION < QT_VERSION_CHECK(5,4,0)
                   | Qt::CustomizeWindowHint
#endif
                   );

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    // On Windows style sheets do not work in Qt 4
#ifdef Q_OS_WIN
    ui->tabWidget->removeTab(1);
#endif
#endif

    bool tray = QSystemTrayIcon::isSystemTrayAvailable();

    if (!tray)
    {
        ui->tray_icon->hide();
        ui->show_notifications->hide();
        ui->notifications_label->hide();
    }

#ifdef Q_OS_MAC
    ui->minimize_to_tray->hide();
#endif

    QStringList styles = getStyles();
    ui->CSS->addItems(styles);

    QStringList mini_styles = getStyles(QString("mini"));
    ui->mini_CSS->addItems(mini_styles);
}

Settings::~Settings()
{
    delete ui;
}

void Settings::activateAndRaise()
{
    show();
    raise();
    activateWindow();
}

void Settings::authorize()
{
    qDebug() << "Settings::authorize()";
    if (last_fm_authorized)
    {
        emit logout();
    }
    else
    {
        emit login(this->ui->last_fm_user_name_text->text(), this->ui->last_fm_password_text->text());
    }
}

void Settings::authorized(bool authorized)
{
    qDebug() << "Settings::setAuthorized(" << authorized << ")";
    this->last_fm_authorized = authorized;
    if (authorized)
    {
        ui->authorize->setText("Deauthorize");
    }
    else
    {
        ui->authorize->setText("Authorize");
    }
}

QStringList Settings::getStyles(QString subdir)
{
#ifdef Q_OS_MAC
    QDir dir(QCoreApplication::applicationDirPath() + QDir::separator() + "../Resources/userstyles" + QDir::separator() + subdir);
#else
    QDir dir(QCoreApplication::applicationDirPath() + QDir::separator() + "userstyles" + QDir::separator() + subdir);
#endif
    qDebug() << dir;
    QList<QFileInfo> files = dir.entryInfoList();
    QStringList result;
    for(QList<QFileInfo>::iterator it = files.begin(); it != files.end(); ++it)
    {
        if ((*it).isFile())
        {
            QString basename = (*it).baseName();
            if (!basename.contains("mini"))
                result.append(basename);
        }
    }
    return result;
}

void Settings::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_MediaPlay:
    case Qt::Key_MediaNext:
    case Qt::Key_MediaPrevious:
    {
        MainWindow *w = qobject_cast<MainWindow*>(parent());
        if (w != 0)
        {
            qDebug() << "Settings dialog captures media key";
            w->keyPressEvent(event);
            break;
        }
    }
    default:
        QDialog::keyPressEvent(event);
        break;
    }
}
