#ifndef GDESKSTYLER_H
#define GDESKSTYLER_H

#include <QMainWindow>

namespace Ui {
class GDeskStyler;
}

class GDeskStyler : public QMainWindow
{
    Q_OBJECT

public:
    explicit GDeskStyler(QWidget *parent = 0);
    ~GDeskStyler();

private:
    Ui::GDeskStyler *ui;
};

#endif // GDESKSTYLER_H
