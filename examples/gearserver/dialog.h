#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

class QLabel;
class QPushButton;
class FortuneServer;
class Protocol;

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = 0);
    ~Dialog();

private:
    QLabel *statusLabel;
    QPushButton *quitButton;
    FortuneServer *tcpServer;
};

#endif // DIALOG_H
