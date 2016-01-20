/****************************************************************************
**
** Copyright (C) 2013 Klaralvdalens Datakonsult AB (KDAB)
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Installer Framework.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include "kdtoolsglobal.h"

#include <QtCore/QObject>
#include <QtCore/QUrl>

#include <QtNetwork/QAuthenticator>

namespace KDUpdater {

class FileDownloaderProxyFactory;

class KDTOOLS_EXPORT FileDownloader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool autoRemoveDownloadedFile READ isAutoRemoveDownloadedFile WRITE setAutoRemoveDownloadedFile)
    Q_PROPERTY(QUrl url READ url WRITE setUrl)
    Q_PROPERTY(QString scheme READ scheme WRITE setScheme)

public:
    explicit FileDownloader(const QString &scheme, QObject *parent = 0);
    ~FileDownloader();

    QUrl url() const;
    void setUrl(const QUrl &url);

    QByteArray sha1Sum() const;

    QByteArray assumedSha1Sum() const;
    void setAssumedSha1Sum(const QByteArray &sha1);

    QString scheme() const;
    void setScheme(const QString &scheme);

    QString errorString() const;

    virtual bool canDownload() const = 0;
    virtual bool isDownloaded() const = 0;
    virtual QString downloadedFileName() const = 0;
    virtual void setDownloadedFileName(const QString &name) = 0;
    virtual FileDownloader *clone(QObject *parent=0) const = 0;

    void download();

    void setAutoRemoveDownloadedFile(bool val);
    bool isAutoRemoveDownloadedFile() const;

    void setFollowRedirects(bool val);
    bool followRedirects() const;

    FileDownloaderProxyFactory *proxyFactory() const;
    void setProxyFactory(FileDownloaderProxyFactory *factory);

    QAuthenticator authenticator() const;
    void setAuthenticator(const QAuthenticator &authenticator);

    bool ignoreSslErrors();
    void setIgnoreSslErrors(bool ignore);

public Q_SLOTS:
    virtual void cancelDownload();

protected:
    virtual void onError() = 0;
    virtual void onSuccess() = 0;

Q_SIGNALS:
    void downloadStarted();
    void downloadCanceled();

    void downloadProgress(double progress);
    void estimatedDownloadTime(int seconds);
    void downloadSpeed(qint64 bytesPerSecond);
    void downloadStatus(const QString &status);
    void downloadProgress(qint64 bytesReceived, qint64 bytesToReceive);
    void authenticatorChanged(const QAuthenticator &authenticator);
    void downloadCompleted();
    void downloadAborted(const QString &errorMessage);

protected:
    void setDownloadCanceled();
    void setDownloadCompleted();
    void setDownloadAborted(const QString &error);

    void runDownloadSpeedTimer();
    void stopDownloadSpeedTimer();

    void addSample(qint64 sample);
    int downloadSpeedTimerId() const;
    void setProgress(qint64 bytesReceived, qint64 bytesToReceive);

    void emitDownloadSpeed();
    void emitDownloadStatus();
    void emitDownloadProgress();
    void emitEstimatedDownloadTime();

    void addCheckSumData(const QByteArray &data);
    void addCheckSumData(const char *data, int length);
    void resetCheckSumData();

private Q_SLOTS:
    virtual void doDownload() = 0;

private:
    struct Private;
    Private *d;
};

} // namespace KDUpdater

#endif // FILEDOWNLOADER_H
