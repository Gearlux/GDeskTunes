// #define QT_NO_DEBUG_OUTPUT

#include "googlemusicapp.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "networkmanager.h"

#include <QWebFrame>
#include <QWebElement>
#include <QDebug>

GoogleMusicApp::GoogleMusicApp(QObject *parent) :
    QWebPage(parent)
{
    QObject::connect(mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(addWindowObjects()));
}

void GoogleMusicApp::increaseVolume()
{
    qDebug() << "increaseVolume";
    mainFrame()->evaluateJavaScript("MusicAPI.Volume.increaseVolume(10);");
}

void GoogleMusicApp::decreaseVolume()
{
    qDebug() << "decreaseVolume";
    mainFrame()->evaluateJavaScript("MusicAPI.Volume.decreaseVolume(10);");
}

void GoogleMusicApp::playbackChanged(int mode)
{
    qDebug() << "GoogleMusicApp::playbackChanged(" << mode << ")";
    emit isPlaying(mode);
}

void GoogleMusicApp::rewindChanged(int mode)
{
    qDebug() << "GoogleMusicApp::rewindChanged(" << mode << ")";
    emit rewindEnabled(mode);
}

void GoogleMusicApp::forwardChanged(int mode)
{
    qDebug() << "GoogleMusicApp::forwardChanged(" << mode << ")";
    emit forwardEnabled(mode);
}

void GoogleMusicApp::ratingChanged(int rate)
{
    if (current_rating != rate)
    {
        qDebug() << "Rating" << rate;
        current_rating = rate;
        if (rate > 3)
        {
            emit love(current_title, current_artist, current_album);
        }
        else
        {
            emit unlove(current_title, current_artist, current_album);
        }
    }
    emit rating(rate);
}

void GoogleMusicApp::playbackTimeChanged(int current, int total)
{
    emit playbackTime(current, total);
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
    return mainFrame()->evaluateJavaScript("MusicAPI.Playback.getShuffle()").toString();
}

void GoogleMusicApp::toggleShuffle()
{
    qDebug() << "GoogleMusicApp::toggleShuffle()";
    mainFrame()->evaluateJavaScript("MusicAPI.Playback.toggleShuffle()");
}

void GoogleMusicApp::shuffleOff()
{
   mainFrame()->evaluateJavaScript("MusicAPI.Playback.changeShuffle('NO_SHUFFLE')");
}

void GoogleMusicApp::shuffleOn()
{
   mainFrame()->evaluateJavaScript("MusicAPI.Playback.changeShuffle('ALL_SHUFFLE')");
}

QString GoogleMusicApp::getRepeat()
{
    return mainFrame()->evaluateJavaScript("MusicAPI.Playback.getRepeat()").toString();
}

void GoogleMusicApp::toggleRepeat()
{
    qDebug() << "GoogleMusicApp::toggleRepeat()";
    mainFrame()->evaluateJavaScript("MusicAPI.Playback.toggleRepeat()");
}

void GoogleMusicApp::repeatOff()
{
    mainFrame()->evaluateJavaScript("MusicAPI.Playback.changeRepeat('NO_REPEAT')");
}

void GoogleMusicApp::repeatAll()
{
    mainFrame()->evaluateJavaScript("MusicAPI.Playback.changeRepeat('LIST_REPEAT')");
}

void GoogleMusicApp::repeatOne()
{
    mainFrame()->evaluateJavaScript("MusicAPI.Playback.changeRepeat('SINGLE_REPEAT')");
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

    int rate = getRating();
    emit rating(rate);

    FileDownloader *loader = new FileDownloader(art, this);
    connect(loader, SIGNAL(downloaded(QByteArray &)), this, SLOT(onDownloaded(QByteArray &)));
}

void GoogleMusicApp::onDownloaded(QByteArray &data)
{
    QPixmap pixmap;
    pixmap.loadFromData(data);
    sender()->deleteLater();

    emit albumArt(pixmap);
}

void GoogleMusicApp::loadFinished(bool status)
{
    if (status)
    {
        emit shuffle(getShuffle());
        emit repeat(getRepeat());
    }
}

void GoogleMusicApp::addWindowObjects()
{
    qDebug() << "GoogleMusicApp::addWindowObjects()";
    mainFrame()->addToJavaScriptWindowObject("GoogleMusicApp", this);
}

void GoogleMusicApp::play()
{
    qDebug() << "GoogleMusicApp::play()";
    QWebElement elt = mainFrame()->findFirstElement("*[data-id='play-pause']");
    elt.evaluateJavaScript("this.click();");
}

void GoogleMusicApp::next()
{
    qDebug() << "GoogleMusicApp::next()";
    QWebElement elt = mainFrame()->findFirstElement("*[data-id='forward']");
    elt.evaluateJavaScript("this.click();");
}

void GoogleMusicApp::previous()
{
    qDebug() << "GoogleMusicApp::previous()";
    QWebElement elt = mainFrame()->findFirstElement("*[data-id='rewind']");
    elt.evaluateJavaScript("this.click();");
}

void GoogleMusicApp::toggleThumbsUp()
{
    qDebug() << "GoogleMusicApp::toggleThumbsUp()";
    mainFrame()->evaluateJavaScript("MusicAPI.Rating.toggleThumbsUp()");
}

void GoogleMusicApp::toggleThumbsDown()
{
    qDebug() << "GoogleMusicApp::toggleThumbsDown()";
    mainFrame()->evaluateJavaScript("MusicAPI.Rating.toggleThumbsDown()");
}

int GoogleMusicApp::getRating()
{
    return mainFrame()->evaluateJavaScript("MusicAPI.Rating.getRating()").toInt();
}

void GoogleMusicApp::changePlaybackTime(int ms)
{
    qDebug() << "GoogleMusicApp::changePlaybackTime(" << ms << ")";
    mainFrame()->evaluateJavaScript(QString("MusicAPI.Playback.setPlaybackTime(%1)").arg(ms));
}


