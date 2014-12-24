#ifndef ARTLABEL_H
#define ARTLABEL_H

#include <QLabel>

class ArtLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ArtLabel(QWidget *parent = 0);

    void mousePressEvent(QMouseEvent * event);

signals:
    void clicked();

public slots:

public:
    bool event(QEvent *e);
    void setPixmap(const QPixmap &);

    void setLarge(bool l) { this->large = l; }

private:
    QPixmap current_pixmap;
    bool large;

};

#endif // ARTLABEL_H
