#include "settings.h"
#include "ui_settings.h"

#include <QDir>
#include <QDebug>
#include <QKeyEvent>

#include "gdesktunes.h"
#include "ui_mainwindow.h"
#include "lastfm.h"
#include "cookiejar.h"

Settings::Settings(GDeskTunes *parent) :
    QDialog(parent),
    ui(new Ui::Settings),
    last_fm_authorized(false),
    // FIXME: remove reference to main_window
    main_window(parent)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog
                   | Qt::WindowCloseButtonHint
#if QT_VERSION < QT_VERSION_CHECK(5,4,0)
                   | Qt::CustomizeWindowHint
#endif
                   );

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    // Remove the appearance and the shortcuts tab since it is not yet implemented in Qt4
    ui->tabWidget->removeTab(2);
    // On Windows style sheets do not work in Qt 4
#ifdef Q_OS_WIN
    ui->tabWidget->removeTab(1);
#endif
#endif
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

void Settings::show()
{
    ui->mini_on_top->setChecked(main_window->isMiniPlayerOnTop());

    ui->customize->setChecked(main_window->isCustomized());

    QStringList styles = getStyles();

    QString css_path = main_window->getCSS();
    QFileInfo css_info(css_path);
    int index = styles.indexOf(css_info.baseName());
    ui->style_combo->clear();
    ui->style_combo->addItems(styles);
    ui->style_combo->setCurrentIndex(index);

    QStringList mini_styles = getStyles(QString("mini"));
    QString mini_css = main_window->getMiniCSS();
    QFileInfo mini_css_info(mini_css);
    int mini_index = mini_styles.indexOf(mini_css_info.baseName());
    ui->mini_style_combo->clear();
    ui->mini_style_combo->addItems(mini_styles);
    ui->mini_style_combo->setCurrentIndex(mini_index);

    QDialog::show();
}

void Settings::closeEvent(QCloseEvent *ev)
{
    qDebug() << "Settings::closeEvent(" << ev << ")";
}

void Settings::miniPlayerOnTop(bool on_top)
{
    main_window->setMiniPlayerOnTop(on_top);
}

void Settings::style(QString style)
{
    main_window->setCSS(style);
}

void Settings::miniStyle(QString style)
{
    main_window->setMiniCSS(style);
}

void Settings::clearCookies()
{
    QNetworkCookieJar *cookiejar = this->main_window->ui->webView->page()->networkAccessManager()->cookieJar();
    CookieJar *jar = qobject_cast<CookieJar*>(cookiejar);
    if (jar != 0)
    {
        qDebug() << "Deleting all cookies";
       jar->deleteAllCookies();
       main_window->ui->webView->load(QUrl("https://play.google.com/music/listen#"));
    }
}

void Settings::doNotSaveCookies(bool do_not_save)
{
    if (do_not_save)
    {
        QNetworkCookieJar *cookiejar = this->main_window->ui->webView->page()->networkAccessManager()->cookieJar();
        CookieJar *jar = qobject_cast<CookieJar*>(cookiejar);
        if (jar != 0)
        {
            qDebug() << "Deleting cookie file";
           jar->removeCookieFile();
        }
    }
    emit saveCookies(!do_not_save);
}

void Settings::customize(bool customize)
{
    main_window->setCustomize(customize);
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

void Settings::setAuthorized(bool authorized)
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
