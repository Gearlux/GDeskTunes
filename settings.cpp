#include "settings.h"
#include "ui_settings.h"

#include <QDir>
#include <QDebug>

#include "mainwindow.h"
#include "lastfm.h"
#include "options.h"
#include "cookiejar.h"

Settings::Settings(MainWindow *parent) :
    QDialog(parent->webView()),
    ui(new Ui::Settings),
    main_window(parent),
    options(parent->options)
{
    ui->setupUi(this);

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    // Remove the appearance and the shortcuts tab since it is not yet implemented in Qt4
    ui->tabWidget->removeTab(2);
    // On Windows style sheets do not work in Qt 4
#ifdef Q_OS_WIN
    ui->tabWidget->removeTab(1);
#endif
#endif

    ui->mini_on_top->setChecked(options.isMiniPlayerOnTop());

    ui->cookies->setChecked(!options.saveCookies());
    ui->customize->setChecked(options.isCustomized());

    QStringList styles = options.getStyles();

    QString css_path = options.getCSS();
    QFileInfo css_info(css_path);
    int index = styles.indexOf(css_info.baseName());
    ui->style_combo->addItems(styles);
    ui->style_combo->setCurrentIndex(index);

    QStringList mini_styles = options.getStyles(QString("mini"));
    QString mini_css = options.getMiniCSS();
    QFileInfo mini_css_info(mini_css);
    int mini_index = mini_styles.indexOf(mini_css_info.baseName());
    ui->mini_style_combo->addItems(mini_styles);
    ui->mini_style_combo->setCurrentIndex(mini_index);

    ui->last_fm_user_name_text->setText(options.getLastFMUserName());
    ui->last_fm_password_text->setText(options.getLastFMPassword());

    this->lastFMChanged();

#ifdef Q_OS_MACX
    qDebug() << "Tweaking settings dialog for mac";
#endif

   connect(main_window->last_fm, SIGNAL(changed()), this, SLOT(lastFMChanged()));
}

Settings::~Settings()
{
    delete ui;
}

void Settings::closeEvent(QCloseEvent *)
{
    main_window->settingsDestroyed();
}

void Settings::miniPlayerOnTop(bool on_top)
{
    options.setMiniPlayerOnTop(on_top);
}

void Settings::style(QString style)
{
    options.setCSS(style);
}

void Settings::miniStyle(QString style)
{
    options.setMiniCSS(style);
}

void Settings::clearCookies()
{
    QNetworkCookieJar *cookiejar = this->main_window->webView()->page()->networkAccessManager()->cookieJar();
    CookieJar *jar = qobject_cast<CookieJar*>(cookiejar);
    if (jar != 0)
    {
        qDebug() << "Deleting all cookies";
       jar->deleteAllCookies();
       main_window->webView()->load(QUrl("https://play.google.com/music/listen#"));
    }
}

void Settings::doNotSaveCookies(bool do_not_save)
{
    if (do_not_save)
    {
        QNetworkCookieJar *cookiejar = this->main_window->webView()->page()->networkAccessManager()->cookieJar();
        CookieJar *jar = qobject_cast<CookieJar*>(cookiejar);
        if (jar != 0)
        {
            qDebug() << "Deleting cookie file";
           jar->removeCookieFile();
        }
    }
    options.setSaveCookies(!do_not_save);
}

void Settings::customize(bool customize)
{
    options.setCustomize(customize);
}

void Settings::authorize()
{
    qDebug() << "Settings::authorize()";
    if (!main_window->last_fm->isAuthorized())
    {
        main_window->options.setLastFMUserName(this->ui->last_fm_user_name_text->text());
        main_window->options.setLastFMPassword(this->ui->last_fm_password_text->text());
        main_window->last_fm->login();
    }
    else
    {
        main_window->last_fm->logout();
    }
    lastFMChanged();
}

void Settings::lastFMChanged()
{
    qDebug() << "Settings::lastFMChanged()";
    if (main_window->last_fm->isAuthorized())
    {
        ui->authorize->setText("Deauthorize");
    }
    else
    {
        ui->authorize->setText("Authorize");
    }
}
