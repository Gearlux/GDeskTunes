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
    ui(new Ui::MiniPlayer),
    large(false)
{
    ui->setupUi(this);

    Qt::WindowFlags flags;
    flags = Qt::Window;
    flags |= Qt::FramelessWindowHint;

    setWindowFlags(flags);
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

void MiniPlayer::albumArt(QPixmap pixmap)
{
    int w = ui->album_art->width();
    int h = ui->album_art->height();

    album_picture = pixmap;
    ui->album_art->setPixmap(pixmap.scaled(w,h,Qt::KeepAspectRatio));

    if (large)
    {
        enableBackground();
    }
}

void MiniPlayer::changeBackground()
{
    large = !large;

    if (large)
        enableBackground();
    else
        disableBackground();
}

void MiniPlayer::enableBackground()
{
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(album_picture));
    setPalette(palette);
}

void MiniPlayer::disableBackground()
{
    QPalette palette;
    setPalette(palette);
}

void MiniPlayer::rating(int rate)
{
    qDebug() << "MiniPlayer::rating(" << rate << ")";
    switch(rate)
    {
    case 1:
    case 2:
        ui->thumbs_down->setIcon(QIcon(":/icons/32x32/thumbs_down_on.png"));
        ui->thumbs_up->setIcon(QIcon(":/icons/32x32/thumbs_up_off.png"));
    break;
    case 4:
    case 5:
        ui->thumbs_down->setIcon(QIcon(":/icons/32x32/thumbs_down_off.png"));
        ui->thumbs_up->setIcon(QIcon(":/icons/32x32/thumbs_up.png"));
        break;
    default:
        ui->thumbs_down->setIcon(QIcon(":/icons/32x32/thumbs_down_off.png"));
        ui->thumbs_up->setIcon(QIcon(":/icons/32x32/thumbs_up_off.png"));
    }
}

void MiniPlayer::setRepeat(QString mode)
{
    if (mode == "LIST_REPEAT")
    {
        ui->repeat->setIcon(QIcon(":/icons/32x32/repeat_on.png"));
    }
    if (mode == "SINGLE_REPEAT")
    {
        ui->repeat->setIcon(QIcon(":/icons/32x32/repeat_one.png"));
    }
    if (mode == "NO_REPEAT")
    {
        ui->repeat->setIcon(QIcon(":/icons/32x32/repeat_off.png"));
    }
}

void MiniPlayer::setShuffle(QString mode)
{
    if (mode == "ALL_SHUFFLE")
    {
        ui->shuffle->setIcon(QIcon(":/icons/32x32/shuffle_on.png"));
    }
    if (mode == "NO_SHUFFLE")
    {
        ui->shuffle->setIcon(QIcon(":/icons/32x32/shuffle_off.png"));
    }
}

void MiniPlayer::mousePressEvent(QMouseEvent *event)
{
    mouse_click_x_coordinate = event->x();
    mouse_click_y_coordinate = event->y();
    do_move = true;
    event->accept();
}

void MiniPlayer::mouseMoveEvent(QMouseEvent *event)
{
    if (do_move)
    {
        this->move(event->globalX() - mouse_click_x_coordinate, event->globalY() - mouse_click_y_coordinate);
        event->accept();
    }
    else
    {
        QDialog::mouseMoveEvent(event);
    }
}

void MiniPlayer::mouseReleaseEvent(QMouseEvent *event)
{
    do_move = false;
    QDialog::mouseReleaseEvent(event);
}


