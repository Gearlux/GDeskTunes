#include "engine.h"
#include <QDebug>

Engine::Engine(const QUrl &url, QObject *parent) :
    QQmlApplicationEngine(url, parent)
{

}

Engine::~Engine()
{

}

void Engine::addImageProvider(const QString &id, QQmlImageProviderBase *ip)
{
    qDebug() << "addImageProvider(" << id << "," << ip << ")";
    QQmlApplicationEngine::addImageProvider(id, ip);

    image_providers[id] = ip;
}

QQmlImageProviderBase* Engine::getImageProvider(const QString &id)
{
    qDebug() << "getImageProvider(" << id << ")";
    return image_providers[id];
}

