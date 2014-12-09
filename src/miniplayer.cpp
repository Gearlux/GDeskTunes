#include "miniplayer.h"
#include "ui_miniplayer.h"

#include "mainwindow.h"

#include <QKeyEvent>
#include <QDebug>

QString seconds_to_DHMS(quint32 duration)
{
QString res;
int seconds = (int) (duration % 60);
duration /= 60;
int minutes = (int) (duration % 60);
duration /= 60;
int hours = (int) (duration % 24);
int days = (int) (duration / 24);
if((hours == 0)&&(days == 0))
    return res.sprintf("%2d:%02d", minutes, seconds);
if (days == 0)
    return res.sprintf("%2d:%02d:%02d", hours, minutes, seconds);
return res.sprintf("%dd%02d:%02d:%02d", days, hours, minutes, seconds);
}

MiniPlayer::MiniPlayer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MiniPlayer)
{
    ui->setupUi(this);
}

MiniPlayer::~MiniPlayer()
{
    delete ui;
}

void MiniPlayer::onTriggerMiniPlayer(QPoint& pt)
{
    if (isHidden())
    {
        MainWindow *w = qobject_cast<MainWindow*>(parent());
        if (w != 0)
        {
            w->activateWindow();
            w->raise();
        }
        show();
    }
    else
    {
        hide();
    }
}

void MiniPlayer::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_MediaPlay:
    case Qt::Key_MediaNext:
    case Qt::Key_MediaPrevious:
    {
        MainWindow *w = qobject_cast<MainWindow*>(parent());
        if (w != 0)
        {
            qDebug() << "MiniPlayer captures media key";
            w->keyPressEvent(event);
            break;
        }
    }
    default:
        QDialog::keyPressEvent(event);
        break;
    }
}

void MiniPlayer::nowPlaying(QString title, QString artist, QString album, int duration)
{
    qDebug() << "MiniPlayer::nowPlaying(" << title << "," << artist << "," << album << "," << duration << ")";
    ui->title->setText(title);
    ui->artist->setText(artist);
    ui->album->setText(album);
    ui->duration->setText(seconds_to_DHMS(duration) );
    ui->slider->setMinimum(0);
    ui->slider->setMaximum(duration);
}

void MiniPlayer::playbackTime(int current, int total)
{
    ui->current->setText( seconds_to_DHMS(current / 1000));
    ui->slider->setValue(current / 1000);
}


