#include "myvideowidget.h"
#include <QKeyEvent>
#include <QMediaPlayer>

MyVideoWidget::MyVideoWidget(QMediaPlayer *Player, QWidget *parent)
    : QVideoWidget (parent),
      MediaPlayer(Player)

{}

void MyVideoWidget::mouseDoubleClickEvent(QMouseEvent *me)
{
    if (MediaPlayer->isVideoAvailable())
    {
        setFullScreen(!isFullScreen());
        me->accept();
    }
    else me->ignore();

}

void MyVideoWidget::keyPressEvent(QKeyEvent *key_event)
{
 if (key_event->key() == Qt::Key_Escape && isFullScreen() == true)
 {
     setFullScreen(false);
     key_event->accept();
 }
 else key_event->ignore();
}
