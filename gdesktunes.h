#ifndef GDESKTUNES_H
#define GDESKTUNES_H

#include "mainwindow.h"

class GDeskTunes : public MainWindow
{
    Q_OBJECT

public:
    explicit GDeskTunes(QWidget *parent = 0);

public slots:
    void finishedLoad(bool ok);

    void switchMini();
    void receiveMessage(const QString &msg);

    void save();
    void load();

    void saveState();
    void show();

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

    static QString getStyle(QString name, QString subdir = QString::null);
    Qt::WindowFlags miniPlayerFlags();

private:
    QString css;
    QString mini_css;
    bool mini;
    bool customize;
    bool mini_player_on_top;

    Qt::WindowFlags normal_flags;
};

#endif // GDESKTUNES_H
