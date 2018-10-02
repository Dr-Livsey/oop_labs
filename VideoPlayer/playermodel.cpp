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
    return fileInfo.exists() && !fileInfo.suffix().compare(QLatin1String("wpl"), Qt::CaseInsensitive);
}

void PlayerModel::AddVideoToPlaylist()
{
    QFileDialog Explorer(this);
    Explorer.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    Explorer.setViewMode(QFileDialog::ViewMode::Detail);
    Explorer.setNameFilter("Video files (*.wmv)");
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

            emit VideoAddedToPlaylist(videos);
    }
}

void PlayerModel::SavePlaylist(QMediaPlaylist *playlist)
{
    QFileDialog Explorer(this);
    Explorer.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
    Explorer.setViewMode(QFileDialog::ViewMode::Detail);
    Explorer.setNameFilter("Playlist's (*.wpl)");
    Explorer.setDirectory(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first());

    if (Explorer.exec())
    {
        QUrl SaveFileUrl = Explorer.selectedUrls().first();
        playlist->setObjectName(SaveFileUrl.toString());
        playlist->save(SaveFileUrl);
    }
}

void PlayerModel::AddPlaylist(QMediaPlaylist *playlist)
{
    QFileDialog Explorer(this);
    Explorer.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    Explorer.setViewMode(QFileDialog::ViewMode::Detail);
    Explorer.setNameFilter("Playlist's (*.wpl)");
    Explorer.setDirectory(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first());

    if (Explorer.exec())
    {
        QUrl SaveFileUrl = Explorer.selectedUrls().first();
        playlist->setObjectName(SaveFileUrl.toString());
    }
}

