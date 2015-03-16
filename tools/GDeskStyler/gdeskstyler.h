#ifndef GDESKSTYLER_H
#define GDESKSTYLER_H

#include <QMainWindow>
#include <QMetaProperty>
#include <QDebug>

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

class Element
{
public:
    Element(QString name, int line) : name(name), line(line), ui(0), parent(0), comment(QString::null) {}
    virtual ~Element();

public:
    QString name;
    QVariant value;
    int line;
    QtProperty *ui;
    Element *parent;
    QString comment;
};

class GDeskStyler : public QMainWindow
{
    Q_OBJECT

public:
    explicit GDeskStyler(QWidget *parent = 0);
    ~GDeskStyler();

    void setGDeskTunes(QObject *gdesktunes);
    void closeEvent(QCloseEvent *evt);

private slots:
    void valueChanged(QtProperty *property, QString value);
    void valueChanged(QtProperty *property, QColor value);
    void valueChanged(QtProperty *property, int value);
    void valueChanged(QtProperty *property, bool value);

    void on_actionSave_triggered();
    void on_actionSave_As_triggered();
    void on_actionExit_triggered();

    void on_actionReload_triggered();

public:
    void show();

private:
    void save(QString filename);
    void load(QString filename);
    void test();
    void ask();
    void populate();

private:
    QObject *gdesktunes;
    Ui::GDeskStyler *ui;

    QString css;
    QString filename;
    bool modified;
    bool block;

    QStringList style;
    QList<Element*> elements;
    QMap<QString, Element*> groups;
    QMap<QtProperty*, Element*> holders;

    QtStringPropertyManager *stringManager;
    QtColorPropertyManager* colorManager;
    QtIntPropertyManager* intManager;
    QtGroupPropertyManager* groupManager;
    QtBoolPropertyManager* boolManager;

};

#endif // GDESKSTYLER_H
