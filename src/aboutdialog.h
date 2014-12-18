#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QNetworkReply>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

public slots:
    void donate();

    void checkVersion(QNetworkReply *reply);

public:
    void keyPressEvent(QKeyEvent *event);
    void setText(QString msg, QString link);

private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
