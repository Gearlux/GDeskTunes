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
    qDebug() << "requestPixmap(" << id << ")";
    QPixmap result;
    if (id == this->latest_id)
    {
        result = this->latest_pixmap.scaled(requestedSize, Qt::KeepAspectRatio);

        if (size)
            *size = result.size();
    }

    return result;
}

void ImageProvider::addPixmap(const QString &id, QPixmap pixmap)
{
    qDebug() << "addPixmap( " << id << ")";
    this->latest_id = id;
    this->latest_pixmap = pixmap;
}

