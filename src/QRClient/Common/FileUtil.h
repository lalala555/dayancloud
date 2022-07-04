/***********************************************************************
* Module:  FileUtil.h
* Author:  hqr
* Modified: 2016/09/27 14:20:22
* Purpose: Declaration of the class
***********************************************************************/
#ifndef FILEUTIL_H_
#define FILEUTIL_H_

#include <qglobal.h>
#include <QFileInfo>
#include <QDir>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QCoreApplication>
#include <QProcess>
#include "xzip/XUnzip.h"

class File
{
public:
    static QString getFileBaseName(const QString& strPath)
    {
        QFileInfo info(strPath);
        return info.fileName();
    }

    static qint64 getFileSize(const QString& filePath)
    {
        qint64 size = 0;
        QFileInfo fileInfo(filePath);

        if (fileInfo.exists()) {
            size = fileInfo.size();
        }

        return size;
    }

    static QByteArray getFileMd5(QString filePath)
    {
        QFile localFile(filePath);
        if (!localFile.open(QFile::ReadOnly)) {
            return QByteArray();
        }

        QCryptographicHash ch(QCryptographicHash::Md5);

        qint64 totalBytes = 0;
        qint64 bytesWritten = 0;
        qint64 bytesToWrite = 0;
        qint64 loadSize = 1024 * 4;
        QByteArray buf;

        totalBytes = localFile.size();
        bytesToWrite = totalBytes;

        while (1) {
            if (bytesToWrite > 0) {
                buf = localFile.read(qMin(bytesToWrite, loadSize));
                ch.addData(buf);
                bytesWritten += buf.length();
                bytesToWrite -= buf.length();
                buf.resize(0);
            }
            else {
                break;
            }

            if (bytesWritten == totalBytes) {
                break;
            }
        }

        localFile.close();
        QByteArray md5 = ch.result();
        return md5;
    }

    static bool copyRecursively(const QString &srcFilePath, const QString &tgtFilePath)
    {
        QFileInfo srcFileInfo(srcFilePath);
        if (srcFileInfo.isDir()) {
            QDir targetDir(tgtFilePath);
            targetDir.cdUp();
            if (!targetDir.mkdir(QFileInfo(tgtFilePath).fileName()))
                return false;
            QDir sourceDir(srcFilePath);
            QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
            foreach(const QString &fileName, fileNames) {
                const QString newSrcFilePath
                    = srcFilePath + QLatin1Char('/') + fileName;
                const QString newTgtFilePath
                    = tgtFilePath + QLatin1Char('/') + fileName;
                if (!copyRecursively(newSrcFilePath, newTgtFilePath))
                    return false;
            }
        }
        else {
            if (!QFile::copy(srcFilePath, tgtFilePath))
                return false;
        }
        return true;
    }

    static QJsonObject buildUploadFileObject(const QString& filePath)
    {
        QJsonObject upload_scene_object;

        QString server_path;
        QString path = filePath;
        path.replace(":", "");
        path.replace("\\", "/");
        if (path.startsWith("//")) {
            path.replace("//", "");
            server_path = QString("/net/%1").arg(path);
        } else {
            server_path = QString("/%1").arg(path);
        }

        upload_scene_object["local"] = filePath;
        upload_scene_object["server"] = server_path;

        return upload_scene_object;
    }

    static bool unZipPackage(const QString& zippedFile, const QString& destDir)
    {
#ifdef Q_OS_WIN
        std::wstring zip_file = zippedFile.toStdWString();
        std::wstring dest_dir = destDir.toStdWString();
        TCHAR currentDir[MAX_PATH] = { 0 };
        GetCurrentDirectory(MAX_PATH, currentDir);

        SetCurrentDirectory(dest_dir.c_str());

        HZIP hz = OpenZip((void *)zip_file.c_str(), 0, ZIP_FILENAME);
        if (hz == NULL)
            return false;
        ZIPENTRYW ze;
        GetZipItem(hz, -1, &ze);
        int numitems = ze.index;
        // -1 gives overall information about the zipfile
        for (int zi = 0; zi < numitems; zi++) {
            GetZipItem(hz, zi, &ze); // // fetch individual details
            wchar_t dest[MAX_PATH];
            swprintf(dest, MAX_PATH, L"%s\\%s", dest_dir.c_str(), ze.name);
            UnzipItem(hz, zi, dest, MAX_PATH, ZIP_FILENAME);
        }
        CloseZip(hz);

        SetCurrentDirectory(currentDir);
#else
        QString command;
        QStringList arguments;

        if (zippedFile.endsWith(".zip")) {
            command = "/usr/bin/unzip";
            arguments << zippedFile << "-d" << destDir;
        } else if (zippedFile.endsWith(".tar.gz")) {
            command = "/usr/bin/tar";
            arguments << "-xvzf" << zippedFile << "-C" << destDir;
        }
        QProcess cmdexe;
        cmdexe.start(command, arguments);
        cmdexe.waitForFinished();
#endif // !Q_OS_WIN
        return true;
    }
};

#endif // FILEUTIL_H_
