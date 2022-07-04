#ifndef LOADPICTUREINFOTHREAD_H
#define LOADPICTUREINFOTHREAD_H

#include <QThread>
#include "kernel/render_struct.h"
#include "exiv2/exiv2.hpp" 

enum PictureLoadState
{
    PictureLoadSuccess,
    PictureLoadFailed,
};

#define EXIVDATA_APP "exifdata.exe"

class ParseImageExifByExiv : public QObject, public QRunnable
{
    Q_OBJECT
public:
    ParseImageExifByExiv(const QString& filePath, QObject* parent = 0);
    ~ParseImageExifByExiv() {};
    int loadImageExifData();
    PictureInfo getPicInfo();
    QString getImagePath() { return m_filePath; }

protected:
    void run();

private:
    int parseExifData(const QString& data);
    double ExifPropertyToDecimal(const QList<double>& values);
    QList<double> propertyToNumber(const QString& prop);
    double stringValueToNum(const QString& str);
    std::string findExifKey(Exiv2::ExifData &ed, std::string key);

signals:
    void parseImageFinished(int code);

private:
    QString m_filePath;
    PictureInfo m_picInfo;
};

class LoadPictureInfoThread : public QThread
{
    Q_OBJECT
public:
    LoadPictureInfoThread(const QStringList& path, bool includeSubDir, bool onlyFile = false, QObject *parent = 0);
    ~LoadPictureInfoThread();
    void stop();
    QList<PictureGroup*> getPictureGroups();

signals:    
    void loadPicInfoFinished(PictureGroup*);
    void loadPicInfoFinished(int);

protected:
    virtual void run();

private:
    PictureInfo loadPictureInfo(const QString& path);
    bool findFiles(const QString& path, QMap<QString, QStringList>& files);
    bool findFiles(const QString& path, QStringList& files);
    bool isPictureFile(const QString& path);
    void loadPictureInfoMuiltThread(const QString& path);
    bool isPictureLoadFinished(int totalPics);

private slots:
    void loadPictureInfoInThreadFinished(int);

private:
    bool m_includeSubDir;
    bool m_stop;
    bool m_onlyFile; // ´¿ÎÄ¼þ
    QStringList m_picturesPath;
    QList<PictureGroup*> m_groups;
    QList<PictureInfo> m_tempPinfos;
    QMutex m_pInfoMutex;
};

#endif //LOADPICTUREINFOTHREAD_H