#include "googlemusicapp.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QWebFrame>

GoogleMusicApp::GoogleMusicApp(MainWindow *parent) :
    QObject(parent),
    main_window(parent)
{
}

void GoogleMusicApp::increaseVolume()
{
    qDebug() << "increaseVolume";
    main_window->webView()->page()->mainFrame()->evaluateJavaScript("MusicAPI.Volume.increaseVolume(10);");
}

void GoogleMusicApp::decreaseVolume()
{
    qDebug() << "decreaseVolume";
    main_window->webView()->page()->mainFrame()->evaluateJavaScript("MusicAPI.Volume.decreaseVolume(10);");
}

void GoogleMusicApp::playbackChanged(int mode)
{
    main_window->ui->actionNext->setDisabled(mode == 0);
    main_window->ui->actionStop->setDisabled(mode == 0);
    main_window->ui->actionPrevious->setDisabled(mode == 0);
}

void GoogleMusicApp::ratingChanged(int rating)
{
    qDebug() << "Rating" << rating;
}

void GoogleMusicApp::playbackTimeChanged(int current, int total)
{

}

void GoogleMusicApp::repeatChanged(QString mode)
{
    qDebug() << mode << getRepeat();
    main_window->repeat_all->setChecked(mode == "LIST_REPEAT");
    main_window->repeat_one->setChecked(mode == "SINGLE_REPEAT");
    main_window->repeat_off->setChecked(mode == "NO_REPEAT");
}

void GoogleMusicApp::shuffleChanged(QString mode)
{
    qDebug() << mode << getShuffle();
    main_window->shuffle_on->setChecked(mode == "ALL_SHUFFLE");
    main_window->shuffle_off->setChecked(mode == "NO_SHUFFLE");
}

QString GoogleMusicApp::getShuffle()
{
    return main_window->webView()->page()->mainFrame()->evaluateJavaScript("MusicAPI.Playback.getShuffle()").toString();
}

void GoogleMusicApp::shuffleOff()
{
   main_window->webView()->page()->mainFrame()->evaluateJavaScript("MusicAPI.Playback.changeShuffle('NO_SHUFFLE')");
}

void GoogleMusicApp::shuffleOn()
{
   main_window->webView()->page()->mainFrame()->evaluateJavaScript("MusicAPI.Playback.changeShuffle('ALL_SHUFFLE')");
}

QString GoogleMusicApp::getRepeat()
{
    return main_window->webView()->page()->mainFrame()->evaluateJavaScript("MusicAPI.Playback.getRepeat()").toString();
}

void GoogleMusicApp::repeatOff()
{
    main_window->webView()->page()->mainFrame()->evaluateJavaScript("MusicAPI.Playback.changeRepeat('NO_REPEAT')");
}

void GoogleMusicApp::repeatAll()
{
    main_window->webView()->page()->mainFrame()->evaluateJavaScript("MusicAPI.Playback.changeRepeat('LIST_REPEAT')");
}

void GoogleMusicApp::repeatOne()
{
    main_window->webView()->page()->mainFrame()->evaluateJavaScript("MusicAPI.Playback.changeRepeat('SINGLE_REPEAT')");
}

void GoogleMusicApp::notifySong(QString title, QString artist, QString album, QString art, int duration)
{
    qDebug() << "Title: " << title;
    qDebug() << "Artist: " << artist;
    qDebug() << "Album: " << album;
    qDebug() << "Art: " << art;
    qDebug() << "Duration: " << duration;

    emit nowPlaying(title, artist, album, duration);
}



