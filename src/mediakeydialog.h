#ifndef MEDIAKEYDIALOG_H
#define MEDIAKEYDIALOG_H

#include <QDialog>

namespace Ui {
class MediaKeyDialog;
}

class MediaKeyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MediaKeyDialog(QWidget *parent = 0);
    ~MediaKeyDialog();

public:
    Ui::MediaKeyDialog *ui;
};

#endif // MEDIAKEYDIALOG_H
