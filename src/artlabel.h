#ifndef ARTLABEL_H
#define ARTLABEL_H

#include <QLabel>

class ArtLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ArtLabel(QWidget *parent = 0);

    void mouseDoubleClickEvent(QMouseEvent * event);

signals:
    void clicked();

public slots:

};

#endif // ARTLABEL_H
