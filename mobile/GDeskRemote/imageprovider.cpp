#include "imageprovider.h"

#include <QDebug>

ImageProvider::ImageProvider() :
    QQuickImageProvider(QQuickImageProvider::Pixmap)
{

}

ImageProvider::~ImageProvider()
{

}

QPixmap ImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    qDebug() << "requestPixmap(" << id << "," << (size ? *size : QSize()) << requestedSize << ")";
    QPixmap result;
    if (this->latest_id.endsWith(id))
    {
        qDebug() << "Scaling pixmap";
        if (requestedSize.isValid())
            result = this->latest_pixmap.scaled(requestedSize, Qt::KeepAspectRatio);
        else
            result = this->latest_pixmap;

        if (size)
            *size = this->latest_pixmap.size();
    }

    return result;
}

QImage ImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    qDebug() << "requestImage(" << id << "," << (size ? *size : QSize()) << requestedSize << ")";
    QImage result;
    if (this->latest_id.endsWith(id))
    {
        if (requestedSize.isValid())
            result = this->latest_pixmap.scaled(requestedSize, Qt::KeepAspectRatio).toImage();
        else
            result = this->latest_pixmap.toImage();

        if (size)
            *size = this->latest_pixmap.size();
    }

    return result;
}

void ImageProvider::addPixmap(const QString &id, QPixmap pixmap)
{
    qDebug() << "addPixmap( " << id << ")";
    this->latest_id = id;
    this->latest_pixmap = pixmap;
}

