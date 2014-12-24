#include "artlabel.h"

#include <QEvent>
#include <QPainter>
#include <QIcon>
#include <QMouseEvent>

ArtLabel::ArtLabel(QWidget *parent) :
    QLabel(parent),
    large(false)
{
    setAttribute(Qt::WA_Hover);
}

void ArtLabel::mousePressEvent(QMouseEvent *event)
{
    emit clicked();
    event->accept();
}

void ArtLabel::setPixmap(const QPixmap &pm)
{
    current_pixmap = pm.scaled(width(), height());
    QLabel::setPixmap(pm);
}

bool ArtLabel::event(QEvent *e)
{
    if (e->type() == QEvent::HoverLeave)
    {
        QLabel::setPixmap(current_pixmap.scaled(width(), height()));
    }
    if (e->type() == QEvent::HoverEnter)
    {
        QPixmap result(width(), height());
        QPainter painter(&result);
        painter.fillRect(0, 0, width(), height(), Qt::black);
        painter.drawPixmap(0, 0, current_pixmap);
        if (large)
        {
            QPixmap overlay(":/icons/64x64/small_art.png");
            painter.drawPixmap(0, 0, overlay.scaled(width(), height()));
        }
        else
        {
            QPixmap overlay(":/icons/64x64/large_art.png");
            painter.drawPixmap(3, 3, overlay.scaled(width()-6, height()-6));
        }
        painter.end();
        QLabel::setPixmap(result);
    }
    return QLabel::event(e);
}
