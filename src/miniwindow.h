#ifndef MINIWINDOW_H
#define MINIWINDOW_H

#include <QMainWindow>

namespace Ui {
class MiniWindow;
}

class MiniWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MiniWindow(QWidget *parent = 0);
    ~MiniWindow();

private:
    Ui::MiniWindow *ui;
};

#endif // MINIWINDOW_H
