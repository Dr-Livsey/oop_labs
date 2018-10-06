#include "playercontroller.h"
#include "playerview.h"
#include "playermodel.h"

#include <QStyle>
#include <QPushButton>
#include <QListWidget>
#include <QMediaPlaylist>
#include <QAudio>

#include <QDebug>

PlayerController::PlayerController(PlayerView *View)
{
    SetupController(this, View);

    VolumeSlider->setRange(0, 100);
    VolumeSlider->setValue(50);

    PlayButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    StopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    PauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));

    NextButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    PrevButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    MuteButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));

    PlayerModel *Model = new PlayerModel(this);

    /*Playlist Buttons*/
    //Add new video to playlist
    connect(AddVideoButton, SIGNAL(clicked()), Model, SLOT(AddVideoToPlaylist()));

    //Delete video from playlist
    connect(DeleteButton, SIGNAL(clicked()), this, SLOT(DeleteButtonPressed()));

    //Reset delete all videos from playlist.
    connect(ResetButton, SIGNAL(clicked()), this, SLOT(ResetListWidget()));

    //Play selected Video
    connect(PlaylistWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
             View, SLOT(PlaySelectedVideo(QListWidgetItem *)));

    //Volume Changed
    connect(VolumeSlider, SIGNAL(valueChanged(int)), this, SLOT(VolumeValueChanged(int)));

    //Playback changed
    connect(this, SIGNAL(updDuration(int)), View, SLOT(UpdateDurationInfo(int)));
    connect(this, SIGNAL(updPosition(int)), View, SLOT(UpdatePositionInfo(int)));

    //SavePlaylist
    connect(View, SIGNAL(SavePlaylist(QMediaPlaylist *)),
             Model, SLOT(SavePlaylist(QMediaPlaylist *)));

    //AddPlaylist
    connect(View, SIGNAL(AddPlaylist(QMediaPlaylist *, bool &)),
             Model, SLOT(AddPlaylist(QMediaPlaylist *, bool &)));

    //Open video
    connect(View, SIGNAL(OpenVideo(QUrl&)), Model, SLOT(OpenVideo(QUrl&)));
}

void PlayerController::ChangeDuration(qint64 new_duration)
{
    int new_dur = new_duration / 1000;
    DurationSlider->setRange(0, new_dur);

    emit updDuration(new_dur);
}

void PlayerController::ChangePlaybackPos(qint64 new_position)
{
   int new_pos = new_position / 1000;
   DurationSlider->setValue(new_pos);

   emit updPosition(new_pos);
}


void PlayerController::PlaylistUpdated(const QList<QUrl> &NewVideos)
{
    foreach (QUrl curUrl, NewVideos)
    {
       QListWidgetItem *new_item = new QListWidgetItem;

       new_item->setText(curUrl.fileName());
       new_item->setData(Qt::UserRole, QVariant(curUrl));

       PlaylistWidget->addItem(new_item);
    }
    emit plUpdated(NewVideos);
}

void PlayerController::DeleteButtonPressed()
{
    QListWidgetItem *item = PlaylistWidget->currentItem();
    int PlaylistIindex = item->listWidget()->row(item);

    delete item;

    emit deleteFromPlaylist(PlaylistIindex);
}
void PlayerController::ResetListWidget()
{
    PlaylistWidget->clear();
    emit ResetPlaylist();
}

void PlayerController::VolumeValueChanged(int VolumeValue)
{
    qreal linearVolume =  QAudio::convertVolume(VolumeValue / qreal(100),
                                                QAudio::LinearVolumeScale,
                                                QAudio::LinearVolumeScale);

    emit SetNewVolumeValue(qRound(linearVolume * 100));
}


