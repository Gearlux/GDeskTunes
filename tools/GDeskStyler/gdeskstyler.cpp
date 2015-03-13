#include <QDebug>
#include <QSettings>
#include <QFileDialog>

#include "gdeskstyler.h"
#include "ui_gdeskstyler.h"

#include "QtProperty"
#include "QtStringPropertyManager"
#include "QtColorPropertyManager"
#include "QtGroupPropertyManager"

#include "QtLineEditFactory"
#include "QtColorEditorFactory"
#include "QtSpinBoxFactory"

GDeskStyler::GDeskStyler(QWidget *parent) :
    QMainWindow(parent),
    gdesktunes(0),
    ui(new Ui::GDeskStyler),
    css(QString::null),
    filename(QString::null),
    modified(false),
    block(false)
{
    setAttribute(Qt::WA_DeleteOnClose);

    ui->setupUi(this);

    stringManager = new QtStringPropertyManager(this);
    colorManager = new QtColorPropertyManager(this);
    intManager = new QtIntPropertyManager(this);
    boolManager = new QtBoolPropertyManager(this);
    groupManager = new QtGroupPropertyManager(this);

    QObject::connect(stringManager, SIGNAL(valueChanged(QtProperty*,QString)), this, SLOT(valueChanged(QtProperty *,QString)));
    QObject::connect(colorManager, SIGNAL(valueChanged(QtProperty*,QColor)), this, SLOT(valueChanged(QtProperty *,QColor)));
    QObject::connect(intManager, SIGNAL(valueChanged(QtProperty*,int)), this, SLOT(valueChanged(QtProperty *,int)));
    QObject::connect(boolManager, SIGNAL(valueChanged(QtProperty*,bool)), this, SLOT(valueChanged(QtProperty *,bool)));

    QtLineEditFactory *lineEditFactory = new QtLineEditFactory(this);
    QtColorEditorFactory *colorEditorFactory = new QtColorEditorFactory(this);
    QtSpinBoxFactory *spinBoxFactory = new QtSpinBoxFactory(this);
    QtCheckBoxFactory *checkBoxFactory = new QtCheckBoxFactory(this);

    ui->property_browser->setFactoryForManager(stringManager, lineEditFactory);
    ui->property_browser->setFactoryForManager(colorManager, colorEditorFactory);
    ui->property_browser->setFactoryForManager(intManager, spinBoxFactory);
    ui->property_browser->setFactoryForManager(boolManager, checkBoxFactory);
}

GDeskStyler::~GDeskStyler()
{
    qDebug() << "Deleting the styler";
    ask();

    if (this->gdesktunes && this->css != QString::null)
    {
        QObject *settings = this->gdesktunes->findChild<QObject*>("Settings");
        if (settings != 0)
        {
            qDebug() << "Update styles";
            QMetaObject::invokeMethod(settings, "updateStyles");
        }

        QMetaObject::invokeMethod(this->gdesktunes, "setCSS", Q_ARG(QString, ""));
        QMetaObject::invokeMethod(this->gdesktunes, "setCSS", Q_ARG(QString, this->css));
    }

    delete ui;
}

void GDeskStyler::populate()
{
    qDebug() << "GDeskStyler::populate()";

    block = true;

    qDeleteAll(elements);
    style.clear();
    elements.clear();
    holders.clear();

    QFile qfile(this->filename);
    qfile.open(QFile::ReadOnly | QFile::Text);

    QTextStream os(&qfile);

    int line_nr = 0;
    while(!os.atEnd())
    {
        QString line = os.readLine();
        style.append(line);
        if (line.startsWith("$"))
        {
            int c = line.indexOf(':', 1);
            if (c != -1)
            {
                QString name = line.mid(1, c-1);
                qDebug() << name;
                Element* elt = new Element(name, line_nr);
                elements.append(elt);
                QtProperty *property = stringManager->addProperty(name);

                int comma = line.indexOf(';', c);

                QString value = line.mid(c+1, comma-c-1).trimmed();
                stringManager->setValue(property, value);
                elt->ui = property;
                elt->value = value;
                holders[property] = elt;

                QtBrowserItem *it = ui->property_browser->addProperty(property);
                ui->property_browser->setExpanded(it, false);
            }
        }
        line_nr += 1;
    }


    qfile.close();

    block = false;

}

template <typename T>
void value_changed(Element *holder, QtProperty *property, T& value)
{
    if (holder == 0)
    {
        qDebug() << "No target object";
        return;
    }
    holder->value = value;
}

template<class T> T& as_lvalue(T&& v){ return v; }

void GDeskStyler::valueChanged(QtProperty *property, QString value)
{
    value_changed(holders[property], property, value);
    test();
}

void GDeskStyler::valueChanged(QtProperty *property, QColor value)
{
    // value_changed(holders[property], property, value);
    test();
}

void GDeskStyler::valueChanged(QtProperty *property, int value)
{
    // value_changed(holders[property], property, value);
    test();
}

void GDeskStyler::valueChanged(QtProperty *property, bool value)
{
    // value_changed(holders[property], property, value);
    test();
}

void GDeskStyler::test()
{
    if (block) return;

    qDebug() << "GDeskStyler::test()";

    modified = true;

    if (this->gdesktunes)
    {
        qDebug() << "Apply test css";
#ifdef Q_OS_MAC
        QDir dir(QCoreApplication::applicationDirPath() + QDir::separator() + "../Resources/userstyles");
#else
        QDir dir(QCoreApplication::applicationDirPath() + QDir::separator() + "userstyles");
#endif
        QString css_file = dir.absolutePath() + QDir::separator() + "__gdesktunes_test.scss";
        save(css_file);
        QMetaObject::invokeMethod(this->gdesktunes, "setCSS", Q_ARG(QString, ""));
        QMetaObject::invokeMethod(this->gdesktunes, "setCSS", Q_ARG(QString, "__gdesktunes_test"));
        QFile::remove(css_file);
        qDebug() << "Test css applied";
    }
}

void GDeskStyler::on_actionExit_triggered()
{
    close();
}

void GDeskStyler::on_actionSave_triggered()
{
    if (filename == QString::null)
    {
        on_actionSave_As_triggered();
    }
    else
    {
        save(filename);
    }
}

void GDeskStyler::on_actionSave_As_triggered()
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    QString dir = settings.value("gss.dir", QCoreApplication::applicationDirPath()).toString();

    QString filename = QFileDialog::getSaveFileName(this, "Save style sheet", dir, "*.gss");

    if (filename != QString::null)
    {
        save(filename);

        this->filename = filename;

        QFileInfo info(filename);
        settings.setValue("gss.dir", info.absoluteDir().absolutePath());
    }

}

void GDeskStyler::save(QString filename)
{

    // style->generate();

    QFile gss_file(filename);
    gss_file.open(QIODevice::WriteOnly);
    QDataStream os(&gss_file);

    for(int e=0; e<elements.size(); ++e)
    {
        Element *elt = elements.at(e);
        style.replace(elt->line, QString("$") + elt->name + ":" + elt->value + ";");
    }

    for(int l=0; l<style.size(); ++l)
    {
        QString line = style.at(l);
        os << line << "\n";
    }
    gss_file.close();
}

void GDeskStyler::load(QString filename)
{
    qDebug() << "GDeskStyler::load(" << filename << ")";
    this->filename = filename;

    ui->property_browser->clear();

    populate();

    modified = false;
}

void GDeskStyler::setGDeskTunes(QObject *gdesktunes)
{
#ifdef Q_OS_MAC
    QDir dir(QCoreApplication::applicationDirPath() + QDir::separator() + "../Resources/userstyles");
#else
    QDir dir(QCoreApplication::applicationDirPath() + QDir::separator() + "userstyles");
#endif

    if (this->gdesktunes)
    {
        qDebug() << "GDeskTunes allready set";
        return;
    }

    this->gdesktunes = gdesktunes;

    // Try to locate the corresponding gss file
    QString css;
    bool success = QMetaObject::invokeMethod(gdesktunes, "getCSS", Q_RETURN_ARG(QString, css));
    if (success)
    {
        qDebug() << "Return value" << css;

        this->css = css;

        QString filename = dir.absolutePath() + QDir::separator() + css + ".scss";

        QFile gss_file(filename);

        if (gss_file.exists())
        {
            load(filename);
        }
    }
}

void GDeskStyler::ask()
{
    qDebug() << "Save ?";
}
