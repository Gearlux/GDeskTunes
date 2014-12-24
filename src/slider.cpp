#include "slider.h"
#include <QStyleOption>
#include <QMouseEvent>

Slider::Slider(QWidget *parent) : QSlider(parent)
{

}

Slider::Slider(Qt::Orientation orientation, QWidget *parent) : QSlider(orientation, parent)
{

}

void Slider::mousePressEvent(QMouseEvent * event )
{
  QStyleOptionSlider opt;
  initStyleOption(&opt);
  QRect sr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

  if (event->button() == Qt::LeftButton &&
      sr.contains(event->pos()) == false)
  {
    int newVal;
    if (orientation() == Qt::Vertical)
       newVal = minimum() + ((maximum()-minimum()) * (height()-event->y())) / height();
    else
       newVal = minimum() + ((maximum()-minimum()) * event->x()) / width();

    if (invertedAppearance() == true)
        setValue( maximum() - newVal );
    else
        setValue(newVal);

    event->accept();
  }
  QSlider::mousePressEvent(event);
}

Slider::~Slider()
{

}

