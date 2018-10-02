#include "playerview.h"
#include "ui_playerview.h"

#include "playercontroller.h"

#include <QListWidgetItem>
#include <QMediaPlaylist>
#include <QTime>

#include <QDebug>

PlayerView::PlayerView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlayerView)
{
    ui->setupUi(this);

    /*Create Video Player*/
    Player = new QMediaPlayer(this);
    VideoWidget = new QVideoWidget(this);
    Playlist = new QMediaPlaylist(this);

    QBoxLayout *DisplayLayout = new QVBoxLayout;
    DisplayLayout->addWidget(VideoWidget);
    ui->Frame->setLayout(DisplayLayout);

    Player->setVideoOutput(VideoWidget);
    Player->setPlaylist(Playlist);

    PlayerController *Controller = new PlayerController(this);

    /*Buttons*/
    connect(ui->Play, SIGNAL(clicked()), Player, SLOT(play()));
    connect(ui->Stop, SIGNAL(clicked()), Player, SLOT(stop()));
    connect(ui->Pause, SIGNAL(clicked()), Player, SLOT(pause()));

    connect(ui->NextButton, SIGNAL(clicked()), Playlist, SLOT(next()));
    connect(ui->PrevButton, &QPushButton::clicked, [this]()
    {
        if (Playlist->currentIndex() == 0)
        {
            Playlist->setCurrentIndex(Playlist->mediaCount() - 1);
        }
        else Playlist->previous();
    });

    /*Playlist*/
    connect(ui->OpenVideo, &QPushButton::clicked, [this]()
    {
        emit ui->AddVideo->clicked();
        if (!Playlist->isEmpty())
        {
            Playlist->setCurrentIndex(Playlist->mediaCount() - 1);
            Player->play();
        }
    });

    /*Add new video(s) to playlist*/
    connect(Controller, SIGNAL(plUpdated(const QList<QUrl> &)),
            this, SLOT(UpdateListWidget(const QList<QUrl> &)));
    /*Delete from playlist*/
     connect(Controller, SIGNAL(deleteFromPlaylist(int)),
             this, SLOT(DeleteVideoFromPlaylist(int)));
    /*Reset playlist.*/
     connect(Controller, SIGNAL(ResetPlaylist()),
             this, SLOT(ResetPlaylist()));
    /*Volume Changed*/
     connect(Controller, SIGNAL(SetNewVolumeValue(int)),
             this, SLOT(SetNewVolume(int)));

     /*Video*/
     connect(Player, SIGNAL(durationChanged(qint64)),
             Controller, SLOT(ChangeDuration(qint64)));
     connect(Player, SIGNAL(positionChanged(qint64)),
             Controller, SLOT(ChangePlaybackPos(qint64)));

     connect(Playlist, SIGNAL(currentIndexChanged(int)),
             this, SLOT(currentVideoChanged(int)));

     connect(ui->DurationSlider, &QSlider::sliderMoved, [this](int pos)
     {
         Player->setPosition(pos * 1000);
     });

     connect(ui->Mute, &QPushButton::clicked, [this](bool checked)
     {
         Player->setMuted(checked);
         if (checked)
             ui->Mute->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
         else
             ui->Mute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
     });

     /*Save playlist*/
     connect(ui->SavePlaylistButton, &QPushButton::clicked, [this]
     {
        emit SavePlaylist(Playlist);
        UpdateplListWidget(Playlist);
     });

     //Muted Button
     connect(Playlist, &QMediaPlaylist::mediaRemoved, [this](int, int)
     {
        if (Playlist->isEmpty()) ui->NowLine->setText("");
     });

     //Add Playlist
     connect(ui->AddPlaylistButton, &QPushButton::clicked, [this]
     {
        QMediaPlaylist *new_playlist = new QMediaPlaylist;
        int PlaylistAmount = PlList.size();

        emit AddPlaylist(new_playlist);
        UpdateplListWidget(new_playlist);

        if (PlaylistAmount == PlList.size())
            delete new_playlist;
     });
}

PlayerView::~PlayerView()
{
    delete Player;
    delete ui;
    delete this;
}

void PlayerView::currentVideoChanged(int index)
{
    if (Playlist->isEmpty()) return;

    Player->setVolume(ui->Volume->value());

    if (Playlist->currentIndex() < 0)
    {
        Playlist->setCurrentIndex(0);
        ui->PlaylistWidget->setCurrentRow(0);
        Player->play();
    }
    else
    {
        ui->PlaylistWidget->setCurrentRow(index);
    }

    QString CurMedia = ui->PlaylistWidget->currentItem()->text();
    ui->NowLine->setText(CurMedia);
}


void PlayerView::UpdateDurationInfo(int duration)
{
    ui->DurationLabel->setText(intToTime(duration));
}

void PlayerView::UpdatePositionInfo(int position)
{
     ui->PositionLabel->setText(intToTime(position));
}

void PlayerView::UpdateListWidget(const QList<QUrl> &NewVideos)
{
    int PlaylistLastSize = Playlist->mediaCount();
    bool PlisEmpty = Playlist->isEmpty();
    bool SomethingAdded = false;

    foreach( QUrl curUrl, NewVideos)
    {
        SomethingAdded = true;
        Playlist->addMedia(curUrl);
    }

    if (PlisEmpty)
    {
        ui->Delete->setEnabled(true);
        ui->Reset->setEnabled(true);
    }

    if (!PlaylistLastSize && SomethingAdded)
    {
        Playlist->setCurrentIndex(0);
        Player->play();
    }
}

void PlayerView::UpdateplListWidget(QMediaPlaylist *new_playlist)
{
    QString NewPlUrl = new_playlist->objectName();

    /*Search this playlist Url in PlListWidget*/
    foreach (QMediaPlaylist *pl, PlList)
    {
       if (pl->objectName() == NewPlUrl) return;
    }

    if (PlList.isEmpty()) ui->GotocurrentplButton->setEnabled(true);

    PlList.append(new_playlist);
    ui->PlaylistContainer->addItem(QUrl(NewPlUrl).fileName());
}

void PlayerView::PlaySelectedVideo(QListWidgetItem *video)
{
    int idx = video->listWidget()->row(video);
    Playlist->setCurrentIndex(idx);
    Player->play();
}

void PlayerView::DeleteVideoFromPlaylist(int PlaylistIndex)
{
    Playlist->removeMedia(PlaylistIndex);

    if (Playlist->isEmpty())
    {
        ui->Delete->setEnabled(false);
        ui->Reset->setEnabled(false);
     }
}

void PlayerView::ResetPlaylist()
{
    Playlist->clear();
    ui->Delete->setEnabled(false);
    ui->Reset->setEnabled(false);
}

void PlayerView::SetNewVolume(int NewVolume)
{
    QString VLabelText = QString::number(NewVolume) + QString("%");

    ui->VolumeValue->setText(QString(VLabelText));
    Player->setVolume(NewVolume);
}

QString PlayerView::intToTime(int itime)
{
   QTime currentTime((itime / 3600), (itime % 3600) / 60, (itime % 3600) % 60);
   QString format = "hh:mm:ss";

   return currentTime.toString(format);
}

void SetupController(PlayerController *Controller, PlayerView *View)
{
    Controller->AddVideoButton = View->ui->AddVideo;
    Controller->DeleteButton = View->ui->Delete;
    Controller->ResetButton = View->ui->Reset;
    Controller->SaveButton = View->ui->SavePlaylistButton;

    Controller->PrevButton = View->ui->PrevButton;
    Controller->NextButton = View->ui->NextButton;

    Controller->PlayButton = View->ui->Play;
    Controller->PauseButton = View->ui->Pause;
    Controller->StopButton = View->ui->Stop;

    Controller->VolumeSlider = View->ui->Volume;
    Controller->MuteButton = View->ui->Mute;
    Controller->DurationSlider = View->ui->DurationSlider;

    Controller->PlaylistWidget = View->ui->PlaylistWidget;
}
