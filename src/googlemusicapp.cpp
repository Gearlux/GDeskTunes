#define QT_NO_DEBUG_OUTPUT

#include "googlemusicapp.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "networkmanager.h"

#include <QWebFrame>
#include <QWebElement>
#include <QDebug>

GoogleMusicApp::GoogleMusicApp(QObject *parent) :
#ifdef USE_WEBKIT
    QWebPage(parent),
#else
    QWebEnginePage(parent),
#endif
    current_title(QString::null),
    current_artist(QString::null),
    current_album(QString::null),
    current_art(QString::null),
    current_repeat(QString::null),
    current_shuffle(QString::null)
{
#ifdef USE_WEBKIT
    QObject::connect(mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(addWindowObjects()));
#else
    qWarning() << "No feedback from google to the application";
#endif
}

int GoogleMusicApp::getVolume()
{
    qDebug() << "GoogleMusicApp::getVolume()";
    return evaluateJavaScript("MusicAPI.Volume.getVolume();").toInt();
}

void GoogleMusicApp::increaseVolume()
{
    qDebug() << "GoogleMusicApp::increaseVolume()";
    evaluateJavaScript("MusicAPI.Volume.increaseVolume(10);");
}

void GoogleMusicApp::decreaseVolume()
{
    qDebug() << "GoogleMusicApp::decreaseVolume()";
    evaluateJavaScript("MusicAPI.Volume.decreaseVolume(10);");
}

void GoogleMusicApp::volChanged(int vol)
{
    qDebug() << "GoogleMusicApp::volChanged(" << vol << ")";
    emit volume(vol);
}

void GoogleMusicApp::volumeChanged(int vol)
{
    qDebug() << "GoogleMusicApp::volumeChanged(" << vol << ")";
    evaluateJavaScript(QString("MusicAPI.Volume.setVolume(%1);").arg(vol));
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
    return evaluateJavaScript("MusicAPI.Playback.getShuffle()").toString();
}

void GoogleMusicApp::toggleShuffle()
{
    qDebug() << "GoogleMusicApp::toggleShuffle()";
    evaluateJavaScript("MusicAPI.Playback.toggleShuffle()");
}

void GoogleMusicApp::shuffleOff()
{
   evaluateJavaScript("MusicAPI.Playback.changeShuffle('NO_SHUFFLE')");
}

void GoogleMusicApp::shuffleOn()
{
   evaluateJavaScript("MusicAPI.Playback.changeShuffle('ALL_SHUFFLE')");
}

QString GoogleMusicApp::getRepeat()
{
    return evaluateJavaScript("MusicAPI.Playback.getRepeat()").toString();
}

void GoogleMusicApp::toggleRepeat()
{
    qDebug() << "GoogleMusicApp::toggleRepeat()";
    evaluateJavaScript("MusicAPI.Playback.toggleRepeat()");
}

void GoogleMusicApp::repeatOff()
{
    evaluateJavaScript("MusicAPI.Playback.changeRepeat('NO_REPEAT')");
}

void GoogleMusicApp::repeatAll()
{
    evaluateJavaScript("MusicAPI.Playback.changeRepeat('LIST_REPEAT')");
}

void GoogleMusicApp::repeatOne()
{
    evaluateJavaScript("MusicAPI.Playback.changeRepeat('SINGLE_REPEAT')");
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
    current_art = art;

    current_rating = -1;

    emit nowPlaying(title, artist, album, art, duration);

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

    emit albumArt(current_art, pixmap);
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
#ifdef USE_WEBKIT
    qDebug() << "GoogleMusicApp::addWindowObjects()";
    mainFrame()->addToJavaScriptWindowObject("GoogleMusicApp", this);
#else
    qWarning() << "Do this with some polling mechanism";
#endif
}

void GoogleMusicApp::play()
{
    qDebug() << "GoogleMusicApp::play()";
    evaluateJavaScript("MusicAPI.Playback.playPause()");
    // QWebElement elt = mainFrame()->findFirstElement("*[data-id='play-pause']");
    // elt.evaluateJavaScript("this.click();");
}

void GoogleMusicApp::next()
{
    qDebug() << "GoogleMusicApp::next()";
    //QWebElement elt = mainFrame()->findFirstElement("*[data-id='forward']");
    //elt.evaluateJavaScript("this.click();");
    evaluateJavaScript("MusicAPI.Playback.forward()");
}

void GoogleMusicApp::previous()
{
    qDebug() << "GoogleMusicApp::previous()";
    // QWebElement elt = mainFrame()->findFirstElement("*[data-id='rewind']");
    // elt.evaluateJavaScript("this.click();");
    evaluateJavaScript("MusicAPI.Playback.rewind()");

}

void GoogleMusicApp::toggleThumbsUp()
{
    qDebug() << "GoogleMusicApp::toggleThumbsUp()";
    evaluateJavaScript("MusicAPI.Rating.toggleThumbsUp()");
}

void GoogleMusicApp::toggleThumbsDown()
{
    qDebug() << "GoogleMusicApp::toggleThumbsDown()";
    evaluateJavaScript("MusicAPI.Rating.toggleThumbsDown()");
}

int GoogleMusicApp::getRating()
{
    return evaluateJavaScript("MusicAPI.Rating.getRating()").toInt();
}

void GoogleMusicApp::changePlaybackTime(int ms)
{
    qDebug() << "GoogleMusicApp::changePlaybackTime(" << ms << ")";
    evaluateJavaScript(QString("MusicAPI.Playback.setPlaybackTime(%1)").arg(ms));
}

void GoogleMusicApp::on_miniButton_clicked()
{
    qDebug() << "GoogleMusicApp::on_miniButton_clicked()";
    emit switchToMiniPlayer();
}

void GoogleMusicApp::on_compactButton_clicked()
{
    qDebug() << "GoogleMusicApp::on_compactButton_clicked()";
    emit switchToCompactPlayer();
}

int GoogleMusicApp::getBodyWidth()
{
#ifdef USE_WEBKIT
    QWebElement elt = mainFrame()->documentElement().findFirst("body");
    QString width = elt.styleProperty("width", QWebElement::ComputedStyle);
    int w = width.replace("px", "").toInt(0);
    return w;
#else
    qWarning() << "Not implemented: GoogleMusicApp::getBodyWidth()";
    return 150;
#endif
}

int GoogleMusicApp::getBodyHeight()
{
#ifdef USE_WEBKIT
    QWebElement elt = mainFrame()->documentElement().findFirst("body");
    QString height = elt.styleProperty("height", QWebElement::ComputedStyle);
    int h = height.replace("px", "").toInt(0);
    return h;
#else
    qWarning() << "Not implemented: GoogleMusicApp::getBodyHeight()";
    return 150;
#endif
}

QVariant GoogleMusicApp::evaluateJavaScript(const QString &script)
{
#ifdef USE_WEBKIT
    return mainFrame()->evaluateJavaScript(script);
#else
    QVariant result;
    runJavaScript(script, [](const QVariant &result) { qDebug() << result; });
    return result;
#endif
}

QColor GoogleMusicApp::getBackgroundColor()
{
#ifdef USE_WEBKIT
    QWebElement elt = mainFrame()->documentElement().findFirst("#main");

    QString color = elt.styleProperty("background-color", QWebElement::ComputedStyle);
    // QString color = elt.styleProperty("background-color", QWebElement::CascadedStyle);
#else
    QString color("rgb(250,250,250)");
#endif
    qDebug() << "GoogleMusicApp::getBackgroundColor()" << color;

    QRegExp rx("\\d+");
    QList<int> nums;
    int pos = 0;
    while((pos = rx.indexIn(color, pos)) != -1)
    {
        nums.append( color.mid(pos, rx.matchedLength()).toInt());
        pos += rx.matchedLength();
    }

    int r = 250, g = 250, b = 250, a = 255;
    if (nums.size() >= 3)
    {
        r = nums.at(0);
        g = nums.at(1);
        b = nums.at(2);
    }
    if (nums.size() > 3)
        a = nums.at(3);

    QColor qColor = QColor(r,g,b);
    if (a == 0)
        qColor = QColor(250,250,250);

    qDebug()  << "GoogleMusicApp::getBackgroundColor()" << qColor;
    return qColor;
}

void GoogleMusicApp::info()
{
    emit volume(this->getVolume());
}
