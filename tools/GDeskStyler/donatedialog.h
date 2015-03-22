#ifndef DONATEDIALOG_H
#define DONATEDIALOG_H

#include <QDialog>

namespace Ui {
class DonateDialog;
}

class DonateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DonateDialog(QWidget *parent = 0);
    ~DonateDialog();

private:
    Ui::DonateDialog *ui;
};

#endif // DONATEDIALOG_H
