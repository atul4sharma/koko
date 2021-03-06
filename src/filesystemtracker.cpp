/*
 * Copyright (C) 2017 Atul Sharma <atulsharma406@gmail.com>
 * Copyright (C) 2014  Vishesh Handa <me@vhanda.in>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "filesystemtracker.h"

#include "filesystemimagefetcher.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <QDBusConnection>

#include <QMimeDatabase>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

#include <KDirNotify>
#include <kdirwatch.h>

FileSystemTracker::FileSystemTracker(QObject* parent)
    : QObject(parent)
{
    QObject::connect(KDirWatch::self(), &KDirWatch::dirty,
                     this, &FileSystemTracker::setSubFolder);


    org::kde::KDirNotify *kdirnotify = new org::kde::KDirNotify(QString(), QString(), QDBusConnection::sessionBus(), this);

    connect(kdirnotify, &org::kde::KDirNotify::FilesRemoved,
            this, [this](const QStringList &files) {
                for (const QString & filePath : files) {
                    removeFile(filePath);
                }
            });
    connect(kdirnotify, &org::kde::KDirNotify::FilesAdded,
            this, &FileSystemTracker::setSubFolder);
    connect(kdirnotify, &org::kde::KDirNotify::FileRenamedWithLocalPath,
            this, [this](const QString &src, const QString &dst, const QString &dstPath) {
                removeFile(src);
                slotNewFiles({dst});
            });

    //
    // Real time updates
    //
    QDBusConnection con = QDBusConnection::sessionBus();
    con.connect(QString(), QLatin1String("/files"), QLatin1String("org.kde"),
                QLatin1String("changed"), this, SLOT(slotNewFiles(QStringList)));
    
    connect( this, &FileSystemTracker::subFolderChanged, 
             this, &FileSystemTracker::reindexSubFolder);

    //
    // Database
    //
    static QString dir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/koko/";
    QDir().mkpath(dir);

    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), QStringLiteral("fstracker"));
    db.setDatabaseName(dir + "/fstracker.sqlite3");
    if (!db.open()) {
        qWarning() << "Failed to open db" << db.lastError().text();
        return;
    }

    if (db.tables().contains("files")) {
        return;
    }

    QSqlQuery query(db);
    bool ret = query.exec(QLatin1String("CREATE TABLE files("
                          "id INTEGER PRIMARY KEY, "
                          "url TEXT NOT NULL UNIQUE)"));
    if (!ret) {
        qWarning() << "Could not create files table" << query.lastError().text();
        return;
    }

    ret = query.exec(QLatin1String("CREATE INDEX fileUrl_index ON files (url)"));
    if (!ret) {
        qWarning() << "Could not create tags index" << query.lastError().text();
        return;
    }

    //
    // WAL Journaling mode has much lower io writes than the traditional journal
    // based indexing.
    //
    ret = query.exec(QLatin1String("PRAGMA journal_mode = WAL"));
    if (!ret) {
        qWarning() << "Could not set WAL journaling mode" << query.lastError().text();
        return;
    }
}

FileSystemTracker::~FileSystemTracker()
{
    QSqlDatabase::removeDatabase(QStringLiteral("fstracker"));
}

void FileSystemTracker::slotImageResult(const QString& filePath)
{
    QSqlQuery query(QSqlDatabase::database("fstracker"));
    query.prepare("SELECT id from files where url = ?");
    query.addBindValue(filePath);
    if (!query.exec()) {
        qDebug() << query.lastError();
        return;
    }

    if (!query.next()) {
        QSqlQuery query(QSqlDatabase::database("fstracker"));
        query.prepare("INSERT into files(url) VALUES (?)");
        query.addBindValue(filePath);
        if (!query.exec()) {
            qDebug() << query.lastError();
            return;
        }
        qDebug() << "ADDED" << filePath;
        emit imageAdded(filePath);
    }

    m_filePaths << filePath;
}

void FileSystemTracker::slotFetchFinished()
{
    QSqlQuery query(QSqlDatabase::database("fstracker"));
    query.prepare("SELECT url from files");
    if (!query.exec()) {
        qDebug() << query.lastError();
        return;
    }

    while (query.next()) {
        QString filePath = query.value(0).toString();

        if (filePath.contains(m_subFolder) && !m_filePaths.contains(filePath)) {
            removeFile(filePath);
        }
    }

    QSqlDatabase::database("fstracker").commit();

    m_filePaths.clear();
    emit initialScanComplete();
}

void FileSystemTracker::removeFile(const QString &filePath)
{
    qDebug() << "REMOVED" << filePath;
    emit imageRemoved(filePath);
    QSqlQuery query(QSqlDatabase::database("fstracker"));
    query.prepare("DELETE from files where url = ?");
    query.addBindValue(filePath);
    if (!query.exec()) {
        qWarning() << query.lastError();
    }
}

void FileSystemTracker::slotNewFiles(const QStringList& files)
{
    if (!m_filePaths.isEmpty()) {
        // A scan is already going on. No point interrupting it.
        return;
    }

    QMimeDatabase db;
    for (const QString& file: files) {
        QMimeType mimetype = db.mimeTypeForFile(file);
        if (mimetype.name().startsWith("image/")) {
            slotImageResult(file);
        }
    }

    m_filePaths.clear();
}


void FileSystemTracker::setFolder(const QString &folder)
{
    if (m_folder == folder) {
        return;
    }

    KDirWatch::self()->removeDir(m_folder);
    m_folder = folder;
    KDirWatch::self()->addDir(m_folder, KDirWatch::WatchSubDirs);
}

QString FileSystemTracker::folder() const
{
    return m_folder;
}

void FileSystemTracker::setSubFolder(const QString& folder)
{
    if(QFileInfo(folder).isDir()) {
        m_subFolder = folder;
        emit subFolderChanged();
    }
}

void FileSystemTracker::reindexSubFolder()
{
    FileSystemImageFetcher* fetcher = new FileSystemImageFetcher(m_subFolder);
    connect(fetcher, &FileSystemImageFetcher::imageResult,
            this, &FileSystemTracker::slotImageResult, Qt::QueuedConnection);
    connect(fetcher, &FileSystemImageFetcher::finished,
            this, &FileSystemTracker::slotFetchFinished, Qt::QueuedConnection);
    
    fetcher->fetch();
    
    QSqlDatabase::database("fstracker").transaction();

}
