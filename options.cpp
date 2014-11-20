#include "options.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "lastfm.h"

#include <QDebug>
#include <QDir>
#include <QWebFrame>

Options::Options(MainWindow *window) :
    main_window(window),
    hide_menu(false),
    mini_player_on_top(false),
    save_cookies(true),
    customize(false),
    css(""),
    mini_css(""),
    last_fm_user_name(QString::null),
    last_fm_password(QString::null)
{
}

void Options::save(QSettings &settings)
{
    settings.setValue("miniPlayerOnTop", this->mini_player_on_top);
    settings.setValue("css", this->css);
    settings.setValue("minicss", this->mini_css);
    settings.setValue("hideMenu", this->hide_menu);
    settings.setValue("saveCookies", this->save_cookies);
    settings.setValue("customize", this->customize);
    settings.setValue("last.fm.username", this->last_fm_user_name);
    settings.setValue("last.fm.password", this->last_fm_password);
    settings.setValue("last.fm.authorized", main_window->last_fm->isAuthorized());
    settings.setValue("last.fm.scrobble", this->scrobble);
    settings.setValue("last.fm.autolike", this->auto_like);
}

void Options::restore(QSettings &settings)
{
    qDebug() << "Restore options";
    this->mini_player_on_top = settings.value("miniPlayerOnTop", false).toBool();

    this->css = settings.value("css", "").toString();
    this->mini_css = settings.value("minicss", "Default").toString();
    this->setHideMenu(settings.value("hideMenu", false).toBool());
    this->save_cookies = settings.value("saveCookies", true).toBool();
    this->customize = settings.value("customize", false).toBool();
    this->last_fm_user_name = settings.value("last.fm.username", "").toString();
    this->last_fm_password = settings.value("last.fm.password", "").toString();
    if (settings.value("last.fm.authorized", false).toBool())
    {
        main_window->last_fm->login();
    }
}

void Options::setCSS(QString css)
{
    main_window->disableStyle(this->css);
    this->css = css;
    if (this->customize && !main_window->isMini())
    {
        main_window->applyStyle(css);
    }
}

void Options::setMiniCSS(QString css)
{
    main_window->disableStyle(this->mini_css, "mini");
    this->mini_css = css;
    if (this->customize && main_window->isMini())
    {
        main_window->applyStyle(css, "mini");
    }

}

QString Options::getCSS()
{
    return css;
}

QString Options::getMiniCSS()
{
    return mini_css;
}

Qt::WindowFlags Options::miniPlayerFlags()
{
    Qt::WindowFlags flags;
    flags = Qt::Window;
    // flags |= Qt::CustomizeWindowHint;
    // flags |= Qt::WindowMinimizeButtonHint;
    // flags |= Qt::WindowCloseButtonHint;
    // if (mini_resize)
    //    flags |= Qt::WindowMaximizeButtonHint;
    flags |= Qt::FramelessWindowHint;
    if (mini_player_on_top)
        flags |= Qt::WindowStaysOnTopHint;

    return flags;
}

void Options::setMiniPlayerOnTop(bool on_top)
{
    this->mini_player_on_top = on_top;

    if (main_window->isMini())
    {
        Qt::WindowFlags flags = miniPlayerFlags();
        qDebug() << flags;
        main_window->setWindowFlags(flags);
        main_window->show();
    }
}

bool Options::isMiniPlayerOnTop()
{
    return this->mini_player_on_top;
}

void Options::setHideMenu(bool hide)
{
    hide_menu = hide;

    // Show the menu to update the visibility of the menu
    if (!main_window->isHidden())
        main_window->show();
}

bool Options::isHideMenu()
{
    return hide_menu;
}

void Options::setSaveCookies(bool save)
{
    qDebug() << "saveCookies=" << save;
    this->save_cookies = save;
}

bool Options::saveCookies()
{
    return this->save_cookies;
}

void Options::setCustomize(bool customize)
{
    this->customize = customize;
    if ( !main_window->isMini() )
    {
        if (customize )
        {
            main_window->applyStyle(css);
        }
        else
        {
            main_window->disableStyle(css);
        }
    }
}

bool Options::isCustomized()
{
    return this->customize;
}

QString Options::getStyle(QString name, QString subdir)
{
#ifdef Q_OS_MAC
    QDir dir(QCoreApplication::applicationDirPath() + QDir::separator() + "../../../userstyles" + QDir::separator() + subdir);
#else
    QDir dir(QCoreApplication::applicationDirPath() + QDir::separator() + "userstyles" + QDir::separator()+ subdir);
#endif
    QList<QFileInfo> files = dir.entryInfoList();

    for(QList<QFileInfo>::iterator it = files.begin(); it != files.end(); ++it)
    {
        if ((*it).isFile() && name.compare((*it).baseName()) == 0)
            return (*it).absoluteFilePath();
    }
    return QString::null;
}

QStringList Options::getStyles(QString subdir)
{
#ifdef Q_OS_MAC
    QDir dir(QCoreApplication::applicationDirPath() + QDir::separator() + "../../../userstyles" + QDir::separator() + subdir);
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



