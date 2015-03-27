// #define QT_NO_DEBUG_OUTPUT

#include "gdesktunes.h"
#include "ui_mainwindow.h"

#include "googlemusicapp.h"

#include <QDir>
#include <QDirIterator>
#include <QWebFrame>
#include <QWebElement>
#include <QSettings>
#include <QDebug>
#include <QFileSystemWatcher>

#ifdef Q_OS_WIN
#if QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
#include <QtWinExtras>
#endif
#endif

#include "sass_context.h"

GDeskTunes::GDeskTunes(QWidget* parent):
    MainWindow(parent),
    css(QString::null),
    mini_css(QString::null),
    mini(false),
    customize(false),
    mini_player_on_top(false),
    keep_logo(false),
    keep_links(false),
    navigation_buttons(false),
    player_buttons(false),
    style_menu(false),
    show_sidebar(true),
    check_update_startup(false),
    updates_checked(false),
    watcher(new QFileSystemWatcher)
{
    // Setup webview and windows interaction
    connect(ui->webView, SIGNAL(loadFinished(bool)), this, SLOT(finishedLoad(bool)));

    connect(watcher, SIGNAL(fileChanged(QString)), this, SLOT(reapplyStyleFile(QString)));
    connect(ui->webView, SIGNAL(linkClicked( const QUrl & )), this, SLOT(onLinkClicked( const QUrl & )) );

    connect(ui->actionFind, SIGNAL(triggered()), this, SLOT(on_actionFind_triggered()));
    connect(ui->closeSearch, SIGNAL(clicked()), this, SLOT(on_closeSearch_clicked()));
}

/*
 * Set the style for the application and applies it if the
 * application is customized.
 */
void GDeskTunes::setCSS(QString css)
{
    qDebug() << "GDeskTunes::setCSS(" << css << ")";
    if (this->css != css)
    {
        disableStyle(this->css, QString::null);
        emit CSS(css);
    }
    this->css = css;
    if (this->customize)
    {
        if (this->mini)
            disableStyle(this->mini_css, "mini");
        applyStyle(css);
        if (this->mini)
            applyStyle(this->mini_css, "mini");
    }
}

/*
 * Set the style for the mini-player and applies if  the
 * application is in mini-player mode.
 */
void GDeskTunes::setMiniCSS(QString css)
{
    qDebug() << "GDeskTunes::setMiniCSS(" << css << ")";
    if (this->mini_css == css) return;
    disableStyle(this->mini_css, "mini");
    this->mini_css = css;
    if (isMini())
    {
        // In order to trigger the correct layout
        // Disable the layout and then apply it again
        setMini(false);
        setMini(true);
    }
    qDebug() << "Sending signal miniCSS(" << css << ")";
    emit miniCSS(css);
}

/*
 * Switches between mini-player and full desktop if applicable.
 */
void GDeskTunes::setMini(bool toMini)
{
    qDebug() << "GDeskTunes::setMini(" << toMini << ")";
    this->draggable = toMini;
    emit isDraggable(this->draggable);

    if (toMini)
    {
        // Save the current geometry and state if not mini
        if (!this->mini)
        {
            saveState();
            normal_flags = windowFlags();
        }
        hide();

        ui->actionSwitch_mini->setText("Switch from Compact Layout");
        ui->actionSwitch_Full_Screen->setEnabled(false);

        // Apply the mini style
        QString mini_style = this->mini_css;
        qDebug() << "Apply mini style: " << mini_style;
        applyStyle(mini_style, "mini");

        // Find the body element which must contain the width and height of the mini player
        GoogleMusicApp* app = dynamic_cast<GoogleMusicApp*>(ui->webView->page());
        int w = app->getBodyWidth();
        int h = app->getBodyHeight();

        // Apply window flags
        setWindowFlags(miniPlayerFlags());
        show();
#ifndef Q_OS_WIN
        hide();
#endif

        // Changing flags loses geometry, save position into pos variable
        restoreMini();
        QPoint pos = this->pos();
        this->mini = toMini;

        // Apply new geometry
#ifdef Q_OS_MAC
        move(pos);
#else
        move(pos - windows_offset);
#endif
        resize(w, h);
        show();
    }
    else
    {
        if (this->mini)
            saveState();
        hide();
        this->mini = toMini;

        disableStyle(this->mini_css, "mini");
        if (customize)
        {
            applyStyle(this->css);
        }

        ui->actionSwitch_mini->setText("Switch to Compact Layout");
        ui->actionSwitch_Full_Screen->setEnabled(true);

        setWindowFlags(normal_flags
#ifndef Q_OS_LINUX
                       | Qt::WindowStaysOnBottomHint
#endif
                       );
        // Show hide combination is only to apply the window flags
        // This will lose the geometry which will be set after hide()
        show();
#ifndef Q_OS_WIN
        hide();
#endif

        restore();

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
void GDeskTunes::setCustomized(bool customize)
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

        ui->toolBar->setStyleSheet("");
        ui->menuBar->setStyleSheet("");
        ui->searchbar->setStyleSheet("");
        ui->search->setStyleSheet("");
        ui->closeSearch->setStyleSheet("");
        ui->nextSearch->setStyleSheet("");
        ui->previousSearch->setStyleSheet("");
        this->setStyleSheet("");

        emit backgroundColor(QColor(250,250,250));
    }
}

/*
 * This function executes all java scripts in the js directory of the application
 * and applies the selected style.
 */
void GDeskTunes::updateStyle()
{
    if (this->customize)
        applyStyle(this->css);

    if (!style_menu)
    {
        ui->menuBar->setStyleSheet("");
        ui->searchbar->setStyleSheet("");
        ui->search->setStyleSheet("");
        ui->closeSearch->setStyleSheet("");
        ui->nextSearch->setStyleSheet("");
        ui->previousSearch->setStyleSheet("");
        this->setStyleSheet("");
    }

    updateAppearance();
}

void GDeskTunes::finishedLoad(bool ok)
{
    qDebug() << "GDeskTunes::finishedLoad(" << ok << ")";
    if (!ok)
        return;

    QUrl url = ui->webView->url();
    // No customization of anything that is not google
    if (url.host() != "play.google.com")
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

    updateStyle();
}

/*
 * Evaluates the java script in the file located at filePath.
 */
void GDeskTunes::evaluateJavaScriptFile(QString filePath)
{
    // qDebug() << "GDeskTunes::evaluateJavaScriptFile(" << filePath << ")";
    QFile jsFile(filePath);
    jsFile.open(QFile::ReadOnly);
    QTextStream stream(&jsFile);
    GoogleMusicApp *page = dynamic_cast<GoogleMusicApp*>(ui->webView->page());
    page->evaluateJavaScript(stream.readAll());
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
    GoogleMusicApp *page = dynamic_cast<GoogleMusicApp*>(ui->webView->page());
    page->evaluateJavaScript(script);
}

int output(int error_status, const char* error_message, const char* output_string, const char* outfile) {
    if (error_status) {
        if (error_message) {
            fprintf(stderr, "%s", error_message);
        } else {
            fprintf(stderr, "An error occured; no error message available.\n");
        }
        return 1;
    } else if (output_string) {
        if(outfile) {
            FILE* fp = fopen(outfile, "w");
            if(!fp) {
                perror("Error opening output file");
                return 1;
            }
            if(fprintf(fp, "%s", output_string) < 0) {
                perror("Error writing to output file");
                fclose(fp);
                return 1;
            }
            fclose(fp);
        }
        else {
            printf("%s", output_string);
        }
        return 0;
    } else {
        fprintf(stderr, "Unknown internal error.\n");
        return 2;
    }
}

void GDeskTunes::reapplyStyleFile(QString full_css)
{
    bool mini = full_css.contains("mini");
    if (!mini)
    {
        this->disableStyle(this->css, QString::null);
        applyStyleFile(full_css);
    }
}

/*
 * Applies the style with name css and in located in the subdir
 * of the application's directory.
 */
void GDeskTunes::applyStyleFile(QString full_css)
{
    qDebug() << "GDeskTunes::applyStyleFile(" << full_css << ")";
    QFileInfo info(full_css);
    bool mini = full_css.contains("mini");
    if (info.suffix() == "scss")
    {
        ui->actionStyleEditor->setVisible(true);
        QString outfile = info.absoluteDir().absolutePath() + QDir::separator() + "__temp.css";
        struct Sass_File_Context* ctx = sass_make_file_context(full_css.toUtf8().constData());
        struct Sass_Options* options = sass_make_options();
        struct Sass_Context* ctx_out = sass_file_context_get_context(ctx);
        sass_option_set_output_path(options, outfile.toUtf8().constData());
        sass_option_set_input_path(options, full_css.toUtf8().constData());
        sass_file_context_set_options(ctx, options);

        sass_compile_file_context(ctx);

        int ret = output(
            sass_context_get_error_status(ctx_out),
            sass_context_get_error_message(ctx_out),
            sass_context_get_output_string(ctx_out),
            outfile.toUtf8().constData()
        );

        if (ret == 0 && sass_option_get_source_map_file(options)) {
            ret = output(
                sass_context_get_error_status(ctx_out),
                sass_context_get_error_message(ctx_out),
                sass_context_get_source_map_string(ctx_out),
                sass_option_get_source_map_file(options)
            );
        }

        sass_delete_file_context(ctx);

        QFile cssFile(outfile);
        cssFile.open(QFile::ReadOnly);
        QTextStream stream(&cssFile);
        QString css_content = stream.readAll();
        qDebug() << (mini ? "mini" + mini_css : css);
        setStyle( (mini ? "mini" + mini_css : css), css_content);
    }
    else
    {
        ui->actionStyleEditor->setVisible(false);

        QFile cssFile(full_css);
        cssFile.open(QFile::ReadOnly);
        QTextStream stream(&cssFile);
        QString css_content = stream.readAll();
        setStyle( (mini ? "mini" + mini_css : css), css_content);
        qDebug() << (mini ? "mini" + mini_css : css) + css;
    }


    GoogleMusicApp *page = dynamic_cast<GoogleMusicApp*>(ui->webView->page());
    QColor qColor = page->getBackgroundColor();

    QString color = QString().sprintf("rgb(%d,%d,%d)",qColor.red(), qColor.green(), qColor.blue());
    qDebug() << "Toolbar background: " << color;
    ui->toolBar->setStyleSheet("border: 0px; background: " + color);

    QPalette pal = ui->search->palette();

    if (style_menu)
    {
        ui->searchbar->setStyleSheet("background: " + color);
        ui->search->setStyleSheet("background: " + pal.base().color().name());
        ui->closeSearch->setStyleSheet("QPushButton { border:0; background-color: lightgray; }");
        ui->nextSearch->setStyleSheet("QPushButton { border:0; background-color: lightgray; }");
        ui->previousSearch->setStyleSheet("QPushButton { border:0; background-color: lightgray; }");

        ui->menuBar->setStyleSheet("border: 0px; background: " + color );
        int gray = qGray(qColor.rgb());

        if (gray < 128)
        {
            this->setStyleSheet("QMenuBar::item { background-color: transparent; color: white; } QMenu::item { color: lightgray; } QMenu::item::selected { color: white; } QMenu::item::disabled { color: dimgray; } QMenu::item::disabled::selected { color: dimgray; }");
        }
        else
        {
            this->setStyleSheet("QMenuBar::item { background-color: transparent; color: black; } QMenu::item { color: dimgray; } QMenu::item::selected { color: black; }  QMenu::item::disabled { color: lightgray; } QMenu::item::disabled::selected { color: lightgray; }");
        }
    }
    else
    {
        ui->searchbar->setStyleSheet("");
        ui->search->setStyleSheet("");
        ui->closeSearch->setStyleSheet("");
        ui->nextSearch->setStyleSheet("");
        ui->previousSearch->setStyleSheet("");
        ui->menuBar->setStyleSheet("");
        this->setStyleSheet("");
    }

    /*
     * XXX
    int gray = qGray(qColor.rgb());

    if (gray < 128)
    {
        ui->actionSwitch_mini->setIcon(QIcon(":/icons/8x8/close_delete_dark.png"));
    }
    else
    {
        ui->actionSwitch_mini->setIcon(QIcon(":/icons/8x8/close_delete.png"));
    }
    */

    qDebug() << "emit backgroundColor(" << qColor << ")";
    emit backgroundColor(qColor);
}

void GDeskTunes::applyStyle(QString css, QString subdir)
{
    qDebug() << "GDeskTunes::applyStyle(" << css << "," << subdir << ")";
    QString full_css = getStyle(css, subdir);

    // Remove all watched files
    QStringList files = watcher->files();
    for(int i=0; i<files.count(); ++i)
    {
        watcher->removePath(files.at(i));
    }
    if (!css.startsWith("_") && !css.isEmpty())
    {
        watcher->addPath(full_css);
    }

    applyStyleFile(full_css);
}

/*
 * Disables the style
 */
void GDeskTunes::disableStyle(QString css, QString subdir)
{
    qDebug() << "GDeskTunes::disableStyle(" << css << "," << subdir << ")";
    // ui->webView->page()->mainFrame()->evaluateJavaScript(QString("Styles.disableStyle(\"%2%1\")").arg(css, subdir));
    GoogleMusicApp *page = dynamic_cast<GoogleMusicApp*>(ui->webView->page());
    page->evaluateJavaScript(QString("Styles.removeStyle(\"%2%1\")").arg(css, subdir));
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

    if (!isVisible())
    {
        show();
    }

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
        ui->actionSwitch_menu->trigger();
    }
    if (commands.contains("--fullscreen"))
    {
        ui->actionSwitch_Full_Screen->trigger();
    }
    if (commands.contains("--about"))
    {
        ui->actionAbout->trigger();
    }
}

void GDeskTunes::show()
{
    qDebug() << "GDeskTunes::show()";

    if (this->check_update_startup && !updates_checked)
    {
        about(false);
        updates_checked = true;
    }

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

void GDeskTunes::save()
{
    qDebug() << "GDeskTunes::save()";
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    settings.setValue("miniPlayerOnTop", this->mini_player_on_top);
    settings.setValue("customize", this->customize);
    if (!this->css.startsWith("_")) // Sanity check if something went wrong with the styler
    {
        settings.setValue("css", this->css);
    }
    settings.setValue("minicss", this->mini_css);
    settings.setValue("hideMenu", this->ui->menuBar->isHidden());
    settings.setValue("showSidebar", this->show_sidebar);
    settings.setValue("minimizeToTray", this->minimize_to_tray);

    settings.setValue("keeplogo", this->keep_logo);
    settings.setValue("keeplinks", this->keep_links);
    settings.setValue("navigation.buttons", this->navigation_buttons);
    settings.setValue("player.buttons", this->player_buttons);
    settings.setValue("style.menu", this->style_menu);
    settings.setValue("updates.startup", this->check_update_startup);
}

void GDeskTunes::load()
{
    qDebug() << "GDeskTunes::load()";
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    this->setMiniPlayerOnTop(settings.value("miniPlayerOnTop", false).toBool());
    this->setCSS(settings.value("css", "").toString());
    this->setMiniCSS(settings.value("minicss", "Default").toString());
    this->setMenuVisible(!settings.value("hideMenu", false).toBool());
    this->setShowSidebar(settings.value("showSidebar", true).toBool());
    this->setCustomized(settings.value("customize", false).toBool());

    this->setKeepLogo(settings.value("keeplogo", true).toBool());
    this->setKeepLinks(settings.value("keeplinks", true).toBool());
    this->setNavigationButtons(settings.value("navigation.buttons", true).toBool());
    this->setPlayerButtons(settings.value("player.buttons", true).toBool());
    this->setStyleMenu(settings.value("style.menu", false).toBool());
    this->setMinimizeToTray(settings.value("minimizeToTray", false).toBool());

    this->setCheckUpdatesStartup(settings.value("updates.startup", true).toBool());
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
    if (player_buttons)
    {
        css += "#miniButton { width: 12px; height: 12px; margin-right: 0px; display: inline-block; vertical-align: top; background-size: cover; background-image: url(http://radiant-player-mac/images/miniplayer.png); }";
        css += "#miniButton:hover { opacity: 0.6; }";

        css += "#compactButton { width: 12px; height: 12px; margin-right: 6px; padding-right: 0px; display: inline-block; vertical-align: top; background-size: cover; background-image: url(http://radiant-player-mac/images/compactplayer.png); }";
        css += "#compactButton:hover { opacity: 0.6; }";

        css += "#oneGoogleWrapper > div:first-child > div:first-child > div:first-child { padding-right: 4px; }";
    }
    else
    {
        css += "#miniButton { display: none; }";
        css += "#compactButton { display: none; }";
    }
    if (!keep_links)
    {
        css += "#oneGoogleWrapper > div:first-child > div:first-child > div:first-child > div:nth-child(2) { min-width: 0px !important; }";
        css += "#oneGoogleWrapper > div:first-child > div:first-child > div:first-child > div:first-child { display: none; }";
        css += "#oneGoogleWrapper > div:first-child > div:first-child > div:first-child > div:nth-child(2) > div:nth-child(2) { display: none; }";
        css += "#oneGoogleWrapper > div:first-child > div:first-child > div:first-child > div:nth-child(2) > div:nth-child(3) { display: none; }";
        css += "#oneGoogleWrapper > div:first-child > div:first-child > div:first-child > div:nth-child(2) > div:nth-child(4) { display: none; }";
    }
    if (!show_sidebar)
    {
       css += "#nav-container { display: none; }";
       css += "#content-container { margin: 0 0 0 0; }";
    }
    setStyle("gdesktunes.navigation.customization", css);
}

void GDeskTunes::checkFlashPlayer()
{
    if (ui->actionCheck_Flash_Player->isChecked())
    {
        if (isMini())
            setMini(false);

        ui->webView->load(QUrl("http://helpx.adobe.com/flash-player.html"));
        ui->webView->page()->setLinkDelegationPolicy( QWebPage::DelegateAllLinks );
    }
    else
    {
        ui->webView->load(QUrl("https://play.google.com/music/listen#"));
    }
}

void GDeskTunes::setShowSidebar(bool show)
{
    this->show_sidebar = show;
    this->ui->actionView_Sidebar->setText(this->show_sidebar ? "Hide Sidebar" : "Show Sidebar");
}

void GDeskTunes::repaint()
{
    // FIXME: can be more efficient
    float zoomFactor = ui->webView->zoomFactor();
    ui->webView->setZoomFactor(zoomFactor * 0.99);
    ui->webView->setZoomFactor(zoomFactor);
}

void GDeskTunes::viewSidebar()
{
    setShowSidebar(!this->show_sidebar);

    updateAppearance();

    repaint();
}

void GDeskTunes::loadUrl()
{
    ui->webView->load(QUrl("https://play.google.com/music/listen#"));
    // ui->webView->load(QUrl("http://www.last.fm/home"));
}

void GDeskTunes::onLinkClicked(const QUrl &url)
{
    qDebug() << "GDeskTunes::onLinkClicked(" << url << ")";
    if (ui->actionCheck_Flash_Player->isChecked())
    {
        ui->webView->load(url);
    }
}

void GDeskTunes::on_actionFind_triggered()
{
    qDebug() << "GDeskTunes::on_actionFind_triggered()";
    ui->searchbar->setVisible(true);
}

void GDeskTunes::on_closeSearch_clicked()
{
    qDebug() << "GDeskTunes::on_closeSearch_clicked()";
    ui->searchbar->setVisible(false);
}


