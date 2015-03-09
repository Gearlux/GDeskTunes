#ifndef GDESKSTYLER_H
#define GDESKSTYLER_H

#include <QMainWindow>
#include <QMetaProperty>
#include <QDebug>

#include "gstyle.h"

namespace Ui {
class GDeskStyler;
}

// Forward declarations
class QtProperty;
class QtStringPropertyManager;
class QtColorPropertyManager;
class QtIntPropertyManager;
class QtGroupPropertyManager;
class QtBoolPropertyManager;

class GDeskStyler : public QMainWindow
{
    Q_OBJECT

public:
    explicit GDeskStyler(QWidget *parent = 0);
    ~GDeskStyler();

    void setGDeskTunes(QObject *gdesktunes);


private slots:
    void valueChanged(QtProperty *property, QString value);
    void valueChanged(QtProperty *property, QColor value);
    void valueChanged(QtProperty *property, int value);
    void valueChanged(QtProperty *property, bool value);

    void on_actionNew_triggered();
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();
    void on_actionExit_triggered();

    void on_actionGenerate_triggered();

    void on_actionOpen_triggered();

private:
    void save(QString filename);
    void load(QString filename);
    void test();
    void ask();
    void populate(QObject *style, QtProperty *parent=0);

private:
    QObject *gdesktunes;
    Ui::GDeskStyler *ui;

    GStyle *style;

    QMap<QtProperty*,QObject*> holders;

    QString css;
    QString filename;
    bool modified;

    QtStringPropertyManager *stringManager;
    QtColorPropertyManager* colorManager;
    QtIntPropertyManager* intManager;
    QtGroupPropertyManager* groupManager;
    QtBoolPropertyManager* boolManager;

};

#endif // GDESKSTYLER_H
