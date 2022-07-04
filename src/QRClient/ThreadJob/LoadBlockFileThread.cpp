#include "stdafx.h"
#include "LoadBlockFileThread.h"
#include "Common/QXmlReader.h"
#include <QProcess>

LoadBlockFileThread::LoadBlockFileThread(const QString& filePath, qint64 taskId, QObject *parent)
    : QThread(parent)
    , m_filePath(filePath)
    , m_taskId(taskId)
    , m_blockExchange(new BLOCK_TASK::BlockExchange())
	, m_tilePoints(false)
{
}

LoadBlockFileThread::~LoadBlockFileThread()
{
}

void LoadBlockFileThread::run()
{
    int code = startLoadFile();
    emit loadBlockFileFinished(code);
}

qint64 LoadBlockFileThread::getTaskId()
{
    return m_taskId;
}

BLOCK_TASK::BlockExchange* LoadBlockFileThread::getBlockExchangeInfo()
{
    return m_blockExchange;
}

int LoadBlockFileThread::startLoadFile()
{
	if (checkTiePoints())
	{
		return loadXmlFileByXAS();
		return loadXmlFileByDOM();
	}
	return CODE_ERROT_NOT_TIEPOINT_FILE;
}

bool LoadBlockFileThread::unzipXmlzFile(const QString& src, const QString& dest)
{
    QString tool = getUnzipToolPath();
    QString code = QString("%1 x \"%2\" -o\"%3\" -aoa").arg(tool).arg(src).arg(dest);

    QProcess unzip;
    unzip.start(code);
    unzip.waitForFinished();

    if (unzip.exitCode() != 0)
        return false;

    return true;
}

QString LoadBlockFileThread::unzipXmlFileTempPath()
{
    QString temp = QString("%1/%2").arg("xmlz_file").arg(m_taskId); 
    temp = RBHelper::getTempPath(temp);
    temp = QDir::toNativeSeparators(temp);

    return temp;
}

QString LoadBlockFileThread::getUnzipToolPath()
{
    QString appPath = QApplication::applicationDirPath();
    QString tool = QString("%1/%2").arg(appPath).arg("resource/tools/7zip/7z.exe");

    return tool;
}

int LoadBlockFileThread::loadXmlFileByDOM()
{
    // 如果文件是xmlz,则解压后再解析
    QString xmlPath = m_filePath;
    QFileInfo info(m_filePath);
    if (info.suffix() == "xmlz") {
        QString unzipDir = unzipXmlFileTempPath();
        if (!unzipXmlzFile(m_filePath, unzipDir)) {
            return CODE_FILE_LOAD_FAILED_ERROR;
        }
        xmlPath = QString("%1/%2.xml").arg(unzipDir).arg(info.baseName());
    }

    QXmlReader xmlReader;

    if (!xmlReader.loadXml(xmlPath))
        return CODE_FILE_LOAD_FAILED_ERROR;

    if (!xmlReader.getElementByTagName("Block").isNull()) {
        QDomElement block = xmlReader.getElementByTagName("Block");
        QDomNodeList nodes = block.childNodes();
        int items = nodes.size();
        for (int i = 0; i < nodes.size(); i++) {
            QDomNode node = nodes.at(i);
            if (node.isNull()) continue;
            QDomElement child = node.toElement();
            if (child.isNull()) continue;

            if (child.tagName() == "Name") {
                m_blockExchange->block.name = child.text();
            }
            else if (child.tagName() == "Description") {
                m_blockExchange->block.description = child.text();
            }
            else if (child.tagName() == "SRSId") {
                m_blockExchange->block.SRSId = child.text().toInt();
            }
            else if (child.tagName() == "Photogroups") {
                QDomNodeList groups = child.childNodes();
                items = groups.size();
                for (int j = 0; j < groups.size(); j++) {
                    QDomNode group = groups.at(j);
                    if (group.isNull()) continue;
                    QDomElement groupElement = group.toElement();
                    if (groupElement.isNull()) continue;

                    if (groupElement.tagName() == "Photogroup") {
                        QDomNodeList photogroups = groupElement.childNodes();
                        items = photogroups.size();
                        BLOCK_TASK::Photogroup photogroupInfo;

                        for (int g = 0; g < photogroups.size(); g++) {
                            QDomNode photogroup = photogroups.at(g);
                            if (photogroup.isNull()) continue;
                            QDomElement photogroupElement = photogroup.toElement();
                            if (photogroupElement.isNull()) continue;

                            if (photogroupElement.tagName() == "Name") {
                                photogroupInfo.name = photogroupElement.text();
                            }
                            else if (photogroupElement.tagName() == "CameraModelType") {
                                photogroupInfo.cameraModelType = photogroupElement.text();
                            }
                            else if (photogroupElement.tagName() == "CameraModelBand") {
                                photogroupInfo.cameraModelBand = photogroupElement.text();
                            }
                            else if (photogroupElement.tagName() == "FocalLength") {
                                photogroupInfo.focalLength = photogroupElement.text();
                            }
                            else if (photogroupElement.tagName() == "SensorSize") {
                                photogroupInfo.sensorSize = photogroupElement.text();
                            }
                            else if (photogroupElement.tagName() == "CameraOrientation") {
                                photogroupInfo.cameraOrientation = photogroupElement.text();
                            }
                            else if (photogroupElement.tagName() == "AspectRatio") {
                                photogroupInfo.aspectRatio = photogroupElement.text();
                            }
                            else if (photogroupElement.tagName() == "Skew") {
                                photogroupInfo.skew = photogroupElement.text();
                            }
                            else if (photogroupElement.tagName() == "ImageDimensions") {
                                QDomNodeList imageDimensions = photogroupElement.childNodes();
                                for (int k = 0; k < imageDimensions.size(); k++) {
                                    QDomNode dimensions = imageDimensions.at(k);
                                    if (dimensions.isNull()) continue;
                                    QDomElement dimension = dimensions.toElement();
                                    if (dimension.isNull()) continue;

                                    if (dimension.tagName() == "Width") {
                                        photogroupInfo.imageDimensions.width = dimension.text();
                                    }
                                    else if (dimension.tagName() == "Height") {
                                        photogroupInfo.imageDimensions.height = dimension.text();
                                    }
                                }

                            }
                            else if (photogroupElement.tagName() == "PrincipalPoint") {
                                QDomNodeList principalPoint = photogroupElement.childNodes();
                                for (int h = 0; h < principalPoint.size(); h++) {
                                    QDomNode principals = principalPoint.at(h);
                                    if (principals.isNull()) continue;
                                    QDomElement principal = principals.toElement();
                                    if (principal.isNull()) continue;

                                    if (principal.tagName() == "x") {
                                        photogroupInfo.principalPoint.x = principal.text();
                                    }
                                    else if (principal.tagName() == "y") {
                                        photogroupInfo.principalPoint.y = principal.text();
                                    }
                                }

                            }
                            else if (photogroupElement.tagName() == "Distortion") {
                                QDomNodeList distortion = photogroupElement.childNodes();
                                for (int m = 0; m < distortion.size(); m++) {
                                    QDomNode distortions = distortion.at(m);
                                    if (distortions.isNull()) continue;
                                    QDomElement distor = distortions.toElement();
                                    if (distor.isNull()) continue;

                                    if (distor.tagName() == "K1") {
                                        photogroupInfo.distortion.k1 = distor.text();
                                    }
                                    else if (distor.tagName() == "K2") {
                                        photogroupInfo.distortion.k2 = distor.text();
                                    }
                                    else if (distor.tagName() == "K3") {
                                        photogroupInfo.distortion.k3 = distor.text();
                                    }
                                    else if (distor.tagName() == "P1") {
                                        photogroupInfo.distortion.p1 = distor.text();
                                    }
                                    else if (distor.tagName() == "P2") {
                                        photogroupInfo.distortion.p1 = distor.text();
                                    }
                                }

                            }
                            else if (photogroupElement.tagName() == "Photo") {
                                QDomNodeList photos = photogroupElement.childNodes();
                                BLOCK_TASK::Photo *photoInfo = new BLOCK_TASK::Photo;
                                for (int n = 0; n < photos.size(); n++) {
                                    QDomNode photo = photos.at(n);
                                    if (photo.isNull()) continue;
                                    QDomElement photoElement = photo.toElement();
                                    if (photoElement.isNull()) continue;


                                    if (photoElement.tagName() == "Id") {
                                        photoInfo->id = photoElement.text().toInt();
                                    }
                                    else if (photoElement.tagName() == "ImagePath") {
                                        photoInfo->imagePath = photoElement.text();
                                    }
                                    else if (photoElement.tagName() == "Component") {
                                        photoInfo->component = photoElement.text();
                                    }
                                    else if (photoElement.tagName() == "NearDepth") {
                                        photoInfo->nearDepth = photoElement.text();
                                    }
                                    else if (photoElement.tagName() == "MedianDepth") {
                                        photoInfo->medianDepth = photoElement.text();
                                    }
                                    else if (photoElement.tagName() == "FarDepth") {
                                        photoInfo->farDepth = photoElement.text();
                                    }
                                    else if (photoElement.tagName() == "Pose") {
                                        QDomNodeList pose = photoElement.childNodes();
                                        for (int k = 0; k < pose.size(); k++) {
                                            QDomNode poseNode = pose.at(k);
                                            if (poseNode.isNull()) continue;
                                            QDomElement element = poseNode.toElement();
                                            if (element.isNull()) continue;

                                            if (element.tagName() == "Rotation") {
                                                QDomNodeList rotation = element.childNodes();
                                                for (int m = 0; m < rotation.size(); m++) {
                                                    QDomNode rotationNode = rotation.at(m);
                                                    if (rotationNode.isNull()) continue;
                                                    QDomElement elementRotation = rotationNode.toElement();
                                                    if (elementRotation.isNull()) continue;

                                                    if (elementRotation.tagName() == "M_00") {
                                                        photoInfo->pose.rotation.M_00 = elementRotation.text();
                                                    }
                                                    else if (elementRotation.tagName() == "M_01") {
                                                        photoInfo->pose.rotation.M_01 = elementRotation.text();
                                                    }
                                                    else if (elementRotation.tagName() == "M_02") {
                                                        photoInfo->pose.rotation.M_02 = elementRotation.text();
                                                    }
                                                    else if (elementRotation.tagName() == "M_10") {
                                                        photoInfo->pose.rotation.M_10 = elementRotation.text();
                                                    }
                                                    else if (elementRotation.tagName() == "M_11") {
                                                        photoInfo->pose.rotation.M_11 = elementRotation.text();
                                                    }
                                                    else if (elementRotation.tagName() == "M_12") {
                                                        photoInfo->pose.rotation.M_12 = elementRotation.text();
                                                    }
                                                    else if (elementRotation.tagName() == "M_20") {
                                                        photoInfo->pose.rotation.M_20 = elementRotation.text();
                                                    }
                                                    else if (elementRotation.tagName() == "M_21") {
                                                        photoInfo->pose.rotation.M_21 = elementRotation.text();
                                                    }
                                                    else if (elementRotation.tagName() == "M_22") {
                                                        photoInfo->pose.rotation.M_22 = elementRotation.text();
                                                    }
                                                }

                                            }
                                            else if (element.tagName() == "Center") {
                                                QDomNodeList center = element.childNodes();
                                                for (int m = 0; m < center.size(); m++) {
                                                    QDomNode centerNode = center.at(m);
                                                    if (centerNode.isNull()) continue;
                                                    QDomElement elementCenter = centerNode.toElement();
                                                    if (elementCenter.isNull()) continue;

                                                    if (elementCenter.tagName() == "x") {
                                                        photoInfo->pose.center.x = elementCenter.text();
                                                    }
                                                    else if (elementCenter.tagName() == "y") {
                                                        photoInfo->pose.center.y = elementCenter.text();
                                                    }
                                                    else if (elementCenter.tagName() == "z") {
                                                        photoInfo->pose.center.z = elementCenter.text();
                                                    }
                                                }

                                            }
                                            else if (element.tagName() == "Metadata") {
                                                QDomNodeList metadata = element.childNodes();
                                                for (int m = 0; m < metadata.size(); m++) {
                                                    QDomNode metadataNode = metadata.at(m);
                                                    if (metadataNode.isNull()) continue;
                                                    QDomElement elementMetadata = metadataNode.toElement();
                                                    if (elementMetadata.isNull()) continue;

                                                    if (elementMetadata.tagName() == "SRSId") {
                                                        photoInfo->pose.metaData.srsId = elementMetadata.text().toInt();

                                                    }
                                                    else if (elementMetadata.tagName() == "Center") {
                                                        QDomNodeList center = elementMetadata.childNodes();
                                                        for (int m = 0; m < center.size(); m++) {
                                                            QDomNode centerNode = center.at(m);
                                                            if (centerNode.isNull()) continue;
                                                            QDomElement elementCenter = centerNode.toElement();
                                                            if (elementCenter.isNull()) continue;

                                                            if (elementCenter.tagName() == "x") {
                                                                photoInfo->pose.metaData.center.x = elementCenter.text();
                                                            }
                                                            else if (elementCenter.tagName() == "y") {
                                                                photoInfo->pose.metaData.center.y = elementCenter.text();
                                                            }
                                                            else if (elementCenter.tagName() == "z") {
                                                                photoInfo->pose.metaData.center.z = elementCenter.text();
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    else if (photoElement.tagName() == "ExifData") {
                                        QDomNodeList exifData = photoElement.childNodes();
                                        for (int m = 0; m < exifData.size(); m++) {
                                            QDomNode exifNode = exifData.at(m);
                                            if (exifNode.isNull()) continue;
                                            QDomElement exifElement = exifNode.toElement();
                                            if (exifElement.isNull()) continue;

                                            if (exifElement.tagName() == "FocalLength") {
                                                photoInfo->exifData.focalLength = exifElement.text();
                                            }
                                            else if (exifElement.tagName() == "FocalLength35mmEq") {
                                                photoInfo->exifData.focalLength35mmEq = exifElement.text();
                                            }
                                            else if (exifElement.tagName() == "Make") {
                                                photoInfo->exifData.make = exifElement.text();
                                            }
                                            else if (exifElement.tagName() == "Model") {
                                                photoInfo->exifData.model = exifElement.text();
                                            }
                                            else if (exifElement.tagName() == "LensModel") {
                                                photoInfo->exifData.lensModel = exifElement.text();
                                            }
                                            else if (exifElement.tagName() == "DateTimeOriginal") {
                                                photoInfo->exifData.dateTimeOriginal = exifElement.text();
                                            }
                                            else if (exifElement.tagName() == "YawPitchRoll") {
                                                photoInfo->exifData.yawPitchRoll = exifElement.text();
                                            }
                                            else if (exifElement.tagName() == "GPS") {
                                                QDomNodeList GPS = photoElement.childNodes();
                                                for (int m = 0; m < GPS.size(); m++) {
                                                    QDomNode gpsNode = GPS.at(m);
                                                    if (gpsNode.isNull()) continue;
                                                    QDomElement gpsElement = gpsNode.toElement();
                                                    if (gpsElement.isNull()) continue;

                                                    if (gpsElement.tagName() == "Latitude") {
                                                        photoInfo->exifData.gps.Latitude = gpsElement.text();
                                                    }
                                                    else if (gpsElement.tagName() == "Longitude") {
                                                        photoInfo->exifData.gps.Longitude = gpsElement.text();
                                                    }
                                                    else if (gpsElement.tagName() == "Altitude") {
                                                        photoInfo->exifData.gps.Altitude = gpsElement.text();
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                if (photoInfo->id >= 0) {
                                    photogroupInfo.photos.append(photoInfo);
                                }
                            }
                        }
                        m_blockExchange->block.photogroups.append(photogroupInfo);
                    }
                }
            }
            else if (child.tagName() == "ControlPoints") {

                QDomNodeList nodes = child.childNodes();
                for (int i = 0; i < nodes.size(); i++) {
                    QDomNode node = nodes.at(i);
                    if (node.isNull()) continue;
                    QDomElement child = node.toElement();
                    if (child.isNull()) continue;

                    if (child.tagName() == "ControlPoint") {
                        BLOCK_TASK::ControlPoint* ctrlPoint = new BLOCK_TASK::ControlPoint;
                        QDomNodeList controlPoints = child.childNodes();
                        for (int j = 0; j < controlPoints.size(); j++) {
                            QDomNode ctrlNode = controlPoints.at(j);
                            if (ctrlNode.isNull()) continue;
                            QDomElement ctrlElement = ctrlNode.toElement();
                            if (ctrlElement.isNull()) continue;

                            if (ctrlElement.tagName() == "SRSId") {
                                ctrlPoint->SRSId = ctrlElement.text().toInt();
                            }
                            else if (ctrlElement.tagName() == "Name") {
                                ctrlPoint->name = ctrlElement.text();
                            }
                            else if (ctrlElement.tagName() == "Category") {
                                ctrlPoint->category = ctrlElement.text();
                            }
                            else if (ctrlElement.tagName() == "HorizontalAccuracy") {
                                ctrlPoint->horizontalAccuracy = ctrlElement.text();
                            }
                            else if (ctrlElement.tagName() == "VerticalAccuracy") {
                                ctrlPoint->verticalAccuracy = ctrlElement.text();
                            }
                            else if (ctrlElement.tagName() == "CheckPoint") {
                                ctrlPoint->checkPoint = ctrlElement.text() == "true" ? true : false;
                            }
                            else if (ctrlElement.tagName() == "Position") {
                                QDomNodeList position = ctrlElement.childNodes();
                                for (int j = 0; j < position.size(); j++) {
                                    QDomNode posNode = position.at(j);
                                    if (posNode.isNull()) continue;
                                    QDomElement posElement = posNode.toElement();
                                    if (posElement.isNull()) continue;

                                    if (posElement.tagName() == "x") {
                                        ctrlPoint->position.x = posElement.text();
                                    }
                                    else if (posElement.tagName() == "y") {
                                        ctrlPoint->position.y = posElement.text();
                                    }
                                    else if (posElement.tagName() == "z") {
                                        ctrlPoint->position.z = posElement.text();
                                    }
                                }
                            }
                        }
                        m_blockExchange->block.controlPoints.append(ctrlPoint);
                    }
                }
            }
        }
    }

    return CODE_SUCCESS;
}

int LoadBlockFileThread::loadXmlFileByXAS()
{
    // 如果文件是xmlz,则解压后再解析
    QString xmlPath = m_filePath;
    QFileInfo info(m_filePath);
    if (info.suffix() == "xmlz") {
        QString unzipDir = unzipXmlFileTempPath();
        if (!unzipXmlzFile(m_filePath, unzipDir)) {
            return CODE_FILE_LOAD_FAILED_ERROR;
        }
        xmlPath = QString("%1/%2.xml").arg(unzipDir).arg(info.baseName());
    }

    QFile file(xmlPath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return CODE_FILE_LOAD_FAILED_ERROR;
    }

    QXmlStreamReader reader;
    reader.setDevice(&file);

    // while (reader.readNextStartElement() || !reader.atEnd()) {
    //     if (reader.name() == "Photogroups") {
    //         qDebug() << "name " << reader.name() << " token " << reader.tokenString();
    //     }     
    // }
    // return CODE_FILE_LOAD_FAILED_ERROR;
	int i = 0;
    while (reader.readNextStartElement() || !reader.atEnd()) {
//        qDebug() << "name " << reader.name();
//		qDebug() << "lineNUmber " << reader.lineNumber();
// 		i++;
// 		cout << i << endl;
        if (reader.name() == "Block") {
            while (reader.readNextStartElement()) {
                if (reader.name() == "Name") {
                    m_blockExchange->block.name = reader.readElementText();
                }
                else if (reader.name() == "Description") {
                    m_blockExchange->block.description = reader.readElementText();
                }
                else if (reader.name() == "SRSId") {
                    m_blockExchange->block.SRSId = reader.readElementText().toInt();

                }
                else if (reader.name() == "Photogroups") {
                    while (reader.readNextStartElement()) {
                        if (reader.name() == "Photogroup") {

                            BLOCK_TASK::Photogroup photogroupInfo;
                            while (reader.readNextStartElement()) {
//                                 QString tokenName = reader.name().toString();
//                                 QString token = reader.tokenString();
// //                                qDebug() << "name " << reader.name() << " type " << reader.tokenString();
//                                 if (tokenName == "Photogroups" && token == "EndElement") {
//                                     break;
//                                 }
                              
                                if (reader.name() == "Name") {
                                    photogroupInfo.name = reader.readElementText();
                                }
                                else if (reader.name() == "CameraModelType") {
                                    photogroupInfo.cameraModelType = reader.readElementText();
                                }
                                else if (reader.name() == "CameraModelBand") {
                                    photogroupInfo.cameraModelBand = reader.readElementText();
                                }
                                else if (reader.name() == "FocalLength") {
                                    photogroupInfo.focalLength = reader.readElementText();
                                }
                                else if (reader.name() == "SensorSize") {
                                    photogroupInfo.sensorSize = reader.readElementText();
                                }
                                else if (reader.name() == "CameraOrientation") {
                                    photogroupInfo.cameraOrientation = reader.readElementText();
                                }
                                else if (reader.name() == "AspectRatio") {
                                    photogroupInfo.aspectRatio = reader.readElementText();
                                }
                                else if (reader.name() == "Skew") {
                                    photogroupInfo.skew = reader.readElementText();
                                }
                                else if (reader.name() == "ImageDimensions") {
                                    while (reader.readNextStartElement()) {
                                        if (reader.name() == "Width") {
                                            photogroupInfo.imageDimensions.width = reader.readElementText();
                                        }
                                        else if (reader.name() == "Height") {
                                            photogroupInfo.imageDimensions.height = reader.readElementText();
                                        }
                                        else {
                                            reader.skipCurrentElement();
                                        }
                                    }
                                }
                                else if (reader.name() == "PrincipalPoint") {
                                    while (reader.readNextStartElement()) {
                                        if (reader.name() == "x") {
                                            photogroupInfo.principalPoint.x = reader.readElementText();
                                        }
                                        else if (reader.name() == "y") {
                                            photogroupInfo.principalPoint.y = reader.readElementText();
                                        }
                                        else {
                                            reader.skipCurrentElement();
                                        }                                       
                                    }
                                }
                                else if (reader.name() == "Distortion") {

                                    while (reader.readNextStartElement()) {
                                        if (reader.name() == "K1") {
                                            photogroupInfo.distortion.k1 = reader.readElementText();
                                        }
                                        else if (reader.name() == "K2") {
                                            photogroupInfo.distortion.k2 = reader.readElementText();
                                        }
                                        else if (reader.name() == "K3") {
                                            photogroupInfo.distortion.k3 = reader.readElementText();
                                        }
                                        else if (reader.name() == "P1") {
                                            photogroupInfo.distortion.p1 = reader.readElementText();
                                        }
                                        else if (reader.name() == "P2") {
                                            photogroupInfo.distortion.p1 = reader.readElementText();
                                        }
                                        else {
                                            reader.skipCurrentElement();
                                        }
                                    }
                                }
                                else if (reader.name() == "Photo") {
                                    BLOCK_TASK::Photo *photoInfo = new BLOCK_TASK::Photo;

                                    while (reader.readNextStartElement()) {

                                        // QString tokenName = reader.name().toString();
                                        // QString token = reader.tokenString();
                                        // qDebug() << "name " << reader.name() << " type " << reader.tokenString();
                                        // if (tokenName == "Photo" && token == "EndElement") {    
                                        //     break;
                                        // }
//                                        qDebug() << "name " << reader.name();

                                        if (reader.name() == "Id") {
                                            photoInfo->id = reader.readElementText().toInt();
                                        }
                                        else if (reader.name() == "ImagePath") {
                                            photoInfo->imagePath = reader.readElementText();
                                        }
                                        else if (reader.name() == "Component") {
                                            photoInfo->component = reader.readElementText();
                                        }
                                        else if (reader.name() == "NearDepth") {
                                            photoInfo->nearDepth = reader.readElementText();
                                        }
                                        else if (reader.name() == "MedianDepth") {
                                            photoInfo->medianDepth = reader.readElementText();
                                        }
                                        else if (reader.name() == "FarDepth") {
                                            photoInfo->farDepth = reader.readElementText();
                                        }
                                        else if (reader.name() == "Pose") {

                                            while (reader.readNextStartElement()) {

                                                if (reader.name() == "Rotation") {
                                                    while (reader.readNextStartElement()) {

                                                        if (reader.name() == "M_00") {
                                                            photoInfo->pose.rotation.M_00 = reader.readElementText();
                                                        }
                                                        else if (reader.name() == "M_01") {
                                                            photoInfo->pose.rotation.M_01 = reader.readElementText();
                                                        }
                                                        else if (reader.name() == "M_02") {
                                                            photoInfo->pose.rotation.M_02 = reader.readElementText();
                                                        }
                                                        else if (reader.name() == "M_10") {
                                                            photoInfo->pose.rotation.M_10 = reader.readElementText();
                                                        }
                                                        else if (reader.name() == "M_11") {
                                                            photoInfo->pose.rotation.M_11 = reader.readElementText();
                                                        }
                                                        else if (reader.name() == "M_12") {
                                                            photoInfo->pose.rotation.M_12 = reader.readElementText();
                                                        }
                                                        else if (reader.name() == "M_20") {
                                                            photoInfo->pose.rotation.M_20 = reader.readElementText();
                                                        }
                                                        else if (reader.name() == "M_21") {
                                                            photoInfo->pose.rotation.M_21 = reader.readElementText();
                                                        }
                                                        else if (reader.name() == "M_22") {
                                                            photoInfo->pose.rotation.M_22 = reader.readElementText();
                                                        }
														else if (reader.name() == "Omega") {
															photoInfo->pose.rotation.Omega = reader.readElementText();
														}
														else if (reader.name() == "Phi") {
															photoInfo->pose.rotation.Phi = reader.readElementText();
														}
														else if (reader.name() == "Kappa") {
															photoInfo->pose.rotation.Kappa = reader.readElementText();
														}
														else if (reader.name() == "Yaw") {
															photoInfo->pose.rotation.Yaw = reader.readElementText();
														}
														else if (reader.name() == "Pitch") {
															photoInfo->pose.rotation.Pitch = reader.readElementText();
														}
														else if (reader.name() == "Roll") {
															photoInfo->pose.rotation.Roll = reader.readElementText();
														}
                                                        else {
                                                            reader.skipCurrentElement();
                                                        }
                                                    }
                                                }
                                                else if (reader.name() == "Center") {

                                                    while (reader.readNextStartElement()) {
                                                        if (reader.name() == "x") {
                                                            photoInfo->pose.center.x = reader.readElementText();
                                                        }
                                                        else if (reader.name() == "y") {
                                                            photoInfo->pose.center.y = reader.readElementText();
                                                        }
                                                        else if (reader.name() == "z") {
                                                            photoInfo->pose.center.z = reader.readElementText();
                                                        }
                                                        else {
                                                            reader.skipCurrentElement();
                                                        }
                                                    }
                                                }
                                                else if (reader.name() == "Metadata") {

                                                    while (reader.readNextStartElement()) {

                                                        if (reader.name() == "SRSId") {
                                                            photoInfo->pose.metaData.srsId = reader.readElementText().toInt();

                                                        }
                                                        else if (reader.name() == "Center") {
                                                            while (reader.readNextStartElement()) {

                                                                if (reader.name() == "x") {
                                                                    photoInfo->pose.metaData.center.x = reader.readElementText();
                                                                }
                                                                else if (reader.name() == "y") {
                                                                    photoInfo->pose.metaData.center.y = reader.readElementText();
                                                                }
                                                                else if (reader.name() == "z") {
                                                                    photoInfo->pose.metaData.center.z = reader.readElementText();

                                                                }
                                                                else {
                                                                    reader.skipCurrentElement();
                                                                }
                                                            }
                                                        }
														else {
															reader.skipCurrentElement();
														}
                                                    }
                                                }
                                                else {
                                                    reader.skipCurrentElement();
                                                }
                                            }
                                        }
                                        else if (reader.name() == "ExifData") {

                                            while (reader.readNextStartElement()) {
                                                if (reader.name() == "FocalLength") {
                                                    photoInfo->exifData.focalLength = reader.readElementText();
                                                }
                                                else if (reader.name() == "FocalLength35mmEq") {
                                                    photoInfo->exifData.focalLength35mmEq = reader.readElementText();
                                                }
                                                else if (reader.name() == "Make") {
                                                    photoInfo->exifData.make = reader.readElementText();
                                                }
                                                else if (reader.name() == "Model") {
                                                    photoInfo->exifData.model = reader.readElementText();
                                                }
                                                else if (reader.name() == "LensModel") {
                                                    photoInfo->exifData.lensModel = reader.readElementText();
                                                }
                                                else if (reader.name() == "DateTimeOriginal") {
                                                    photoInfo->exifData.dateTimeOriginal = reader.readElementText();
                                                }
                                                else if (reader.name() == "YawPitchRoll") {													
													bool datatype = true;
													while (reader.readNextStartElement()) {
														datatype = false;
														if (reader.name() == "x") {
															photoInfo->exifData.yawPitchRoll += reader.readElementText();
															photoInfo->exifData.yawPitchRoll += QString(" ");
														}
														else if (reader.name() == "y") {
															photoInfo->exifData.yawPitchRoll += reader.readElementText();
															photoInfo->exifData.yawPitchRoll += QString(" ");
														}
														else if (reader.name() == "z") {
															photoInfo->exifData.yawPitchRoll += reader.readElementText();
														}
														else {
															reader.skipCurrentElement();
														}
													}
													if (datatype)
													{
														photoInfo->exifData.yawPitchRoll = reader.readElementText();
													}
                                                }
                                                else if (reader.name() == "GPS") {

                                                    while (reader.readNextStartElement()) {
                                                        if (reader.name() == "Latitude") {
                                                            photoInfo->exifData.gps.Latitude = reader.readElementText();
                                                        }
                                                        else if (reader.name() == "Longitude") {
                                                            photoInfo->exifData.gps.Longitude = reader.readElementText();
                                                        }
                                                        else if (reader.name() == "Altitude") {
                                                            photoInfo->exifData.gps.Altitude = reader.readElementText();
                                                        }
                                                        else {
                                                            reader.skipCurrentElement();
                                                        }
                                                    }
                                                }
                                                else {
                                                   reader.skipCurrentElement();
                                                }
                                            }
                                        }
										else {
											reader.skipCurrentElement();
										}
                                    }
                                    if (photoInfo->id >= 0) {
                                        photogroupInfo.photos.append(photoInfo);
                                    }
                                }             
								else {
									reader.skipCurrentElement();
								}
                            }
                            qDebug() << "name " << reader.name() << " type " << reader.tokenString();
                            m_blockExchange->block.photogroups.append(photogroupInfo);
                        }
                        else {
                            reader.skipCurrentElement();
                        }
                    }
                }
                else if (reader.name() == "ControlPoints") {
                    while (reader.readNextStartElement()) {

//                         QString tokenName = reader.name().toString();
//                         QString token = reader.tokenString();
//                         // qDebug() << "name " << reader.name() << " type " << reader.tokenString();
//                         if (tokenName == "ControlPoints" && token == "EndElement") {
//                             break;
//                         }
                        if (reader.name() == "ControlPoint") {
                            BLOCK_TASK::ControlPoint* ctrlPoint = new BLOCK_TASK::ControlPoint;

                            while (reader.readNextStartElement()) {

                                if (reader.name() == "SRSId") {
                                    ctrlPoint->SRSId = reader.readElementText().toInt();
                                }
                                else if (reader.name() == "Name") {
                                    ctrlPoint->name = reader.readElementText();
                                }
                                else if (reader.name() == "Category") {
                                    ctrlPoint->category = reader.readElementText();
                                }
                                else if (reader.name() == "HorizontalAccuracy") {
                                    ctrlPoint->horizontalAccuracy = reader.readElementText();
                                }
                                else if (reader.name() == "VerticalAccuracy") {
                                    ctrlPoint->verticalAccuracy = reader.readElementText();
                                }
                                else if (reader.name() == "CheckPoint") {
                                    ctrlPoint->checkPoint = reader.readElementText() == "true" ? true : false;
                                }
                                else if (reader.name() == "Position") {

                                    while (reader.readNextStartElement()) {

                                        if (reader.name() == "x") {
                                            ctrlPoint->position.x = reader.readElementText();
                                        }
                                        else if (reader.name() == "y") {
                                            ctrlPoint->position.y = reader.readElementText();
                                        }
                                        else if (reader.name() == "z") {
                                            ctrlPoint->position.z = reader.readElementText();
                                        }
                                        else {
                                            //reader.skipCurrentElement();
                                        }
                                    }
                                }
                                else {
                                   reader.skipCurrentElement();
                                }
                            }
                            m_blockExchange->block.controlPoints.append(ctrlPoint);
                        }
                        else {
                           reader.skipCurrentElement();
                        }
                    }
                }
                else {
                    reader.skipCurrentElement();
                }
            }
        }
    }
  
    file.close();
    return CODE_SUCCESS;
}
bool LoadBlockFileThread::checkTiePoints()
{
	// 如果文件是xmlz,则解压后再解析
	QString xmlPath = m_filePath;
	QFileInfo info(m_filePath);
	if (info.suffix() == "xmlz") {
		QString unzipDir = unzipXmlFileTempPath();
		if (!unzipXmlzFile(m_filePath, unzipDir)) {
			return CODE_FILE_LOAD_FAILED_ERROR;
		}
		xmlPath = QString("%1/%2.xml").arg(unzipDir).arg(info.baseName());
	}

	QFile file(xmlPath);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		return CODE_FILE_LOAD_FAILED_ERROR;
	}
	QXmlStreamReader reader;
	reader.setDevice(&file);

	while (reader.readNextStartElement() || !reader.atEnd()) {
		
		if (reader.name() == "TiePoint")
		{
			m_tilePoints = true;
//			qDebug() << "name " << reader.name() << " token " << reader.tokenString() << reader.lineNumber();
			break;
		}
	}
	file.close();
	return m_tilePoints;
}