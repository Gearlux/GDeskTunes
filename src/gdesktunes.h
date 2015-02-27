#ifndef GDESKTUNES_H
#define GDESKTUNES_H

#include "mainwindow.h"

class GDeskTunes : public MainWindow
{
    Q_OBJECT

public:
    explicit GDeskTunes(QWidget *parent = 0);

signals:
    void miniPlayerOnTop(bool top);
    void customized(bool custom);
    void keepLogo(bool keep);
    void navigationButtons(bool buttons);
    void playerButtons(bool buttons);
    void styleMenu(bool style);
    void keepLinks(bool keep);
    void CSS(QString style);
    void miniCSS(QString style);
    void checkUpdatesStartup(bool check);
    void backgroundColor(QColor color);

public slots:
    void finishedLoad(bool ok);

    void switchMini();
    void viewSidebar();
    void receiveMessage(const QString &msg);

    void save();
    void load();

    void show();

    void setMiniPlayerOnTop(bool on_top);
    void setCustomized(bool customize);

    void setKeepLogo(bool keep) { if (keep == keep_logo) return; keep_logo = keep; emit keepLogo(keep); }
    void setNavigationButtons(bool buttons) { if (navigation_buttons == buttons) return; navigation_buttons = buttons; emit navigationButtons(buttons); }
    void setPlayerButtons(bool buttons) { if (player_buttons == buttons) return; player_buttons = buttons; emit playerButtons(buttons); }
    void setStyleMenu(bool style) {  if (style_menu == style) return; style_menu = style; emit styleMenu(style); }
    void setKeepLinks(bool keep) { if (keep == keep_links) return; keep_links = keep; emit keepLinks(keep); }
    void setCheckUpdatesStartup(bool set) { if (check_update_startup == set) return; check_update_startup = set; emit checkUpdatesStartup(set); }

    void updateAppearance();
    void updateStyle();

    void setCSS(QString css);
    void setMiniCSS(QString css);

    void loadUrl();
    void checkFlashPlayer();

    void repaint();

public:
    void setShowSidebar(bool show);
    void setMini(bool toMini);

    bool isMini() { return this->mini; }

    void restoreMini();

private:
    void evaluateJavaScriptFile(QString filePath);
    void applyStyle(QString css, QString subdir = QString::null);
    void disableStyle(QString css, QString subdir = QString::null);
    void setStyle(QString name, QString css_content);

    static QString getStyle(QString name, QString subdir = QString::null);
    Qt::WindowFlags miniPlayerFlags();

private:
    QString css;
    QString mini_css;
    bool mini;
    bool customize;
    bool mini_player_on_top;

    bool keep_logo;
    bool keep_links;
    bool navigation_buttons;
    bool player_buttons;
    bool style_menu;
    bool show_sidebar;

    bool check_update_startup;
    bool updates_checked;

    Qt::WindowFlags normal_flags;
};

#endif // GDESKTUNES_H
