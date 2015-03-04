#include "gdeskstyler.h"
#include "ui_gdeskstyler.h"

GDeskStyler::GDeskStyler(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GDeskStyler)
{
    ui->setupUi(this);
}

GDeskStyler::~GDeskStyler()
{
    delete ui;
}
