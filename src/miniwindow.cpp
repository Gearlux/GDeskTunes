#include "miniwindow.h"
#include "ui_miniwindow.h"

MiniWindow::MiniWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MiniWindow)
{
    ui->setupUi(this);
}

MiniWindow::~MiniWindow()
{
    delete ui;
}
