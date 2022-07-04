#include "stdafx.h"
#include "LoadPictureInfoThread.h"
#include "Config/CameraDB.h"
#include "Common/StringUtil.h"
#include "EasyExif/EasyExif.h"
#include "Common/RBHelper.h"
#include <QProcess>
//////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <gdiplus.h>
#include <strsafe.h>
#include <Propkey.h>
#include <comutil.h>
#include <propvarutil.h>
#pragma comment(lib, "comsuppw.lib")

using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

#define MAX_PROPTYPE_SIZE 30
#define USE_WINDOWS_API 1

class LoadPictureInfo
{
public:
    LoadPictureInfo(const QString& filePath);
    ~LoadPictureInfo() {};

    void startLoadInfo();
    PictureInfo getPicInfo();

private:
    QList<double> RationalValueToDouble(const Gdiplus::PropertyItem& buffer);
    QString ASCIIValueToString(const Gdiplus::PropertyItem& buffer);
    double XTitudeToDecimal(const QList<double>& values);
    QString XTitudeToString(const QList<double>& values);

private:
    QString m_filePath;
    PictureInfo m_picInfo;
};
//////////////////////////////////////////////////////////////////////////

class LoadPictureInfoByExiv2
{
public:
    LoadPictureInfoByExiv2(const QString& filePath);
    ~LoadPictureInfoByExiv2() {}

    int startLoadInfo();
    PictureInfo getPicInfo();

private:
    QString m_filePath;
    PictureInfo m_picInfo;
};


//////////////////////////////////////////////////////////////////////////
LoadPictureInfoThread::LoadPictureInfoThread(const QStringList& path, bool includeSubDir, bool onlyFile, QObject *parent)
    : QThread(parent)
    , m_picturesPath(path)
    , m_includeSubDir(includeSubDir)
    , m_stop(false)
    , m_onlyFile(onlyFile)
{
    qRegisterMetaType<PictureGroup*>("PictureGroup*");
    QThreadPool::globalInstance()->setMaxThreadCount(20);
}

LoadPictureInfoThread::~LoadPictureInfoThread()
{
}

void LoadPictureInfoThread::run()
{
    if(m_picturesPath.isEmpty())
        return;
#if 0
    PictureGroup *group = new PictureGroup;
    QList<PictureInfo> pinfos;
    foreach(QString path, m_picturesPath)
    {
        QFileInfo info(path);

        if(info.isDir()) {
            group->groupName = QDir(path).dirName();
            group->groupPath = path;

            QMap<QString, QStringList> picPaths;
            if(findFiles(path, picPaths)) {
                for(int i = 0; i < picPaths.size(); i++) {
                    pinfos.append(loadPictureInfo(picPaths[i]));
                }
            }
        } else {
            group->groupName = info.absoluteDir().dirName();
            group->groupPath = info.absoluteDir().absolutePath();

            pinfos.append(loadPictureInfo(path));
        }
    }
    group->groupId = QUuid::createUuid().toString();
    group->pictures = pinfos;
    group->picsCount = pinfos.count();
    if(!pinfos.isEmpty()) {
        group->picWidth = pinfos[0].picWidth;
        group->picHeight = pinfos[0].picHeight;;
    }
    emit loadPicInfoFinished(group);
#else
    QList<PictureGroup*> groups;

    if(m_onlyFile) { // 只扫描文件
        QList<PictureInfo> pinfos;
        PictureGroup *group = new PictureGroup;
        qint32 totalPics = 0;
        foreach(QString path, m_picturesPath)
        {
            QFileInfo info(path);
            if(info.isFile()) {
                group->groupName = info.absoluteDir().dirName();
                group->groupPath = info.absoluteDir().absolutePath();
#if USE_WINDOWS_API
                pinfos.append(loadPictureInfo(path));
#else
                loadPictureInfoMuiltThread(path);
                totalPics++;
#endif             
            }
        }

#if USE_WINDOWS_API
#else
        // 等待所有照片解析完成
        while (true) {
            if (isPictureLoadFinished(totalPics)) {
                break;
            } else {
                QThread::msleep(100);
            }
        }
        pinfos.swap(m_tempPinfos);
#endif

        group->groupId = QUuid::createUuid().toString();
        group->pictures = pinfos;
        group->picsCount = pinfos.count();
        if(!pinfos.isEmpty()) {
            group->picWidth = pinfos[0].picWidth;
            group->picHeight = pinfos[0].picHeight;
            group->cameraProducter = pinfos[0].cameraProducter;
            group->cameraModel = pinfos[0].cameraModel;
            group->focalLength = pinfos[0].focalLength;
            group->sensorSize = pinfos[0].sensorSize;
            group->totalPixel = group->picsCount * group->picWidth * group->picHeight;
            group->totalPixelStr = RBHelper::pixelsToString(group->totalPixel);
        }
        groups.append(group);
        m_tempPinfos.clear();

    } else {
        foreach(QString path, m_picturesPath)
        {
            QFileInfo info(path);
            if(info.isDir()) {
                QMap<QString, QStringList> picPaths;
                if(findFiles(path, picPaths)) {
                    auto it = picPaths.begin();
                    while(it != picPaths.end()) {
                        QDir dir(it.key());
                        PictureGroup *group = new PictureGroup;
                        group->groupName = dir.dirName();
                        group->groupPath = it.key();

                        QList<PictureInfo> pinfos;
                        qint32 totalPics = 0;
                        foreach(QString filePath, it.value())
                        {
#if USE_WINDOWS_API
                            pinfos.append(loadPictureInfo(filePath));
#else
                            loadPictureInfoMuiltThread(filePath);
                            totalPics++;
#endif                        
                        }

#if USE_WINDOWS_API
#else
                        // 等待所有照片解析完成
                        while (true) {
                            if (isPictureLoadFinished(totalPics)) {
                                break;
                            } else {
                                QThread::msleep(100);
                            }
                        }
                        pinfos.swap(m_tempPinfos);
#endif

                        group->groupId = QUuid::createUuid().toString();
                        group->pictures = pinfos;
                        group->picsCount = pinfos.count();
                        if(!pinfos.isEmpty()) {
                            group->picWidth = pinfos[0].picWidth;
                            group->picHeight = pinfos[0].picHeight;
                            group->cameraProducter = pinfos[0].cameraProducter;
                            group->cameraModel = pinfos[0].cameraModel;
                            group->focalLength = pinfos[0].focalLength;
                            group->sensorSize = pinfos[0].sensorSize;
                            group->totalPixel = group->picsCount * group->picWidth * group->picHeight;
                            group->totalPixelStr = RBHelper::pixelsToString(group->totalPixel);
                        }
                        groups.append(group);
                        m_tempPinfos.clear();

                        ++it;
                    }
                }
            }
        }
    }

    m_groups = groups;
    emit loadPicInfoFinished(PictureLoadSuccess);

#endif
}

void LoadPictureInfoThread::stop()
{
    m_stop = true;
}

QList<PictureGroup*> LoadPictureInfoThread::getPictureGroups()
{
    return m_groups;
}

PictureInfo LoadPictureInfoThread::loadPictureInfo(const QString& path)
{
    QFileInfo file(path);
    PictureInfo info;

#ifndef _DEBUG
    ParseImageExifByExiv pic(path);
    if (pic.loadImageExifData() == PictureLoadSuccess) {
        info = pic.getPicInfo();
        pic.deleteLater();
    } else {
        LoadPictureInfo pic(path);
        pic.startLoadInfo();
        info = pic.getPicInfo();
    }
#else

    LoadPictureInfo pic(path);
    pic.startLoadInfo();
    info = pic.getPicInfo();

#endif

    info.picName = file.fileName();
    info.picPath = path;
    info.picSize = file.size();
    info.fileMtime = file.lastModified().toTime_t();
    info.fileUniqueCode = String::getStringMd5(path + QString::number(info.picSize) + QString::number(info.fileMtime));

    return info;
}

void LoadPictureInfoThread::loadPictureInfoMuiltThread(const QString& path)
{
    ParseImageExifByExiv *pic = new ParseImageExifByExiv(path);
    connect(pic, &ParseImageExifByExiv::parseImageFinished, this, &LoadPictureInfoThread::loadPictureInfoInThreadFinished);
    QThreadPool::globalInstance()->start(pic);
}

void LoadPictureInfoThread::loadPictureInfoInThreadFinished(int code)
{
    PictureInfo info;
    QObject *snder = sender();
    ParseImageExifByExiv *pic = qobject_cast<ParseImageExifByExiv*>(snder);
    if (pic != nullptr) {
        QString filePath = pic->getImagePath();
        QFileInfo file(filePath);
        if (code == PictureLoadSuccess) {
            info = pic->getPicInfo();
        } else {
            LoadPictureInfo pic(pic->getImagePath());
            pic.startLoadInfo();
            info = pic.getPicInfo();
        }

        info.picName = file.fileName();
        info.picPath = filePath;
        info.picSize = file.size();
        info.fileMtime = file.lastModified().toTime_t();
        info.fileUniqueCode = String::getStringMd5(filePath + QString::number(info.picSize) + QString::number(info.fileMtime));

        pic->deleteLater();
    }

    {
        QMutexLocker locker(&m_pInfoMutex);
        m_tempPinfos.append(info);
    }
}

bool LoadPictureInfoThread::isPictureLoadFinished(int totalPics)
{
    QMutexLocker locker(&m_pInfoMutex);
    return m_tempPinfos.size() == totalPics;
}

bool LoadPictureInfoThread::isPictureFile(const QString& path)
{
#if 1
    std::wstring filePath = path.toStdWString();
    Gdiplus::Image image_src(filePath.c_str());
    Gdiplus::Status status = image_src.GetLastStatus();
    int s = status;
    if(status != Gdiplus::Ok) {
        return false;
    }
    GUID guid;
    if(image_src.GetRawFormat(&guid) != Gdiplus::Ok) {
        return false;
    }
    if(guid == Gdiplus::ImageFormatGIF ||
       guid == Gdiplus::ImageFormatJPEG ||
       guid == Gdiplus::ImageFormatPNG ||
       guid == Gdiplus::ImageFormatBMP ||
       guid == Gdiplus::ImageFormatIcon ||
       guid == Gdiplus::ImageFormatTIFF) {
        return true;
    }
    return false;
#else
    QImage img;
    img.load(path);
    if(!img.isNull())
        return true;

    return false;
#endif
}

bool LoadPictureInfoThread::findFiles(const QString& path, QMap<QString, QStringList>& files)
{
    QDir dir(path);
    if(!dir.exists())
        return false;

    QFileInfoList filst;
    QFileInfoList::iterator curFi;
    QStringList pathList;
    pathList << path;

    for(int i = 0; i < pathList.size(); i++) {
        dir.setPath(pathList[i]);
        filst = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot, QDir::DirsFirst);
        if(m_stop) return false;

        if(filst.count() > 0) {

            curFi = filst.begin();
            while(curFi != filst.end()) {

                if(m_stop) return false;

                if(curFi->isDir()) {
                    if(m_includeSubDir) { // 需要扫描子文件夹
                        pathList.push_back(curFi->absoluteFilePath());
                    }
                } else {
                    if(isPictureFile(curFi->absoluteFilePath())) {
                        QString dirPath = curFi->absoluteDir().path();
                        files[dirPath].append(curFi->absoluteFilePath());
                        // files.append(curFi->absoluteFilePath());
                    }
                }
                curFi++;
            }
        }
    }

    return true;
}

bool LoadPictureInfoThread::findFiles(const QString& path, QStringList& files)
{
    QDir dir(path);
    if(!dir.exists())
        return false;

    QFileInfoList filst;
    QFileInfoList::iterator curFi;
    QStringList pathList;
    pathList << path;

    for(int i = 0; i < pathList.size(); i++) {
        dir.setPath(pathList[i]);
        filst = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot, QDir::DirsFirst);
        if(m_stop) return false;

        if(filst.count() > 0) {

            curFi = filst.begin();
            while(curFi != filst.end()) {

                if(m_stop) return false;

                if(curFi->isDir()) {
                    if(m_includeSubDir) { // 需要扫描子文件夹
                        pathList.push_back(curFi->absoluteFilePath());
                    }
                } else {
                    if(isPictureFile(curFi->absoluteFilePath())) {
                        files.append(curFi->absoluteFilePath());
                    }
                }
                curFi++;
            }
        }
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////
LoadPictureInfo::LoadPictureInfo(const QString& filePath)
    :m_filePath(filePath)
{
}

void LoadPictureInfo::startLoadInfo()
{
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Gdiplus::Bitmap>(m_filePath.toStdWString().c_str());
    UINT size = 0;
    UINT count = 0;
    bitmap->GetPropertySize(&size, &count);

    // GetAllPropertyItems returns an array of PropertyItem objects.
    // Allocate a buffer large enough to receive that array.
    std::unique_ptr<PropertyItem[]> pPropBuffer(new PropertyItem[size]);

    // Get the array of PropertyItem objects.
    bitmap->GetAllPropertyItems(size, count, pPropBuffer.get());
    m_picInfo.picWidth = bitmap->GetWidth();
    m_picInfo.picHeight = bitmap->GetHeight();

    // For each PropertyItem in the array, display the id, type, and length.
    for(UINT j = 0; j < count; ++j) {
        auto propItem = pPropBuffer[j];
        qint64 type = propItem.type;
        if(type == PropertyTagTypeRational || type == PropertyTagTypeSRational) {
            switch(pPropBuffer[j].id) {
            case PropertyTagGpsLatitude: // 纬度
                m_picInfo.picLatitude = XTitudeToDecimal(RationalValueToDouble(propItem));
                m_picInfo.picLatitudeChar = XTitudeToString(RationalValueToDouble(propItem));
                break;
            case PropertyTagGpsLongitude: // 经度
                m_picInfo.picLongitude = XTitudeToDecimal(RationalValueToDouble(propItem));
                m_picInfo.picLongitudeChar = XTitudeToString(RationalValueToDouble(propItem));
                break;
            case PropertyTagGpsAltitude:
            {  // 高程
                QList<double> value = RationalValueToDouble(propItem);
                if(!value.isEmpty()) {
                    m_picInfo.picAltitude = value.at(0);
                }
                break;
            }
            case PropertyTagExifFocalLength:
            {  // 焦距
                QList<double> value = RationalValueToDouble(propItem);
                if(!value.isEmpty()) {
                    m_picInfo.focalLength = QString::number(value.at(0), 'f', 2);
                }
                break;
            }
            default:
                break;
            }
        } else if(type == PropertyTagTypeASCII || type == PropertyTagTypeByte) {
            switch(propItem.id) {
            case PropertyTagGpsLatitudeRef: // 纬度
                m_picInfo.picLatitudeRef = ASCIIValueToString(propItem);
                break;
            case PropertyTagGpsLongitudeRef: // 经度
                m_picInfo.picLongitudeRef = ASCIIValueToString(propItem);
                break;
            case PropertyTagGpsAltitudeRef: // 高程
                m_picInfo.picAltitudeRef = ASCIIValueToString(propItem);
                break;
            case PropertyTagGpsMapDatum: // 坐标系
                m_picInfo.picMapDatum = ASCIIValueToString(propItem);
                break;
            case PropertyTagEquipMake: // 相机
            case PropertyTagThumbnailEquipMake:
            case PropertyTagExifMakerNote:
                m_picInfo.cameraProducter = ASCIIValueToString(propItem);
                break;
            case PropertyTagEquipModel: // 相机型号
            case PropertyTagThumbnailEquipModel:
                m_picInfo.cameraModel = ASCIIValueToString(propItem);
                break;
            default:
                QString mm = ASCIIValueToString(propItem);
                qDebug() << "item " << mm;
                break;
            }
        } else {
            QString mm = ASCIIValueToString(propItem);
            qDebug() << "item " << mm;
        }
    }

    QString camera = m_picInfo.cameraProducter + m_picInfo.cameraModel;
    m_picInfo.sensorSize = CameraDB::getInstance()->getSensorSize(camera);
}

PictureInfo LoadPictureInfo::getPicInfo()
{
    return m_picInfo;
}

// RationalValue 由 分子和分母组成，所以一组数据最少两个值
// 得到分子和分母后两者相除  得到真正的值
QList<double> LoadPictureInfo::RationalValueToDouble(const Gdiplus::PropertyItem& buffer)
{
    // 获取所有int
    QList<int> values;
    for(int i = 0; i < buffer.length / sizeof(int); i++) {
        int a = ((int*)buffer.value)[i];
        values.append(a);
    }

    // 计算商
    QList<double> dvalues;
    int count = values.size();
    for(int i = 0; i < count - 1; i += 2) {
        double a = (double)values.at(i);
        double b = (double)values.at(i + 1);
        double c = 0.0;
        if(b != 0) {
            c = a / b;
            dvalues.append(c);
        }
    }

    return dvalues;
}

// 经纬度转换成小数
// 计算方式为：(((秒 / 60) + 分) / 60) + 度 
// 如 12°36'45" -> (((45" / 60) + 36')/60) + 12°
double LoadPictureInfo::XTitudeToDecimal(const QList<double>& values)
{
    if(values.isEmpty() || values.size() < 3)
        return 0;

    double d = values.at(0);
    double m = values.at(1);
    double s = values.at(2);

    double decimal = d + ((s / 60) + m) / 60;

    return decimal;
}

QString LoadPictureInfo::XTitudeToString(const QList<double>& values)
{
    if(values.isEmpty() || values.size() < 3)
        return QString();

    int d = (int)values.at(0);
    int m = (int)values.at(1);
    double s = values.at(2);

    QString value = QString("%1°%2′%3″").arg(d).arg(m).arg(QString::number(s, 'f', TITUDE_DECIMAL_LENGTH));

    return value;
}

QString LoadPictureInfo::ASCIIValueToString(const Gdiplus::PropertyItem& buffer)
{
    return QString::fromLatin1((const char*)buffer.value);
}

//////////////////////////////////////////////////////////////////////////
LoadPictureInfoByExiv2::LoadPictureInfoByExiv2(const QString& filePath)
    :m_filePath(filePath)
{
}

int LoadPictureInfoByExiv2::startLoadInfo()
{
    try
    {
        // char photoPath[512] = {0}; 
        // memcpy(photoPath, m_filePath.toLocal8Bit().constData(), sizeof(photoPath));
        const char* photoPath = qPrintable(m_filePath);
        // Read the JPEG file into a buffer
        FILE *fp = fopen(photoPath, "rb");
        if (!fp) {
            printf("Can't open file.\n");
            return -1;
        }
        fseek(fp, 0, SEEK_END);
        unsigned long fsize = ftell(fp);
        rewind(fp);
        unsigned char *buf = new unsigned char[fsize];
        if (fread(buf, 1, fsize, fp) != fsize) {
            printf("Can't read file.\n");
            delete[] buf;
            return -1;
        }
        fclose(fp);

        // Parse EXIF
        easyexif::EXIFInfo result;
        int code = result.parseFrom(buf, fsize);
        delete[] buf;
        if (code) {
            printf("Error parsing EXIF: code %d\n", code);
            return code;
        }

        m_picInfo.cameraModel = QString::fromStdString(result.Model).remove(QChar(0));
        m_picInfo.cameraProducter = QString::fromStdString(result.Make).remove(QChar(0));
        m_picInfo.picWidth = result.ImageWidth;
        m_picInfo.picHeight = result.ImageHeight;
        m_picInfo.focalLength = QString::number(result.FocalLength, 'g', 2);
        m_picInfo.picLongitude = result.GeoLocation.Longitude;
        m_picInfo.picLatitude = result.GeoLocation.Latitude;
        m_picInfo.picAltitude = result.GeoLocation.Altitude;

        QString camera = m_picInfo.cameraProducter + m_picInfo.cameraModel;
        m_picInfo.sensorSize = CameraDB::getInstance()->getSensorSize(camera);
    } 
    catch (std::exception* e) {
        return -1;
    }

    return 0;
}

PictureInfo LoadPictureInfoByExiv2::getPicInfo()
{
    return m_picInfo;
}

//////////////////////////////////////////////////////////////////////////
ParseImageExifByExiv::ParseImageExifByExiv(const QString& filePath, QObject* parent)
    : QObject(parent)
    , m_filePath(filePath)
{
    setAutoDelete(false);
}

void ParseImageExifByExiv::run()
{
    loadImageExifData();
}

PictureInfo ParseImageExifByExiv::getPicInfo()
{
    return m_picInfo;
}

int ParseImageExifByExiv::loadImageExifData()
{
#if 0
    QString exivdata = QString("%1/%2/%3").arg(QCoreApplication::applicationDirPath()).arg("resource/tools/exif").arg(EXIVDATA_APP);
    QStringList params;
    params.append(m_filePath);
    params.append("json");

    QProcess process;
    process.start(exivdata, params);
    process.waitForFinished();

    QByteArray data = process.readAllStandardOutput();
    return parseExifData(data);
#else
    try
    {
        Exiv2::XmpParser::initialize();
        ::atexit(Exiv2::XmpParser::terminate);
#ifdef EXV_ENABLE_BMFF
        Exiv2::enableBMFF();
#endif

        //输入的图像路径
        std::string image_path = qPrintable(m_filePath);

        std::unique_ptr<Exiv2::Image> image = Exiv2::ImageFactory::open(image_path);

        if (image.get() == 0)
        {
            std::cout << "Read Exif Error." << std::endl;
            return PictureLoadFailed;
        }

        //读取照片的exif信息  
        image->readMetadata();
        Exiv2::ExifData ed = image->exifData();//得到exif信息  

        if (ed.empty())
        {
            std::cout << "Not Find ExifInfo" << std::endl;
            return PictureLoadFailed;
        }

        // std::cout << "版权	:" << FindExifKey(ed, "Exif.Image.Copyright") << std::endl;
        // std::cout << "创作人	:" << FindExifKey(ed, "Exif.Image.Artist") << std::endl;
        // std::cout << "相机品牌	:" << FindExifKey(ed, "Exif.Image.Make") << std::endl;
        // std::cout << "相机型号	:" << FindExifKey(ed, "Exif.Image.Model") << std::endl;
        // std::cout << "镜头型号	:" << FindExifKey(ed, "Exif.Photo.LensModel") << std::endl;
        // std::cout << "镜头序列号	:" << FindExifKey(ed, "Exif.Photo.LensSerialNumber") << std::endl;
        // std::cout << "ISO	:" << FindExifKey(ed, "Exif.Photo.ISOSpeedRatings") << std::endl;
        // std::cout << "光圈	:" << FindExifKey(ed, "Exif.Photo.FNumber") << std::endl;
        // std::cout << "焦距	:" << FindExifKey(ed, "Exif.Photo.FocalLength") << std::endl;
        // std::cout << "快门	:" << FindExifKey(ed, "Exif.Photo.ExposureTime") << std::endl;
        // std::cout << "拍摄时间	:" << FindExifKey(ed, "Exif.Image.DateTime") << std::endl;
        // std::cout << "闪光灯	:" << FindExifKey(ed, "Exif.CanonCs.FlashMode") << std::endl;
        // std::cout << "水平分辨率	:" << FindExifKey(ed, "Exif.Image.XResolution") << std::endl;
        // std::cout << "垂直分辨率	:" << FindExifKey(ed, "Exif.Image.YResolution") << std::endl;
        // std::cout << "照片尺寸	:" << FindExifKey(ed, "Exif.Photo.PixelYDimension") << " x " << FindExifKey(ed, "Exif.Photo.PixelXDimension") << std::endl;

        m_picInfo.cameraProducter = QString::fromStdString(findExifKey(ed, "Exif.Image.Make"));
        m_picInfo.cameraModel     = QString::fromStdString(findExifKey(ed, "Exif.Image.Model"));
        m_picInfo.picWidth        = QString::fromStdString(findExifKey(ed, "Exif.Photo.PixelXDimension")).toInt();
        m_picInfo.picHeight       = QString::fromStdString(findExifKey(ed, "Exif.Photo.PixelYDimension")).toInt();

        m_picInfo.focalLength  = QString::number(stringValueToNum(QString::fromStdString(findExifKey(ed, "Exif.Photo.FocalLength"))));
        m_picInfo.picLongitude = ExifPropertyToDecimal(propertyToNumber(QString::fromStdString(findExifKey(ed, "Exif.GPSInfo.GPSLongitude"))));
        m_picInfo.picLatitude  = ExifPropertyToDecimal(propertyToNumber(QString::fromStdString(findExifKey(ed, "Exif.GPSInfo.GPSLatitude"))));
        m_picInfo.picAltitude  = stringValueToNum(QString::fromStdString(findExifKey(ed, "Exif.GPSInfo.GPSAltitude")));

        QString camera = m_picInfo.cameraProducter + m_picInfo.cameraModel;
        m_picInfo.sensorSize = CameraDB::getInstance()->getSensorSize(camera);

        image->clearMetadata();
        image->clearExifData();

        if (m_picInfo.picWidth == 0 || m_picInfo.picHeight == 0) {
            return PictureLoadFailed;
        }

        return PictureLoadSuccess;
    }
    catch (Exiv2::AnyError& e)
    {
        return PictureLoadFailed;

    }
#endif

}

int ParseImageExifByExiv::parseExifData(const QString& data)
{
    QJsonObject jsonObject;
    QJsonParseError error;
    QJsonDocument jsonDocument(QJsonDocument::fromJson(data.toUtf8(), &error));
    if (jsonDocument.isNull() || jsonDocument.isEmpty() || error.error != QJsonParseError::NoError) {
        LOGFMTE("[RBHelper::loadJsonFile] File parse failed : %s, error msg is %s", qPrintable(data), qPrintable(error.errorString()));
        emit parseImageFinished(PictureLoadFailed);
        return PictureLoadFailed;
    }
    jsonObject = jsonDocument.object();

    m_picInfo.cameraProducter = jsonObject["Exif.Image.Make"].toString();
    m_picInfo.cameraModel     = jsonObject["Exif.Image.Model"].toString();
    m_picInfo.picWidth        = jsonObject["Exif.Photo.PixelXDimension"].toString().toInt();
    m_picInfo.picHeight       = jsonObject["Exif.Photo.PixelYDimension"].toString().toInt();

    m_picInfo.focalLength     = QString::number(stringValueToNum(jsonObject["Exif.Photo.FocalLength"].toString()));
    m_picInfo.picLongitude    = ExifPropertyToDecimal(propertyToNumber(jsonObject["Exif.GPSInfo.GPSLongitude"].toString()));
    m_picInfo.picLatitude     = ExifPropertyToDecimal(propertyToNumber(jsonObject["Exif.GPSInfo.GPSLatitude"].toString()));
    m_picInfo.picAltitude     = stringValueToNum(jsonObject["Exif.GPSInfo.GPSAltitude"].toString());

    QString camera = m_picInfo.cameraProducter + m_picInfo.cameraModel;
    m_picInfo.sensorSize = CameraDB::getInstance()->getSensorSize(camera);

    emit parseImageFinished(PictureLoadSuccess);
    return PictureLoadSuccess;
}

double ParseImageExifByExiv::ExifPropertyToDecimal(const QList<double>& values)
{
    if (values.isEmpty() || values.size() < 3)
        return 0;

    double d = values.at(0);
    double m = values.at(1);
    double s = values.at(2);

    double decimal = d + ((s / 60) + m) / 60;

    return decimal;
}

// "Exif.Photo.FocalLength":"1500/100",
double ParseImageExifByExiv::stringValueToNum(const QString& str)
{
    if (str.isEmpty() || !str.contains("/"))
        return 0;
    QStringList nums = str.split("/");
    if (nums.size() < 2) return nums[0].toDouble();

    qint32 num1 = nums[0].toInt();
    qint32 num2 = nums[1].toInt();

    return num2 != 0 ? (double)num1 / (double)num2 : 0; // 如果除数为0.则直接返回0
}

// "Exif.GPSInfo.GPSAltitude":"121005/1000",
// "Exif.GPSInfo.GPSLatitude":"22/1 52/1 33871/10000",
// "Exif.GPSInfo.GPSLongitude":"108/1 27/1 397914/10000",
QList<double> ParseImageExifByExiv::propertyToNumber(const QString& prop)
{
    QString data = prop;
    data = data.replace(" ", ";");
    QStringList datas = data.split(";");
    QList<double> nums;

    foreach(QString num, datas) {
        if (num.contains("/")) {
            nums.append(stringValueToNum(num));
        }
    }

    return nums;
}

std::string ParseImageExifByExiv::findExifKey(Exiv2::ExifData &ed, std::string key)
{
    Exiv2::ExifKey tmp = Exiv2::ExifKey(key);
    Exiv2::ExifData::iterator pos = ed.findKey(tmp);
    if (pos == ed.end())
    {
        return "Unknow";
    }
    return pos->value().toString();
}