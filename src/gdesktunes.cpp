#include "gdesktunes.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QDirIterator>
#include <QWebFrame>
#include <QWebElement>
#include <QSettings>
#include <QDebug>

#ifdef Q_OS_WIN
#if QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
#include <QtWinExtras>
#endif
#endif

GDeskTunes::GDeskTunes(QWidget* parent):
    MainWindow(parent),
    mini(false)
{
    // Setup webview and windows interaction
    connect(ui->webView, SIGNAL(loadFinished(bool)), this, SLOT(finishedLoad(bool)));

}

/*
 * Set the style for the application and applies it if the
 * application is customized.
 */
void GDeskTunes::setCSS(QString css)
{
    qDebug() << "GDeskTunes::setCSS(" << css << ")";
    if (this->css == css) return;
    disableStyle(this->css);
    this->css = css;
    emit CSS(css);
    if (this->customize && !this->isMini())
    {
        applyStyle(css);
    }
}

/*
 * Set the style for the mini-player and applies if the
 * application is in mini-player mode.
 */
void GDeskTunes::setMiniCSS(QString css)
{
    qDebug() << "GDeskTunes::setMiniCSS(" << css << ")";
    if (this->mini_css == css) return;
    disableStyle(this->mini_css, "mini");
    emit miniCSS(css);
    if (isMini())
    {
        applyStyle(css, "mini");
    }
}

/*
 * Switches between mini-player and full desktop if applicable.
 */
void GDeskTunes::setMini(bool toMini)
{
    qDebug() << "GDeskTunes::setMini(" << toMini << ")";
    this->draggable = toMini;

    if (toMini)
    {
        // Save the current geometry and state if not mini
        if (!this->mini)
        {
            saveState();
            normal_flags = windowFlags();
        }
        hide();

        ui->actionSwitch_mini->setText("Switch from Miniplayer");

        // Apply the mini style
        QString mini_style = this->mini_css;
        applyStyle(mini_style, "mini");

        // Find the body element which must contain the width and height of the mini player
        QWebElement elt = ui->webView->page()->mainFrame()->documentElement().findFirst("body");
        QString width = elt.styleProperty("width", QWebElement::ComputedStyle);
        QString height = elt.styleProperty("height", QWebElement::ComputedStyle);
        int w = width.replace("px", "").toInt(0);
        int h = height.replace("px", "").toInt(0);

        // Apply window flags
        setWindowFlags(miniPlayerFlags());
        show();
        hide();

        // Changing flags loses geometry, save position into pos variable
        restoreMini();
        QPoint pos = this->pos();
        this->mini = toMini;

        // Apply new geometry
        move(pos - windows_offset);
        resize(w, h);
        show();
    }
    else
    {
        if (this->mini)
            saveState();
        hide();
        this->mini = toMini;

        ui->actionSwitch_mini->setText("Switch to Miniplayer");

        setWindowFlags(normal_flags
#ifndef Q_OS_LINUX
                       | Qt::WindowStaysOnBottomHint
#endif
                       );
        // Show hide combination is only to apply the window flags
        // This will lose the geometry which will be set after hide()
        show();
        hide();

        restore();
        disableStyle(this->mini_css, "mini");
        if (customize)
        {
            applyStyle(this->css);
        }

#ifdef Q_OS_LINUX
        activateWindow();
#endif
        show();
    }

#if defined Q_OS_WIN && QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    updateJumpList();
#endif

}

/*
 * Places the mini-player on top if it's running,
 * otherwise stores the variable for next time.
 */
void GDeskTunes::setMiniPlayerOnTop(bool on_top)
{
    if (on_top == this->mini_player_on_top) return;

    this->mini_player_on_top = on_top;
    emit miniPlayerOnTop(on_top);

    if (isMini())
    {
        setWindowFlags(miniPlayerFlags());
        show();
    }
}

/*
 * Customizes the player.
 */
void GDeskTunes::setCustomize(bool customize)
{
    if (customize == this->customize) return;

    this->customize = customize;
    emit customized(customize);

    if (customize )
    {
        applyStyle(css);
    }
    else
    {
        disableStyle(css);
    }
}

/*
 * This function executes all java scripts in the js directory of the application
 * and applies the selected style.
 */
void GDeskTunes::finishedLoad(bool ok)
{
    qDebug() << "GDeskTunes::finishedLoad(" << ok << ")";
    if (!ok)
        return;

#ifdef Q_OS_MAC
    QDir dir(QCoreApplication::applicationDirPath() + QDir::separator() + "../Resources/js");
#else
    QDir dir(QCoreApplication::applicationDirPath() + QDir::separator() + "js");
#endif

    QDirIterator it(dir);
    while(it.hasNext())
    {
        QFileInfo nextFile(it.next());
        if (nextFile.isDir())
            continue;
        evaluateJavaScriptFile(nextFile.absoluteFilePath());
    }

    if (this->customize)
        applyStyle(this->css);

    updateAppearance();
}

/*
 * Evaluates the java script in the file located at filePath.
 */
void GDeskTunes::evaluateJavaScriptFile(QString filePath)
{
    qDebug() << "evaluateJavaScriptFile(" << filePath << ")";
    QFile jsFile(filePath);
    jsFile.open(QFile::ReadOnly);
    QTextStream stream(&jsFile);
    ui->webView->page()->mainFrame()->evaluateJavaScript(stream.readAll());
    jsFile.close();
}

/*
 * Set the style.
 */
void GDeskTunes::setStyle(QString name, QString css_content)
{
    css_content.remove(QRegExp("[\\n\\t\\r]"));
    css_content.replace("\"", "\\\"");

    qDebug() << "GDeskTunes::setStyle(" << name << "," << css_content.left(100) << ")";

    QString script = QString("Styles.applyStyle(\"%2\",\"%1\");").arg(css_content, name);
    ui->webView->page()->mainFrame()->evaluateJavaScript(script);
}

/*
 * Applies the style with name css and in located in the subdir
 * of the application's directory.
 */
void GDeskTunes::applyStyle(QString css, QString subdir)
{
    qDebug() << "GDeskTunes::applyStyle(" << css << "," << subdir << ")";
    QString full_css = getStyle(css, subdir);

    QFile cssFile(full_css);
    cssFile.open(QFile::ReadOnly);
    QTextStream stream(&cssFile);
    QString css_content = stream.readAll();
    setStyle(subdir + css, css_content);
}

/*
 * Disables the style
 */
void GDeskTunes::disableStyle(QString css, QString subdir)
{
    // ui->webView->page()->mainFrame()->evaluateJavaScript(QString("Styles.disableStyle(\"%2%1\")").arg(css, subdir));
    ui->webView->page()->mainFrame()->evaluateJavaScript(QString("Styles.removeStyle(\"%2%1\")").arg(css, subdir));
}

/*
 * Retrieves the full path of the style with name and located
 * in the application's userstyles subdirectory.
 */
QString GDeskTunes::getStyle(QString name, QString subdir)
{
#ifdef Q_OS_MAC
    QDir dir(QCoreApplication::applicationDirPath() + QDir::separator() + "../Resources/userstyles" + QDir::separator() + subdir);
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

/*
 * WindowFlags for the mini player.
 */
Qt::WindowFlags GDeskTunes::miniPlayerFlags()
{
    Qt::WindowFlags flags;
    flags = Qt::Window;
    flags |= Qt::FramelessWindowHint;
    if (mini_player_on_top)
        flags |= Qt::WindowStaysOnTopHint;

    return flags;
}

void GDeskTunes::receiveMessage(const QString &msg)
{
    qDebug() << "GDeskTunes::receiveMessage(" << msg << ")";
    QStringList commands = msg.split(" ");

    qDebug() << "Received message" << msg;
    if (commands.contains("--mini"))
    {
        setMini(!mini);
    }
    if (commands.contains("--settings"))
    {
        ui->actionPreferences->trigger();
    }
    if (commands.contains("--menu"))
    {
        ui->actionShow_menu->trigger();
    }
    if (commands.contains("--fullscreen"))
    {
        ui->actionEnter_Full_Screen->trigger();
    }
    if (commands.contains("--about"))
    {
        ui->actionAbout->trigger();
    }
}

void GDeskTunes::show()
{
    qDebug() << "GDeskTunes::show()";
    ui->toolBar->setVisible(isMini());

#ifndef Q_OS_MAC
    if (isMini())
    {
        bool old_hide_menu = this->hide_menu;
       setMenuVisible(false);
       this->hide_menu = old_hide_menu;
    }
    else
    {
        qDebug() << "GDeskTunes::show()::setMenuVisible(" << !this->hide_menu << ")";
        setMenuVisible(!this->hide_menu);
    }
#endif

    MainWindow::show();
}

void GDeskTunes::saveState()
{
    qDebug() << "GDeskTunes::saveState()";
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    if (isMini())
    {
        settings.setValue("miniGeometry", saveGeometry());
    }
    else
    {
        settings.setValue("geometry", saveGeometry());
        settings.setValue("state", QMainWindow::saveState());
    }
}

void GDeskTunes::save()
{
    qDebug() << "GDeskTunes::save()";
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    settings.setValue("miniPlayerOnTop", this->mini_player_on_top);
    settings.setValue("customize", this->customize);
    settings.setValue("css", this->css);
    settings.setValue("minicss", this->mini_css);
    settings.setValue("hideMenu", this->ui->menuBar->isHidden());

    settings.setValue("keeplogo", this->keep_logo);
    settings.setValue("navigation.buttons", this->navigation_buttons);
}

void GDeskTunes::load()
{
    qDebug() << "GDeskTunes::load()";
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    this->setMiniPlayerOnTop(settings.value("miniPlayerOnTop", false).toBool());
    this->setCSS(settings.value("css", "").toString());
    this->setMiniCSS(settings.value("minicss", "Default").toString());
    setMenuVisible(!settings.value("hideMenu", false).toBool());
    this->setCustomize(settings.value("customize", false).toBool());

    this->setKeepLogo(settings.value("keeplogo", true).toBool());
    this->setNavigationButtons(settings.value("navigation.buttons", true).toBool());
}

void GDeskTunes::restore()
{
    qDebug() << "GDeskTunes::restore()";
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    qDebug() << "Preferences: " << settings.fileName();

    QString key = "geometry";
    if (settings.contains(key))
    {
        restoreGeometry(settings.value(key).toByteArray());
    }
    key = "state";
    if (settings.contains(key))
    {
        restoreState(settings.value(key).toByteArray());
    }
}

void GDeskTunes::restoreMini()
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    if (settings.contains("miniGeometry"))
    {
        qDebug() << "Restore Mini Position";
        restoreGeometry(settings.value("miniGeometry").toByteArray());
    }
}

void GDeskTunes::switchMini()
{
    qDebug() << "GDeskTunes::switchMini()";
    setMini(!isMini());
}

void GDeskTunes::updateAppearance()
{
    qDebug() << "GDeskTunes::updateAppearance()";
    disableStyle("gdesktunes.navigation.customization");

    qDebug() << "keep_logo: " << keep_logo;
    qDebug() << "navigations_buttons: " << navigation_buttons;
    qDebug() << "keep_links: " << keep_links;
    QString css;
    if (keep_logo)
    {
        if (navigation_buttons)
            css += "#oneGoogleWrapper > div:first-child > div:first-child > div:nth-child(2) { min-width: 310px !important; }";
    }
    else
    {
        css += "#oneGoogleWrapper > div:first-child > div:first-child > div:nth-child(2) > div:first-child > a:first-child { display: none !important; }";
    }
    if (navigation_buttons)
    {
        css += "#oneGoogleWrapper > div:first-child > div:first-child > div:nth-child(1) { -webkit-flex-grow: 0; }";
        css += "#oneGoogleWrapper > div:first-child > div:first-child > div:nth-child(3) { -webkit-flex-grow: 1; }";

        css += ".gm-nav-button { display: inline-block; border: none; outline: none; vertical-align: top; opacity: 0.6; ";
        css += " width: 30px; height: 30px; background-color: transparent; background-size: 30px 30px; margin: 15px 8px 0 8px; }";

        css += " .gm-nav-button:hover { opacity: 0.4; } ";
        css += ".gm-nav-button:active { opacity: 0.6; }";

        css += "#gm-back { background-image: url(http://radiant-player-mac/images/arrow-left.png); }";
        css += "#gm-forward { background-image: url(http://radiant-player-mac/images/arrow-right.png); }";
    }
    else
    {
        css += " .gm-nav-button { display: none; }";
    }
    if (!keep_links)
    {
        css += "#oneGoogleWrapper > div:first-child > div:first-child > div:first-child > div:nth-child(2) { min-width: 0px !important; }";
        css += "#oneGoogleWrapper > div:first-child > div:first-child > div:first-child > div:first-child { display: none; }";
        css += "#oneGoogleWrapper > div:first-child > div:first-child > div:first-child > div:nth-child(2) > div:nth-child(2) { display: none; }";
        css += "#oneGoogleWrapper > div:first-child > div:first-child > div:first-child > div:nth-child(2) > div:nth-child(3) { display: none; }";
        css += "#oneGoogleWrapper > div:first-child > div:first-child > div:first-child > div:nth-child(2) > div:nth-child(4) { display: none; }";
    }

    qDebug() << css;
    setStyle("gdesktunes.navigation.customization", css);
}

void GDeskTunes::loadUrl()
{
    ui->webView->load(QUrl("https://play.google.com/music/listen#"));
}
