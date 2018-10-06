#include "playerview.h"
#include "ui_playerview.h"

#include "playercontroller.h"
#include "createplaylistpopup.h"
#include "myvideowidget.h"

#include <QMessageBox>
#include <QListWidgetItem>
#include <QMediaPlaylist>
#include <QTime>

#include <QDebug>

PlayerView::PlayerView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlayerView),
    CreatePopUp(new CreatePlaylistPopup(this))
{
    ui->setupUi(this);

    ui->tabWidget->setCurrentIndex(0);
    setWindowTitle("VideoPlayer");

    /*Create Video Player*/
    Player = new QMediaPlayer(this);
    VideoWidget = new MyVideoWidget(Player, this);

    QBoxLayout *DisplayLayout = new QVBoxLayout;
    DisplayLayout->addWidget(VideoWidget);
    ui->Frame->setLayout(DisplayLayout);

    Player->setVideoOutput(VideoWidget);

    PlayerController *Controller = new PlayerController(this);

    /*Buttons*/
    connect(ui->Play, SIGNAL(clicked()), Player, SLOT(play()));
    connect(ui->Stop, SIGNAL(clicked()), Player, SLOT(stop()));
    connect(ui->Pause, SIGNAL(clicked()), Player, SLOT(pause()));

    connect(ui->NextButton, &QPushButton::clicked, [this]
    {
        if (Player->playlist()) Player->playlist()->next();
    });
    connect(ui->PrevButton, &QPushButton::clicked, [this]()
    {
       if (!Player->playlist()) return;

        QMediaPlaylist *CurrentPlayingPlaylist = Player->playlist();
       if (CurrentPlayingPlaylist->currentIndex() == 0)
       {
            CurrentPlayingPlaylist->setCurrentIndex
            (
              CurrentPlayingPlaylist->mediaCount() - 1
            );
       }
        else CurrentPlayingPlaylist->previous();
    });

    /*Open video*/
    connect(ui->OpenVideo, &QPushButton::clicked, [this]()
    {
        QUrl OpenedUrl("");

        emit OpenVideo(OpenedUrl);

        if (OpenedUrl != QUrl(""))
        {
            ui->NowPlaylistLine->setText("");
            ui->GotocurrentplButton->setDisabled(true);

            ui->NowLine->setText(OpenedUrl.fileName());
            Player->setMedia(OpenedUrl);
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

     connect(ui->DurationSlider, &QSlider::sliderMoved, [this](int pos)
     {
         Player->setPosition(pos * 1000);
     });

     //Fullscreen Button
     connect(ui->FullscreenButton, &QPushButton::clicked,
     [this]
     {
         if (Player->isVideoAvailable())
         {
            VideoWidget->setFullScreen(true);
         }
     });

     //Muted Button
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
         QMediaPlaylist *CurrentPlaylist = GetCurrentPlaylist();
        emit SavePlaylist(CurrentPlaylist);
     });

     //Add Playlist
     connect(ui->AddPlaylistButton, &QPushButton::clicked, [this]
     {
        QMediaPlaylist *new_playlist = new QMediaPlaylist;

        bool isadded = false;
        emit AddPlaylist(new_playlist, isadded);

        if (isadded == false)
        {
            delete new_playlist;
        }
        else
            UpdateplListWidget(new_playlist);
     });

     //Create Playlist
     connect(CreatePopUp, SIGNAL(PlaylistSetted()), this, SLOT(CreatePlaylist()));
     connect(ui->CreatePlaylistButton, &QPushButton::clicked,
     [this]
     {
         ui->centralWidget->setDisabled(true);
         CreatePopUp->show();
     });

     //DeletePlaylist
     connect(ui->DeletePlaylistButton, &QPushButton::clicked,
     [this]
     {
         QListWidgetItem *SelectedItem = ui->PlaylistContainer->currentItem();
         QMediaPlaylist *Selected_Playlist = GetPlaylistFromItem(SelectedItem);
         bool iscurrent = Selected_Playlist == Player->playlist();

         ui->PlaylistWidget->clear();
         ui->PlaylistContainer->removeItemWidget(SelectedItem);
         delete SelectedItem;
         delete Selected_Playlist;

         if (iscurrent)
         {
             ui->NowLine->setText("");
             ui->NowPlaylistLine->setText("");
             ui->GotocurrentplButton->setDisabled(true);
         }

         ui->DeletePlaylistButton->setDisabled(true);
         ui->Delete->setDisabled(true);
         ui->AddVideo->setDisabled(true);
         ui->Reset->setDisabled(true);
         ui->SavePlaylistButton->setDisabled(true);
     });

     //Show current playlist
     connect(ui->GotocurrentplButton, &QPushButton::clicked, [this]
     {
        QString PlName = Player->playlist()->objectName();

        QList<QListWidgetItem*> have_found = ui->PlaylistContainer->findItems
        (
           PlName,
           Qt::MatchFlag::MatchExactly
        );

        foreach (QListWidgetItem* i, have_found)
        {
            if (Player->playlist() == GetPlaylistFromItem(i))
            {
                ui->PlaylistContainer->setCurrentItem(i);
                break;
            }
        }
        showPlaylist(Player->playlist());
     });

     //QListWidget PlayContainer clicked.
     connect(ui->PlaylistContainer, &QListWidget::itemClicked,
     [this](QListWidgetItem *item)
     {
         emit ui->PlaylistContainer->itemSelectionChanged();
         showPlaylist(GetPlaylistFromItem(item));
     });

     //QListWidget PlayContainer doubleclicked.
     connect(ui->PlaylistContainer, &QListWidget::itemDoubleClicked,
     [this](QListWidgetItem *item)
     {
         emit ui->PlaylistContainer->itemClicked(item);

         QMediaPlaylist *SelectdPlaylist = GetPlaylistFromItem(item);

          ui->NowPlaylistLine->setText(item->text());
         ui->GotocurrentplButton->setEnabled(true);

         Player->setPlaylist(SelectdPlaylist);
         SelectdPlaylist->setCurrentIndex(0);
         Player->play();
     });

     connect(ui->PlaylistContainer, &QListWidget::itemSelectionChanged,
     [this]()
     {
        QMediaPlaylist *CurrentPlaylist = GetCurrentPlaylist();

        if (CurrentPlaylist && CurrentPlaylist->isEmpty() ||
                !CurrentPlaylist)
            ui->NowLine->setText("");

        ui->AddVideo->setEnabled(true);
        ui->SavePlaylistButton->setEnabled(true);
        ui->DeletePlaylistButton->setEnabled(true);

        if (CurrentPlaylist && !CurrentPlaylist->isEmpty())
        {
            ui->Delete->setEnabled(true);
            ui->Reset->setEnabled(true);
        }
        else
        {
            ui->Delete->setEnabled(false);
            ui->Reset->setEnabled(false);
        }
     });

    connect(ui->menuThemes, &QMenu::triggered,
    [this](QAction *action)
    {
        if (action->text() == "Blue Tint") this->setBlueTintTheme();
        else if (action->text() == "White") this->setWhiteTheme();
    });

}

PlayerView::~PlayerView()
{
    delete CreatePopUp;
    delete Player;
    delete ui;
}

void PlayerView::currentVideoChanged(int index)
{
    QMediaPlaylist *PlayingPlaylist = Player->playlist();
    bool iscurrent = PlayingPlaylist == GetCurrentPlaylist();
    if (PlayingPlaylist->isEmpty()) return;

    Player->setVolume(ui->Volume->value());

    if (PlayingPlaylist->currentIndex() < 0)
    {
        PlayingPlaylist->setCurrentIndex(0);
        if (iscurrent) ui->PlaylistWidget->setCurrentRow(0);
        Player->play();
    }
    else
    {
        if (iscurrent) ui->PlaylistWidget->setCurrentRow(index);
    }

    QString CurMedia = PlayingPlaylist->currentMedia().canonicalUrl().fileName();
    ui->NowLine->setText(CurMedia);
}

void PlayerView::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question
            ( this, "VideoPlayer",
              tr("Are you sure?\n"),
              QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
              QMessageBox::Yes
             );
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {
        event->accept();
    }
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
    QMediaPlaylist *CurrentPlaylist = GetCurrentPlaylist();

    int PlaylistLastSize = CurrentPlaylist->mediaCount();
    bool PlisEmpty = CurrentPlaylist->isEmpty();
    bool SomethingAdded = false;

    foreach( QUrl curUrl, NewVideos)
    {
        SomethingAdded = true;
        CurrentPlaylist->addMedia(curUrl);
    }

    if (SomethingAdded == false) return;

    if (PlisEmpty)
    {
        ui->Delete->setEnabled(true);
        ui->Reset->setEnabled(true);
    }

    if (Player->playlist() == CurrentPlaylist)
    {
        if (!PlaylistLastSize)
        {
            ui->GotocurrentplButton->setEnabled(true);
            ui->NowPlaylistLine->setText(CurrentPlaylist->objectName());
            Player->play();
        }
        else return;
    }
    else ui->PlaylistWidget->setCurrentRow(CurrentPlaylist->mediaCount() - 1);
}

void PlayerView::UpdateplListWidget(QMediaPlaylist *new_playlist)
{
    QString NewPlName(new_playlist->objectName());
    int LastSize = ui->PlaylistContainer->count();

    AddPlaylistToContainer(new_playlist, NewPlName);

    if (!LastSize)
    {
        ui->DeletePlaylistButton->setEnabled(true);
        ui->SavePlaylistButton->setEnabled(true);

        if (Player->isVideoAvailable() == false)
        {
            ui->NowPlaylistLine->setText(NewPlName);
            ui->GotocurrentplButton->setEnabled(true);
            Player->setPlaylist(new_playlist);
            Player->play();
        }
    }

    ui->PlaylistWidget->setCurrentRow(ui->PlaylistContainer->count() - 1);
    showPlaylist(new_playlist);
}

void PlayerView::PlaySelectedVideo(QListWidgetItem *video)
{
    QMediaPlaylist *CurrentPlaylist = GetCurrentPlaylist();

    int idx = video->listWidget()->row(video);

    ui->NowPlaylistLine->setText(ui->PlaylistContainer->currentItem()->text());
    ui->GotocurrentplButton->setEnabled(true);

    Player->setPlaylist(CurrentPlaylist);
    CurrentPlaylist->setCurrentIndex(idx);
    Player->play();
}

void PlayerView::DeleteVideoFromPlaylist(int PlaylistIndex)
{
    QMediaPlaylist *CurrentPlaylist = GetCurrentPlaylist();

    CurrentPlaylist->removeMedia(PlaylistIndex);

    if (CurrentPlaylist->isEmpty())
    {
        ui->Delete->setEnabled(false);
        ui->Reset->setEnabled(false);
    }
    else emit CurrentPlaylist->currentIndexChanged(PlaylistIndex);
}

void PlayerView::ResetPlaylist()
{
    GetCurrentPlaylist()->clear();
    ui->Delete->setEnabled(false);
    ui->Reset->setEnabled(false);
}

void PlayerView::CreatePlaylist()
{
    QString PlaylistName = CreatePopUp->GetPlaylistName();
    int LastSize = ui->PlaylistContainer->count();

    if (PlaylistName != "")
    {
        QMediaPlaylist *NewPlaylist = new QMediaPlaylist(this);
        AddPlaylistToContainer(NewPlaylist, PlaylistName);

        if (!LastSize)
        {
            ui->DeletePlaylistButton->setEnabled(true);
            ui->SavePlaylistButton->setEnabled(true);

            if (!Player->isVideoAvailable())
            {
                ui->NowPlaylistLine->setText(PlaylistName);
                ui->GotocurrentplButton->setEnabled(true);
                Player->setPlaylist(NewPlaylist);
                Player->play();
            }
        }
    }
    ui->centralWidget->setEnabled(true);
    ui->Delete->setEnabled(false);
    ui->Reset->setEnabled(false);
    ui->PlaylistWidget->clear();
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

void PlayerView::showPlaylist(QMediaPlaylist *playlist)
{
    ui->PlaylistWidget->clear();

    if (playlist->mediaCount() == 0) return;

    for (int i = 0; i < playlist->mediaCount(); i++)
    {
        QMediaContent CurrentContent = playlist->media(i);      
        ui->PlaylistWidget->addItem(CurrentContent.canonicalUrl().fileName());
    }

    if (Player->playlist() == playlist)
    {
        int idx = playlist->currentIndex();
        ui->PlaylistWidget->setCurrentRow(idx);
    }
    else ui->PlaylistWidget->setCurrentRow(0);
}

QMediaPlaylist *PlayerView::GetPlaylistFromItem(QListWidgetItem *item)
{
    QVariant data = item->data(Qt::UserRole);
    return data.value<QMediaPlaylist*>();
}

void PlayerView::AddPlaylistToContainer(QMediaPlaylist *playlist, QString PlName)
{
    QListWidgetItem *NewItem = new QListWidgetItem;
    QVariant var;

    //QListWidgetItem contains Playlist
    var.setValue(playlist);
    NewItem->setData(Qt::UserRole, var);
    NewItem->setText(PlName);

    playlist->setObjectName(PlName);

    connect(playlist, SIGNAL(currentIndexChanged(int)), this, SLOT(currentVideoChanged(int)));
    connect(playlist, &QMediaPlaylist::mediaRemoved, [playlist, this](int, int)
    {
       if (playlist->isEmpty()) ui->NowLine->setText("");
    });

    ui->AddVideo->setEnabled(true);
    ui->PlaylistContainer->addItem(NewItem);
    ui->PlaylistContainer->setCurrentItem(NewItem);
}

QMediaPlaylist *PlayerView::GetCurrentPlaylist()
{
    if (!ui->PlaylistContainer->currentItem()) return nullptr;

    QMediaPlaylist *CurrentPlaylist = GetPlaylistFromItem
    (
       ui->PlaylistContainer->currentItem()
    );

    return CurrentPlaylist;
}

void PlayerView::setButtonStyle(QString foreground,
                          QString color,
                          QString disabledForeground,
                          QString disabledColor)
{
    QString ButtonsStyleSheet = ":enabled { color: " + color
                                + "; background-color: " + foreground
                                + " } :disabled { color: " + disabledColor
                                + "; background-color: " + disabledForeground + " }";


    ui->AddVideo->setStyleSheet(ButtonsStyleSheet);
    ui->Delete->setStyleSheet(ButtonsStyleSheet);
    ui->AddPlaylistButton->setStyleSheet(ButtonsStyleSheet);
    ui->CreatePlaylistButton->setStyleSheet(ButtonsStyleSheet);
    ui->DeletePlaylistButton->setStyleSheet(ButtonsStyleSheet);
    ui->FullscreenButton->setStyleSheet(ButtonsStyleSheet);
    ui->GotocurrentplButton->setStyleSheet(ButtonsStyleSheet);
    ui->Mute->setStyleSheet(ButtonsStyleSheet);
    ui->NextButton->setStyleSheet(ButtonsStyleSheet);
    ui->OpenVideo->setStyleSheet(ButtonsStyleSheet);
    ui->Pause->setStyleSheet(ButtonsStyleSheet);
    ui->Play->setStyleSheet(ButtonsStyleSheet);
    ui->PrevButton->setStyleSheet(ButtonsStyleSheet);
    ui->Reset->setStyleSheet(ButtonsStyleSheet);
    ui->SavePlaylistButton->setStyleSheet(ButtonsStyleSheet);
    ui->Stop->setStyleSheet(ButtonsStyleSheet);

}

void PlayerView::setBlueTintTheme()
{
    setButtonStyle
    (
       "rgb(87, 185, 200)",
       "rgb(255, 255, 255)",
       "rgb(0, 69, 104)",
       "rgb(202, 202, 202)"
    );

    ui->tabWidget->setStyleSheet
    (
             "QWidget {background-color: rgb(0, 0, 85); color: white;}"
             "QTabBar::tab {\
                border: 2px solid #C4C4C3;\
                border-bottom-color: #C2C7CB;\
                border-top-left-radius: 4px;\
                border-top-right-radius: 4px;\
                min-width: 8ex;\
                padding: 2px;\
            }"
            "QTabBar::tab:selected {\
                border-color: #9B9B9B;\
                border-bottom-color: #C2C7CB;\
            }"
            "QTabBar::tab:!selected {\
                border-color: #9B9B9B;\
                border-bottom-color: #C2C7CB;\
                 color : rgb(216, 216, 216);\
             }"

    );
                ui->centralWidget->setStyleSheet("background-color: rgb(0, 0, 127)");
}

void PlayerView::setWhiteTheme()
{
    setButtonStyle
    (
       "",
       "",
       "",
       ""
    );
    ui->tabWidget->setStyleSheet
    (
                "QTabBar::tab {\
                   min-width: 8ex;\
               }"
    );
    ui->centralWidget->setStyleSheet("");
}



void SetupController(PlayerController *Controller, PlayerView *View)
{
    Controller->AddVideoButton = View->ui->AddVideo;
    Controller->DeleteButton = View->ui->Delete;
    Controller->ResetButton = View->ui->Reset;
    Controller->SaveButton = View->ui->SavePlaylistButton;

    Controller->PrevButton = View->ui->PrevButton;
    Controller->NextButton = View->ui->NextButton;

    Controller->OpenButton = View->ui->OpenVideo;
    Controller->PlayButton = View->ui->Play;
    Controller->PauseButton = View->ui->Pause;
    Controller->StopButton = View->ui->Stop;

    Controller->VolumeSlider = View->ui->Volume;
    Controller->MuteButton = View->ui->Mute;
    Controller->DurationSlider = View->ui->DurationSlider;

    Controller->PlaylistWidget = View->ui->PlaylistWidget;
}
