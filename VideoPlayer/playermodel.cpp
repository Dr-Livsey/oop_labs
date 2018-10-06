#include "playermodel.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QMediaPlaylist>
#include <QListWidget>
#include <QDebug>
#include <QFileInfo>

PlayerModel::PlayerModel(QWidget *parent) : QWidget(parent)
{
    connect(this, SIGNAL(VideoAddedToPlaylist(const QList<QUrl> &)),
            parent, SLOT(PlaylistUpdated(const QList<QUrl> &)));

}

bool PlayerModel::isPlaylist(const QUrl &url)
{
    if (!url.isLocalFile())
        return false;
    const QFileInfo fileInfo(url.toLocalFile());
    return fileInfo.exists() && !fileInfo.suffix().compare(QLatin1String("m3u"), Qt::CaseInsensitive);
}

void PlayerModel::AddVideoToPlaylist()
{
    QFileDialog Explorer(this);
    Explorer.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    Explorer.setViewMode(QFileDialog::ViewMode::Detail);
    Explorer.setNameFilter("Video files (*.mp4 *.wmv)");
    Explorer.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).first());

    QList<QUrl> videos;
    if (Explorer.exec())
    {
            videos = Explorer.selectedUrls();
            foreach (const QUrl &curUrl, videos)
            {
                if (isPlaylist(curUrl) || curUrl.isLocalFile() == false)
                    videos.removeAll(curUrl);
            }

           if(videos.isEmpty() == false) emit VideoAddedToPlaylist(videos);
    }
}

void PlayerModel::OpenVideo(QUrl &VideoUrl)
{
    QFileDialog Explorer(this);
    Explorer.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    Explorer.setViewMode(QFileDialog::ViewMode::Detail);
    Explorer.setNameFilter("Video files (*.mp4 *.wmv)");
    Explorer.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).first());

    if (Explorer.exec())
            VideoUrl = Explorer.selectedUrls().first();

}

void PlayerModel::SavePlaylist(QMediaPlaylist *playlist)
{
    QFileDialog Explorer(this);
    Explorer.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
    Explorer.setViewMode(QFileDialog::ViewMode::Detail);
    Explorer.setNameFilter("Playlist's (*.m3u)");
    Explorer.setDirectory(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first());

    if (Explorer.exec())
    {
        QUrl SaveFileUrl = Explorer.selectedUrls().first();
        playlist->save(SaveFileUrl, "m3u");
    }
}

void PlayerModel::AddPlaylist(QMediaPlaylist *playlist, bool &added)
{
    QFileDialog Explorer(this);
    Explorer.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    Explorer.setViewMode(QFileDialog::ViewMode::Detail);
    Explorer.setNameFilter("Playlist's (*.m3u)");
    Explorer.setDirectory(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first());

    if (Explorer.exec())
    {
        QUrl SaveFileUrl = Explorer.selectedUrls().first();
        playlist->setObjectName(SaveFileUrl.fileName());
        playlist->load(SaveFileUrl);
        added = true;
    }
    else
    {
        added = false;
    }
}

