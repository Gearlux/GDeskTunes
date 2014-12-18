#include "artlabel.h"

ArtLabel::ArtLabel(QWidget *parent) :
    QLabel(parent)
{
}

void ArtLabel::mouseDoubleClickEvent(QMouseEvent * event)
{
    emit clicked();
}
