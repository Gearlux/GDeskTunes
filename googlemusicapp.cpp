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
    if (current_rating != rating)
    {
        qDebug() << "Rating" << rating;
        current_rating = rating;
        if (rating > 3)
        {
            emit love(current_title, current_artist, current_album);
        }
        else
        {
            emit unlove(current_title, current_artist, current_album);
        }
    }
}

void GoogleMusicApp::playbackTimeChanged(int current, int total)
{

}

void GoogleMusicApp::repeatChanged(QString mode)
{
    if (current_repeat != mode)
    {
        qDebug() << mode << getRepeat();
        emit repeat(mode);
        current_repeat = mode;
    }
}

void GoogleMusicApp::shuffleChanged(QString mode)
{
    if (current_shuffle != mode)
    {
        qDebug() << mode << getShuffle();
        emit shuffle(mode);
        current_shuffle = mode;
    }
}

QString GoogleMusicApp::getShuffle()
{
    return main_window->webView()->page()->mainFrame()->evaluateJavaScript("MusicAPI.Playback.getShuffle()").toString();
}

void GoogleMusicApp::on_shuffle_off_triggered()
{
   main_window->webView()->page()->mainFrame()->evaluateJavaScript("MusicAPI.Playback.changeShuffle('NO_SHUFFLE')");
}

void GoogleMusicApp::on_shuffle_on_triggered()
{
   main_window->webView()->page()->mainFrame()->evaluateJavaScript("MusicAPI.Playback.changeShuffle('ALL_SHUFFLE')");
}

QString GoogleMusicApp::getRepeat()
{
    return main_window->webView()->page()->mainFrame()->evaluateJavaScript("MusicAPI.Playback.getRepeat()").toString();
}

void GoogleMusicApp::on_repeat_off_triggered()
{
    main_window->webView()->page()->mainFrame()->evaluateJavaScript("MusicAPI.Playback.changeRepeat('NO_REPEAT')");
}

void GoogleMusicApp::on_repeat_all_triggered()
{
    main_window->webView()->page()->mainFrame()->evaluateJavaScript("MusicAPI.Playback.changeRepeat('LIST_REPEAT')");
}

void GoogleMusicApp::on_repeat_one_triggered()
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

    current_title = title;
    current_artist = artist;
    current_album = album;

    current_rating = -1;

    emit nowPlaying(title, artist, album, duration);
}

void GoogleMusicApp::loadFinished(bool status)
{
    emit shuffle(getShuffle());
    emit repeat(getRepeat());
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
void GoogleMusicApp::addWindowObjects()
{
    qDebug() << "GoogleMusicApp::addWindowObjects()";
    // FIXME
    MainWindow *w = (MainWindow*)parent();
    w->webView()->page()->mainFrame()->addToJavaScriptWindowObject("GoogleMusicApp", this);
}
#endif




