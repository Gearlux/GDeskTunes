#ifndef SLIDER_H
#define SLIDER_H

#include <QSlider>

class Slider : public QSlider
{
    Q_OBJECT

public:
    Slider(QWidget *parent = 0);
    Slider(Qt::Orientation orientation, QWidget * parent = 0);
    ~Slider();

    void mousePressEvent(QMouseEvent * event );
};

#endif // SLIDER_H
