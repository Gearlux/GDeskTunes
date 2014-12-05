#ifndef GDESKTUNES_H
#define GDESKTUNES_H

#include "mainwindow.h"

class GDeskTunes : public MainWindow
{
    Q_OBJECT

public:
    explicit GDeskTunes(QWidget *parent = 0);

signals:
    void keepLogo(bool keep);
    void navigationButtons(bool buttons);
    void keepLinks(bool keep);

public slots:
    void finishedLoad(bool ok);

    void switchMini();
    void receiveMessage(const QString &msg);

    void save();
    void load();

    void saveState();
    void show();

    void setKeepLogo(bool keep) { if (keep == keep_logo) return; keep_logo = keep; emit keepLogo(keep); }
    void setNavigationButtons(bool buttons) { if (navigation_buttons == buttons) return; navigation_buttons = buttons; emit navigationButtons(buttons); }
    void setKeepLinks(bool keep) { if (keep == keep_links) return; keep_links = keep; emit keepLinks(keep); }

    void updateAppearance();

public:
    void setCSS(QString css);
    void setMiniCSS(QString css);
    void setMini(bool toMini);
    void setCustomize(bool customize);
    void setMiniPlayerOnTop(bool on_top);

    QString getCSS() { return this->css; }
    QString getMiniCSS() { return this->mini_css; }
    bool isMini() { return this->mini; }
    bool isCustomized() { return this->customize; }
    bool isMiniPlayerOnTop() { return this->mini_player_on_top; }

    void restore();
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

    Qt::WindowFlags normal_flags;
};

#endif // GDESKTUNES_H
