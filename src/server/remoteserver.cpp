#include "remoteserver.h"

RemoteServer::RemoteServer() :
    Server(),
    state_playing(-1),
    state_repeat(QString::null),
    state_shuffle(QString::null)
{

}

RemoteServer::~RemoteServer()
{

}

void RemoteServer::onClientPlay()
{
    qDebug() << "RemoteServer::onClientPlay()";
    emit play();
}

void RemoteServer::onClientNext()
{
    qDebug() << "RemoteServer::onClientNext()";
    emit next();
}

void RemoteServer::onClientPrevious()
{
    qDebug() << "RemoteServer::onClientPrevious()";
    emit previous();
}

void RemoteServer::onClientInfo()
{
    qDebug() << "RemoteServer::onClientInfo()";
    connections_mutex.lock();
    for(int i=0; i<connections.count();++i)
    {
        Protocol* proto = connections.at(i);
        proto->invokeMethod("onServerNowPlaying", state_title, state_artist, state_album);
        proto->invokeMethod("onServerPlaying", state_playing);
        proto->invokeMethod("onServerAlbumArt", state_url, state_pixmap);
        proto->invokeMethod("onServerRepeat", state_repeat);
        proto->invokeMethod("onServerShuffle", state_shuffle);
    }
    connections_mutex.unlock();
}

void RemoteServer::onClientVolume(int vol)
{
    qDebug() << "RemoteServer::onClientVolume(" << vol << ")";
    emit volume(vol);
}

void RemoteServer::isPlaying(int playing)
{
    if (playing == state_playing) return;
    state_playing = playing;
    connections_mutex.lock();
    for(int i=0; i<connections.count();++i)
    {
        Protocol* proto = connections.at(i);
        proto->invokeMethod("onServerPlaying", playing);
    }
    connections_mutex.unlock();

}

void RemoteServer::nowPlaying(QString title, QString artist, QString album, QString art, int duration)
{
    qDebug() << "RemoteServer::nowPlaying(" << title << "," << artist << "," << album << "," << art << "," << duration << ")";
    state_title = title;
    state_artist = artist;
    state_album = album;
    connections_mutex.lock();
    for(int i=0; i<connections.count();++i)
    {
        Protocol* proto = connections.at(i);
        proto->invokeMethod("onServerNowPlaying", title, artist, album);
    }
    connections_mutex.unlock();
}

void RemoteServer::albumArt(QString url, QPixmap pixmap)
{
    qDebug() << "RemoteServer::albumArt(" << url << "," << pixmap << ")";
    if (url == state_url) return;
    state_url = url;
    state_pixmap = pixmap;
    connections_mutex.lock();
    for(int i=0; i<connections.count();++i)
    {
        Protocol* proto = connections.at(i);
        proto->invokeMethod("onServerAlbumArt", url, pixmap);
    }
    connections_mutex.unlock();

}


void RemoteServer::repeat(QString mode)
{
    if (state_repeat == mode) return;
    state_repeat = mode;
    connections_mutex.lock();
    for(int i=0; i<connections.count();++i)
    {
        Protocol* proto = connections.at(i);
        proto->invokeMethod("onServerRepeat", mode);
    }
    connections_mutex.unlock();
}

void RemoteServer::shuffle(QString mode)
{
    if (state_shuffle == mode) return;
    state_shuffle = mode;
    connections_mutex.lock();
    for(int i=0; i<connections.count();++i)
    {
        Protocol* proto = connections.at(i);
        proto->invokeMethod("onServerShuffle", mode);
    }
    connections_mutex.unlock();
}

