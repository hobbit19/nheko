/*
 * nheko Copyright (C) 2017  Konstantinos Sideris <siderisk@auth.gr>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QUrl>
#include <mtx.hpp>

/*
 * MatrixClient provides the high level API to communicate with
 * a Matrix homeserver. All the responses are returned through signals.
 */
class MatrixClient : public QNetworkAccessManager
{
        Q_OBJECT
public:
        MatrixClient(QString server, QObject *parent = 0);

        // Client API.
        void initialSync() noexcept;
        void sync() noexcept;
        void sendRoomMessage(mtx::events::MessageType ty,
                             int txnId,
                             const QString &roomid,
                             const QString &msg,
                             const QFileInfo &info,
                             const QString &url = "") noexcept;
        void login(const QString &username, const QString &password) noexcept;
        void registerUser(const QString &username,
                          const QString &password,
                          const QString &server) noexcept;
        void versions() noexcept;
        void fetchRoomAvatar(const QString &roomid, const QUrl &avatar_url);
        //! Download user's avatar.
        void fetchUserAvatar(const QUrl &avatarUrl,
                             std::function<void(QImage)> onSuccess,
                             std::function<void(QString)> onError);
        void fetchCommunityAvatar(const QString &communityId, const QUrl &avatarUrl);
        void fetchCommunityProfile(const QString &communityId);
        void fetchCommunityRooms(const QString &communityId);
        void downloadImage(const QString &event_id, const QUrl &url);
        void downloadFile(const QString &event_id, const QUrl &url);
        void messages(const QString &room_id, const QString &from_token, int limit = 30) noexcept;
        void uploadImage(const QString &roomid,
                         const QSharedPointer<QIODevice> data,
                         const QString &filename);
        void uploadFile(const QString &roomid,
                        const QSharedPointer<QIODevice> data,
                        const QString &filename);
        void uploadAudio(const QString &roomid,
                         const QSharedPointer<QIODevice> data,
                         const QString &filename);
        void uploadFilter(const QString &filter) noexcept;
        void joinRoom(const QString &roomIdOrAlias);
        void leaveRoom(const QString &roomId);
        void sendTypingNotification(const QString &roomid, int timeoutInMillis = 20000);
        void removeTypingNotification(const QString &roomid);
        void readEvent(const QString &room_id, const QString &event_id);
        void inviteUser(const QString &room_id, const QString &user);
        void createRoom(const mtx::requests::CreateRoom &request);

        QUrl getHomeServer() { return server_; };
        int transactionId() { return txn_id_; };
        int incrementTransactionId() { return ++txn_id_; };

        void reset() noexcept;

public slots:
        void getOwnProfile() noexcept;
        void getOwnCommunities() noexcept;
        void logout() noexcept;

        void setServer(const QString &server)
        {
                server_ = QUrl(QString("https://%1").arg(server));
        };
        void setAccessToken(const QString &token) { token_ = token; };
        void setNextBatchToken(const QString &next_batch) { next_batch_ = next_batch; };

signals:
        void loginError(const QString &error);
        void registerError(const QString &error);
        void versionError(const QString &error);

        void loggedOut();
        void invitedUser(const QString &room_id, const QString &user);
        void roomCreated(const QString &room_id);

        void loginSuccess(const QString &userid, const QString &homeserver, const QString &token);
        void registerSuccess(const QString &userid,
                             const QString &homeserver,
                             const QString &token);
        void versionSuccess();
        void imageUploaded(const QString &roomid,
                           const QSharedPointer<QIODevice> data,
                           const QString &filename,
                           const QString &url);
        void fileUploaded(const QString &roomid, const QString &filename, const QString &url);
        void audioUploaded(const QString &roomid, const QString &filename, const QString &url);

        void roomAvatarRetrieved(const QString &roomid,
                                 const QPixmap &img,
                                 const QString &url,
                                 const QByteArray &data);
        void userAvatarRetrieved(const QString &userId, const QImage &img);
        void communityAvatarRetrieved(const QString &communityId, const QPixmap &img);
        void communityProfileRetrieved(const QString &communityId, const QJsonObject &profile);
        void communityRoomsRetrieved(const QString &communityId, const QJsonObject &rooms);
        void imageDownloaded(const QString &event_id, const QPixmap &img);
        void fileDownloaded(const QString &event_id, const QByteArray &data);

        // Returned profile data for the user's account.
        void getOwnProfileResponse(const QUrl &avatar_url, const QString &display_name);
        void getOwnCommunitiesResponse(const QList<QString> &own_communities);
        void initialSyncCompleted(const mtx::responses::Sync &response);
        void initialSyncFailed(const QString &msg);
        void syncCompleted(const mtx::responses::Sync &response);
        void syncFailed(const QString &msg);
        void joinFailed(const QString &msg);
        void messageSent(const QString &event_id, const QString &roomid, const int txn_id);
        void messageSendFailed(const QString &roomid, const int txn_id);
        void emoteSent(const QString &event_id, const QString &roomid, const int txn_id);
        void messagesRetrieved(const QString &room_id, const mtx::responses::Messages &msgs);
        void joinedRoom(const QString &room_id);
        void leftRoom(const QString &room_id);
        void roomCreationFailed(const QString &msg);

private:
        QNetworkReply *makeUploadRequest(QSharedPointer<QIODevice> iodev);

        // Client API prefix.
        QString clientApiUrl_;

        // Media API prefix.
        QString mediaApiUrl_;

        // The Matrix server used for communication.
        QUrl server_;

        // The access token used for authentication.
        QString token_;

        // Increasing transaction ID.
        int txn_id_;

        // Token to be used for the next sync.
        QString next_batch_;

        // filter to be send as filter-param for (initial) /sync requests
        QString filter_;
};
