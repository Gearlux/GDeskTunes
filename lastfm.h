#ifndef LASTFM_H
#define LASTFM_H

#include <QObject>
#include "thirdparty/liblastfm/src/Audioscrobbler.h"

#include "mainwindow.h"

class LastFM : public QObject
{
    Q_OBJECT
public:
    explicit LastFM(MainWindow *parent = 0);

    bool isAuthorized();

signals:
    void changed();

public slots:
    void login();
    void loggedIn();

    void logout();

    void scrobble(QString title, QString artist, QString album, int duration);

private:
    MainWindow* main_window;
    lastfm::Audioscrobbler audioscrobbler;
};

#endif // LASTFM_H
