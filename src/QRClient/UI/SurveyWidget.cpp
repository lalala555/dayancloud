#include "stdafx.h"
#include "SurveyWidget.h"
#include <QScrollBar>
#include <fstream>
#include <sstream>
#include <vector>
#include <QMetaObject> 
#include <QFileInfo>
#include <QString> 
#include <QDebug>
#include <QTreeWidgetItem>
#include <iostream>
#include <atomic>
#include <opencv2\opencv.hpp>
#include <opencv2\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\core\utility.hpp>
#include <QtConcurrent>
#include "HttpCommand/HttpCmdManager.h"
#include "HttpCommand\HttpCmd\HttpOperatorTaskFrame.h"
#include "NewJobs/JobStateMgr.h"
#include <math.h>
#include "KrGcp.h"


#include <cstdlib> 
using namespace cv;
#pragma push_macro("slots")  //解决Python 和 Qt 的关键词 slots 冲突
#undef slots
#include <Python.h>
#pragma pop_macro("slots")

// #include <windows.h>
// #include <gdiplus.h>
// #include <strsafe.h>
// #include <Propkey.h>
// #include <comutil.h>
// #include <propvarutil.h>
// #pragma comment(lib, "comsuppw.lib")
// 
// using namespace Gdiplus;
// #pragma comment(lib, "gdiplus.lib")


SurveyWidget::SurveyWidget(QWidget *parent)
	: QWidget(parent),
	m_initListWidget(false),
	m_krGcp(nullptr),
	m_currentSelectedBtnIndex(enIndexAll),
	m_blockExchange(nullptr),
	m_blockMergeStateSubmit(false)
{
    ui.setupUi(this);
	initUI();
	initSignalSlot();
#ifdef FOXRENDERFARM
	ui.btnAdd->setFixedWidth(180);
	ui.btnDelete->setFixedWidth(180);
#endif 
	
}
SurveyWidget::~SurveyWidget()
{
	// delete m_blockExchange;
}

bool SurveyWidget::initPythonCall()
{
	// m_krGcp 内存记得释放
	m_krGcp = new KrGcp(
		m_imagePos_path, 
		m_control_point,
		m_compute_line_input,
		m_sift_photo_output, 
		m_compute_line_output,
		m_update_photos_input,
		m_update_photos_output,
		m_license_url,m_license_code,
		m_validate_return_output,
		m_update_photos_tp_input,
		m_update_photos_tp_output);

	if (m_krGcp->getObjectGCP() == nullptr)
	{
		LOGFMTI("SurveyWidget::initPythonCall()  ObjectGCP is NULL");
		return false;
	}
	m_krGcp->verifylicense();
	m_krGcp->siftPhotos();
	return true;

}

KrGcp * SurveyWidget::getKrGcp()
{
	return m_krGcp;
}

void SurveyWidget::deleteKrGcp()
{
	delete m_krGcp;
	m_krGcp = nullptr;
}

void SurveyWidget::setRootDir(const QString& _dirPath)
{
	m_rootDir = _dirPath;
}

void SurveyWidget::run()
{
	emit initSurveyWidgetBegin();
}
void SurveyWidget::loadImageData()
{
	cleanUI();
    QtConcurrent::run([this]() {
		outPutControlJson();
		if (!initPythonCall())
		{
			emit createthumbnailFinished(CODE_INIT_PYTHON_CALL_FAILED);
			return;
		}
        getImages(m_imagePath, m_images);
        int code = createthumbnail();
		CreateLocalToXML();
		updataThumimageMap();
		ParsesiftPhotoOutputJson();
		QMetaObject::invokeMethod(this, "initFirst");

		loadXmlFileToCP();
        emit createthumbnailFinished(code);
    });
}



void SurveyWidget::setImagePosPath(const QString &_filePath)
{
	m_imagePos_path = _filePath;
}

void SurveyWidget::setSiftPhotoInput(const QString &_filePath)
{
	m_control_point = _filePath;
}
void SurveyWidget::setComputeLineInput(const QString &_filePath)
{
	m_compute_line_input = _filePath;
}
void SurveyWidget::setSiftPhotoOutput(const QString &_filePath)
{
	m_sift_photo_output = _filePath;
}
void SurveyWidget::setComputeLineOutput(const QString &_filePath)
{
	m_compute_line_output = _filePath;
}

void SurveyWidget::setAtTaskID(const t_taskid &_taskID)
{
	m_at_taskID = _taskID;
}

void SurveyWidget::initCalcParam(const at_para_calc::at_param_json &_param)
{
	m_calc_param = _param;
}

void SurveyWidget::outPutControlJson()
{
	QString outdir = m_rootDir + "/input";
	QDir().mkpath(outdir);

	QString out_file_path = outdir + "/sift_photo_input.json";
	QFile file(out_file_path);
	if (!file.open(QIODevice::WriteOnly))
	{
		return;
	}
	QJsonObject json;
	QJsonArray earr;
	for (auto & e : m_surveysInfo.ControlPoints)
	{
		QJsonObject eobj;
		eobj["SRS"] = QString::number(e.srs_id);
		eobj["name"] = e.control_name;
		eobj["x"] = e.coordinate.x;
		eobj["y"] = e.coordinate.y;
		eobj["z"] = e.coordinate.z;
		earr.push_back(eobj);
	}
	json["e"] = earr;	
	QByteArray data = QJsonDocument(json).toJson();
	file.write(data);
	file.close();
	m_control_point = out_file_path;

	createJsonFile();
}

void SurveyWidget::createJsonFile()
{
	QString indir = m_rootDir + "/input";
	QDir().mkpath(indir);

	m_compute_line_input = indir + "/compute_line_input.json";
	m_update_photos_input = indir + "/update_photos_input.json";
	m_update_photos_tp_input = indir + "/update_photos_tp_input.json";


	QString outdir = m_rootDir + "/output";
	QDir().mkpath(outdir);

	m_sift_photo_output = outdir + "/sift_photo_output.json";
	m_compute_line_output = outdir + "/compute_line_output.json";
	m_update_photos_output = outdir + "/update_photos_output.json";
	m_update_photos_tp_output = outdir + "/update_photos_tp_output.json";

	m_surveyPointXml_path = outdir + "/survey.xml";
	m_validate_return_output = outdir + "/validate_return_value.json";
	m_update_photos_tp_output = outdir + "/update_photos_tp_output.json";
}

void SurveyWidget::ParsesiftPhotoOutputJson()
{
	QFile file(m_sift_photo_output);
	if (!file.open(QIODevice::ReadOnly))
		return;
	auto data = file.readAll();
	QJsonDocument doc = QJsonDocument::fromJson(data);
	QJsonObject obj = doc.object();
	auto keys = obj.keys();
	for (auto &k : keys)
	{
		QJsonArray imgsInfo = obj[k].toArray();

		for (auto &p : m_surveysInfo.ControlPoints)
		{
			if (p.control_name == k)
			{
				for (auto &info : imgsInfo)
				{
					QStringList  _keys = info.toObject().keys();  // mingzi

//					QStringList key_name = _keys.
					QJsonObject ic = info.toObject();
					for (auto &_k : _keys)
					{
						SURVEYS_INFO::imageMeasurement temp;
						QJsonObject icon = ic[_k].toObject();
						
						temp.image_id = 0;     //计算潜在匹配输出无ID;

//						temp.image_path = _k;  // 此处替换为对应的缩略图路径，潜在匹配列表显示缩略图

						auto X2L = m_map_XML2localthumimg.find(_k.toStdString());
						if (X2L == m_map_XML2localthumimg.end()) {
							continue;
						}
						std::string imagexml2LocalImg = X2L->second; //通过xml中的影像路径找到实际缩略图对应的路径
						
//                      auto itmap = m_map_imageThum.find(_k.toStdString());
//                      if (itmap == m_map_imageThum.end()) {
//                           continue;
//                      }
// 						temp.image_path = QString::fromLocal8Bit(itmap->second.c_str());
						temp.image_path = QString::fromLocal8Bit(imagexml2LocalImg.c_str());
						temp.pix_x = icon["u"].toDouble();
						temp.pix_y = icon["v"].toDouble();
					
						p.potential_match.push_back(temp);
					}
				}					
				break;
			}
		}

	}

}

bool SurveyWidget::UpdatePhotoOutputJson()
{
	QFile file(m_update_photos_output);
	if (!file.open(QIODevice::ReadOnly))
		return false;
	auto data = file.readAll();
	QJsonDocument doc = QJsonDocument::fromJson(data);
	QJsonObject obj = doc.object();
	auto keys = obj.keys();
	if ( keys.size() < 3)  //控制点数据至少需要3个
	{
		return false;
	}

	//清除旧的潜在匹配项数据
	for (auto &cp : m_surveysInfo.ControlPoints)
	{
		cp.potential_match.clear();
	}

	for (auto &k : keys)
	{
		QJsonArray imgsInfo = obj[k].toArray();

		for (auto &p : m_surveysInfo.ControlPoints)
		{
			if (p.control_name == k)
			{
				for (auto &info : imgsInfo)
				{
					auto  _keys = info.toObject().keys();  // mingzi
					QJsonObject ic = info.toObject();
					for (auto &_k : _keys)
					{
						SURVEYS_INFO::imageMeasurement temp;
						QJsonObject icon = ic[_k].toObject();

						temp.image_id = 0;     //计算潜在匹配输出无ID;	后续增加					

						auto itmap = m_map_imageThum.find(_k.toStdString());
						if (itmap == m_map_imageThum.end()) {
							continue;
						}
						temp.image_path = QString::fromLocal8Bit(itmap->second.c_str());
						temp.pix_x = icon["u"].toDouble();
						temp.pix_y = icon["v"].toDouble();

						p.potential_match.push_back(temp);
					}
				}
				break;
			}
		}

	}

	return true;
}

void SurveyWidget::setReJson(const QString& _filePath)
{
	m_re_json = _filePath;
	//解析json文件中对应的潜在匹配项
	QFile file(_filePath);
	if (!file.open(QIODevice::ReadOnly))
		return;
	auto data = file.readAll();
	QJsonDocument doc = QJsonDocument::fromJson(data);
	QJsonObject obj = doc.object();
	if (obj["siftPhotos"].isObject())
	{
		auto siftPhotosObj = obj["siftPhotos"].toObject();
		auto keys = siftPhotosObj.keys();

		for (auto &k : keys)
		{
			QStringList img_paths = siftPhotosObj[k].toObject().keys();
			for (auto &p : m_surveysInfo.ControlPoints)
			{
				if (p.control_name == k)
				{
					for (auto &path : img_paths)
//						p.potential_match.push_back(path);
					break;
				}
			}
		}
	}
}


void SurveyWidget::setControlPint(const SURVEYS_INFO::surveys_data& ctrlPointInfo)
{
	m_surveysInfo = ctrlPointInfo;
}

void SurveyWidget::setImagePath(const QString& imagePath)
{
	m_imagePath = imagePath;
}

void SurveyWidget::setBlockInfo( BLOCK_TASK::BlockExchange* blockExchange)
{
	m_blockExchange = blockExchange;
}

void SurveyWidget::setThumimagePath(const QString &_dirPath)
{
	m_thumbnail_path = _dirPath;
}

SURVEYS_INFO::surveys_data  SurveyWidget::getSurveysData()
{
	return m_surveysInfo;
}

void SurveyWidget::getImages(const QString &dirpath, std::list<std::string> & imgs)
{
//	QString path = QString::fromLocal8Bit(dirpath.c_str());
#if 0
	QDir dir(dirpath);
	QFileInfoList fileList = dir.entryInfoList(QStringList() << "*.JPG");
	for (auto fileInfo:fileList)
	{
		imgs.push_back(fileInfo.absoluteFilePath().toStdString());
	}
#endif
	QStringList pathList; pathList << dirpath;

	foreach(QString path, pathList)
	{
		QFileInfo info(path);
		if (info.isDir()) {
			QMap<QString, QStringList> picPaths;
			if (findFiles(path, picPaths)) {
				QMap<QString, QStringList>::Iterator itr;
				for (itr = picPaths.begin(); itr != picPaths.end(); itr++)
				{
					QStringList currimgs = itr.value();
					for (int i = 0; i < currimgs.size(); i++)
					{
						imgs.push_back(currimgs[i].toStdString());
					}
				}
			}
		}
	}
}

int SurveyWidget::createthumbnail()
{
	LOGFMTI("SurveyWidget::createthumbnail() ,begin");

	std::mutex mtx;
	std::atomic_int img_count = m_images.size();

	for (auto image : m_images)
	{
#if 0  //图片质量压缩 分辨率不改变
		Mat src = imread(image);
		std::vector<int>compression_params;
		compression_params.push_back(IMWRITE_JPEG_QUALITY);
		compression_params.push_back(5);
		std::string temp_path = image;
		int temp_pos = temp_path.rfind("/");
		std::string ys_path = "E:\\test_image_ys\\";
		std::string result_path = ys_path + temp_path.substr(temp_pos + 1);		
		bool bRet = imwrite(result_path, src, compression_params);
		if (bRet)
		{
			m_thumbnail.push_back(result_path);
			m_map_thumimage.insert(std::make_pair(result_path, temp_path));
		}
#endif

		QtConcurrent::run([=, &mtx, &img_count] {
#if 1  //图片尺寸压缩
			std::string temp_path = image;
			//		int temp_pos = temp_path.rfind("/");
			int temp_pos = temp_path.find_first_of("/"); //全路径创建影像缩略图

														 //		std::string result_path = m_thumbnail_path.toStdString() + "/" + temp_path.substr(temp_pos + 1);

			string temp_path_qian = m_thumbnail_path.toLocal8Bit() + "/";
			string temp_path_hou = temp_path.substr(temp_pos + 1);

			std::string result_path = temp_path_qian + temp_path_hou;


			int ys_dir_pos = result_path.rfind("/");

			std::string ys_path = result_path.substr(0, ys_dir_pos);
			QDir().mkpath(QString::fromLocal8Bit(ys_path.c_str()));

			//		LOGFMTI("SurveyWidget::createthumbnail(), ys_path=%s", ys_path.c_str());

			std::vector<int>compression_params;
			compression_params.push_back(IMWRITE_JPEG_QUALITY);
			compression_params.push_back(60);

			std::ifstream ifs(result_path.c_str());
			if (!ifs.is_open())
			{
				cv::Mat image = cv::imread(temp_path);
				cv::Mat image_ys = cv::imread(result_path);
				cv::Mat image2X_INTER_NEAREST;
				cv::resize(image, image_ys, Size(), 0.03, 0.03, INTER_NEAREST);

				bool bRet = cv::imwrite(result_path, image_ys, compression_params);
				if (!bRet)
				{
					qDebug() << "thumbnail image creat fail";
				}
			}

			std::lock_guard<std::mutex> locker(mtx);

			m_thumbnail.push_back(result_path);
			m_map_thumimage.insert(std::make_pair(result_path, temp_path));
			// 获取文件名，用于后面进行匹配
			// std::string temp_name = QFileInfo(QString::fromStdString(temp_path)).fileName().toStdString();
			m_map_imageThum.insert(std::make_pair(temp_path, result_path));

			--img_count;
#endif
		});
	}

	while (img_count != 0)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	if (m_thumbnail.size() == m_images.size())
	{
		LOGFMTI("SurveyWidget::createthumbnail() ,end");
		return CODE_SUCCESS;
	}
	LOGFMTI("SurveyWidget::createthumbnail() ,fasle end");
	return CODE_CREATE_THUMBNAIL_FAILED;
}

void SurveyWidget::setImages(const std::vector<std::string> & imgs)
{
	// 将图片读取放到界面上
	for (auto &img : imgs)
	{
		QListWidgetItem *imageItem = new QListWidgetItem;
		imageItem->setData(Qt::UserRole, QString(img.c_str()));
		
		// TODO: 缩略图
		imageItem->setIcon(QIcon(img.c_str()));

		imageItem->setSizeHint(QSize(100, 80));
		ui.imgListWidget->addItem(imageItem);
	}
}

void SurveyWidget::initFirst()
{
	initFirstImage();
	showImageList(m_thumbnail);
	showControlPointList();
}

void SurveyWidget::selectedImageChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (current == nullptr) 
        return;
	std::vector<std::vector<QPoint>> emptyLines;
	ui.viewer_Image->setLines(emptyLines);	
	
	QString imgthumFile = current->data(Qt::UserRole).toString();
	

	if (m_map_thumimage.find(imgthumFile.toStdString())== m_map_thumimage.end())
	{
		return;
	}
	QString imgFile = QString::fromLocal8Bit(m_map_thumimage[imgthumFile.toStdString()].c_str());
	QImage img;	
	LOGFMTI("[SurveyWidget] selectedImageChanged thumImg  %s", imgthumFile.toStdString().c_str());
	LOGFMTI("[SurveyWidget] selectedImageChanged imgFile  %s", imgFile.toStdString().c_str());
	if (img.load(imgFile))
	{		
		ui.viewer_Image->showImage(img);
		ui.imageNameLabel->setText(QFileInfo(imgFile).fileName());
		ui.imageSizeLabel->setText(QString("%1x%2").arg(img.width()).arg(img.height()));
	}

	//此处添加核线计算显示并 ，计算条件 必须要选中对应的控制点（控制点对应的刺点数据 >=1 ）
	//1. 选择缩略图
	//2. 将对应控制点数据及 显示核线影像 先输出 compute_line_input.json 文件
	//3. 调用pyd  computeLine 
	//4. 解析步骤3 输出的 compute_line_output文件 
	//5. 将步骤4 的结果显示在计算后 ui.viewer_Image->setLines(vector<QLine>) 上

	QTreeWidgetItem * pointItem = ui.controlPointListWidget->currentItem();

	if (pointItem == nullptr) return;

	if (pointItem->type() == enTypeGCP ||(pointItem->parent() != nullptr && pointItem->parent()->type() == enTypeGCP))
	{
		if (pointItem != nullptr && pointItem->parent() == nullptr)
		{
			const int id = pointItem->data(0, Qt::UserRole).toInt();
			//找到对应控制点数据
			for (auto &p : m_surveysInfo.ControlPoints)
			{
				if (p.control_id == id)
				{
					calcEpipolarLineData(p, imgFile, imgthumFile);
					break;
				}
			}

		}
		else if (pointItem != nullptr && pointItem->parent() != nullptr && pointItem->parent()->parent() == nullptr)
		{
			const int id = pointItem->parent()->data(0, Qt::UserRole).toInt();
			//找到对应控制点数据
			for (auto &p : m_surveysInfo.ControlPoints)
			{
				if (p.control_id == id)
				{
					if (m_map_imageThum.find(imgFile.toStdString()) == m_map_imageThum.end())
					{
						return;
					}
					QString imgthumFile = QString::fromLocal8Bit(m_map_imageThum[imgFile.toStdString()].c_str());
					updateMeasurementPoints(imgthumFile, imgFile);
					calcEpipolarLineData(p, imgFile, imgthumFile);
					break;
				}
			}
		}
		updateMeasurementPoints(imgthumFile, imgFile);
	}
	else if (pointItem->type() == enTypeTP || (pointItem->parent() != nullptr && pointItem->parent()->type() == enTypeTP))
	{
		if (pointItem != nullptr && pointItem->parent() == nullptr)
		{
			const int id = pointItem->data(0, Qt::UserRole).toInt();
			//找到对应的连接点数据
			for (auto &p : m_surveysInfo.TiePoints.tie_points)
			{
				if (p.id == id)
				{
					calcEpipolarLineDataTP(p, imgFile, imgthumFile);
					break;
				}
			}

		}
		else if (pointItem != nullptr && pointItem->parent() != nullptr && pointItem->parent()->parent() == nullptr)
		{
			const int id = pointItem->parent()->data(0, Qt::UserRole).toInt();
			//找到对应连接点数据
			for (auto &p : m_surveysInfo.TiePoints.tie_points)
			{
				if (p.id == id)
				{
					if (m_map_imageThum.find(imgFile.toStdString()) == m_map_imageThum.end())
					{
						return;
					}
					QString imgthumFile = QString::fromLocal8Bit(m_map_imageThum[imgFile.toStdString()].c_str());
					updateMeasurementPointsTP(imgthumFile, imgFile);
					calcEpipolarLineDataTP(p, imgFile, imgthumFile);
					break;
				}
			}
		}
		updateMeasurementPointsTP(imgthumFile, imgFile);
	}
#if 0
	if (pointItem != nullptr && pointItem->parent() == nullptr)
	{
		const int id = pointItem->data(0, Qt::UserRole).toInt();
		//找到对应控制点数据
		for (auto &p : m_surveysInfo.ControlPoints)
		{
			if (p.control_id == id)
			{
				calcEpipolarLineData(p, imgFile, imgthumFile);
				break;
			}
		}

	}else if (pointItem != nullptr && pointItem->parent() != nullptr && pointItem->parent()->parent() == nullptr)
	{
		const int id = pointItem->parent()->data(0, Qt::UserRole).toInt();
		//找到对应控制点数据
		for (auto &p : m_surveysInfo.ControlPoints)
		{
			if (p.control_id == id)
			{
				if (m_map_imageThum.find(imgFile.toStdString()) == m_map_imageThum.end())
				{
					return;
				}
				QString imgthumFile = QString::fromLocal8Bit(m_map_imageThum[imgFile.toStdString()].c_str());
				updateMeasurementPoints(imgthumFile, imgFile);
				calcEpipolarLineData(p, imgFile, imgthumFile);
				break;
			}
		}
	}
	updateMeasurementPoints(imgthumFile, imgFile);
#endif
	
}

void SurveyWidget::onListWidgetValueChanged()
{
	if (ui.imgListWidget) {
		ui.imgListWidget->clear();
		m_initListWidget = false;
	}
    if (m_currentSelectedBtnIndex == enIndexAll) {
		if (m_thumbnail.size() < 1) return;
		LOGFMTI("[SurveyWidget] thumbnail clicked  imagenum   %d  first path %s ", m_thumbnail.size(), m_thumbnail[0].c_str());
        showImageList(m_thumbnail);
    }
    else if (m_currentSelectedBtnIndex == enIndexMatched) {
		if (m_matchThumbnail.size() < 1) return;
		LOGFMTI("[SurveyWidget] matchThumbnail clicked  imagenum   %d  first path %s ", m_matchThumbnail.size(), m_matchThumbnail[0].c_str());
        showImageList(m_matchThumbnail);
    }
    else if (m_currentSelectedBtnIndex == enIndexSurvey) {
		if(m_surveyThumbnail.size() < 1) return;
		LOGFMTI("[SurveyWidget] surveyThumbnail clicked  imagenum   %d  first path %s ", m_surveyThumbnail.size(), m_surveyThumbnail[0].c_str());
        showImageList(m_surveyThumbnail);
    }

}

void SurveyWidget::showImageList(const std::vector<std::string>& thumbnail)
{
//	ui.imgListWidget->clear();
	if (!m_initListWidget)
	{
		m_initListWidget = true;
		//初始化缩略图
		for (auto &img : thumbnail)
		{
			QListWidgetItem *imageItem = new QListWidgetItem;
			imageItem->setData(Qt::UserRole, QString(img.c_str()));

			// TODO: 缩略图
			imageItem->setIcon(QIcon(img.c_str()));

			imageItem->setSizeHint(QSize(100, 80));
			ui.imgListWidget->addItem(imageItem);
		}
	}

	QListWidgetItem * curritem = ui.imgListWidget->itemAt(50, 40);
	if (curritem == NULL)
	{
		return;
	}
	int index = ui.imgListWidget->row(curritem);
	int count = ui.imgListWidget->count();
	qDebug() << "row index:" << index;
	qDebug() << "count index:" << count;

#if 1
	if (thumbnail.size() < 80)
	{
		for (int i = 0; i < thumbnail.size(); ++i) {
			if (ui.imgListWidget->item(i) == nullptr)
			{
				return;
			}
//			ui.imgListWidget->item(i)->setData(Qt::UserRole, QString(m_thumbnail[i].c_str()));
			ui.imgListWidget->item(i)->setIcon(QIcon(thumbnail[i].c_str()));
		}
	}else 
	{
		//一次加载80个
		if (index >= 80) {
			for (int i = index - 79; i < index + 1; ++i) {
				if (ui.imgListWidget->item(i) == nullptr)
				{
					return;
				}
//				ui.imgListWidget->item(i)->setData(Qt::UserRole, QString(m_thumbnail[i].c_str()));
				ui.imgListWidget->item(i)->setIcon(QIcon(thumbnail[i].c_str()));
			}
		}
		else {
			for (int i = 0; i < 80; ++i) {
				if (ui.imgListWidget->item(i) == nullptr)
				{
					return;
				}
//				ui.imgListWidget->item(i)->setData(Qt::UserRole, QString(m_thumbnail[i].c_str()));
				ui.imgListWidget->item(i)->setIcon(QIcon(thumbnail[i].c_str()));
			}
		}
	}	
#endif
	
}

void SurveyWidget::onStabPoints(const QPointF & pos)
{
	QListWidgetItem * item = ui.imgListWidget->currentItem();
	if (item == nullptr)
		return;

	// 右边选择的像控点
	QTreeWidgetItem * pointItem = ui.controlPointListWidget->currentItem();
	if (pointItem && pointItem->parent())
		pointItem = pointItem->parent();
	if (pointItem != nullptr) // 如果为空说明是顶层的相控点
	{
		QString img_path = item->data(Qt::UserRole).toString();

		if (m_map_thumimage.find(img_path.toStdString()) == m_map_thumimage.end())
		{
			return;
		}

		std::string raw_path = m_map_thumimage[img_path.toStdString()];
		if (raw_path.empty())
			return;

		const int id = pointItem->data(0, Qt::UserRole).toInt();

		qDebug() << id;

		// 找到对应的相控点
		if (pointItem->type() == enTypeGCP)
		{
			for (auto &p : m_surveysInfo.ControlPoints)
			{
				if (p.control_id == id)
				{
					// 找到对应的图片，没有就新增
					auto iter = std::find_if(p.photos.begin(), p.photos.end(), [&](auto &iter) -> bool {
						return iter.image_path == raw_path.c_str();
					});

					if (iter != p.photos.end())
					{
						iter->pix_x = pos.x();
						iter->pix_y = pos.y();
					}
					else
					{
						if (m_map_Local2XML.find(img_path.toStdString()) == m_map_Local2XML.end())
						{
							return;
						}
						QString img_group = m_map_Local2XML[img_path.toStdString()].group;
						p.photos.push_back(SURVEYS_INFO::imageMeasurement{ -1, raw_path.c_str() ,pos.x(), pos.y(),img_group });

						auto new_tiem = new QTreeWidgetItem(QStringList() << QFileInfo(raw_path.c_str()).fileName());
						new_tiem->setData(0, Qt::UserRole, raw_path.c_str());
						pointItem->addChild(new_tiem);
					}

					//当前点的刺点信息保存为 update_photos_input.json				
					SaveCurrGcpInfoToJson(p);
					break;
				}
			}
		}
		else if (pointItem->type() == enTypeTP)
		{
			for (auto &p : m_surveysInfo.TiePoints.tie_points)
			{
				if (p.id == id)
				{
					// 找到对应的图片，没有就新增
					auto iter = std::find_if(p.photos.begin(), p.photos.end(), [&](auto &iter) -> bool {
						return iter.image_path == raw_path.c_str();
					});

					if (iter != p.photos.end())
					{
						iter->pix_x = pos.x();
						iter->pix_y = pos.y();
					}
					else
					{
						if (m_map_Local2XML.find(img_path.toStdString()) == m_map_Local2XML.end())
						{
							return;
						}
						QString img_group = m_map_Local2XML[img_path.toStdString()].group;
						p.photos.push_back(SURVEYS_INFO::imageMeasurement{ -1, raw_path.c_str() ,pos.x(), pos.y(),img_group });

						auto new_tiem = new QTreeWidgetItem(QStringList() << QFileInfo(raw_path.c_str()).fileName());
						new_tiem->setData(0, Qt::UserRole, raw_path.c_str());
						pointItem->addChild(new_tiem);
					}

					//当前点的刺点信息保存为 update_photos_tp_input.json				
					SaveCurrTpInfotoJson(p);
					m_krGcp->updataPhotosTP();
					//连接点信息更新&&连接点潜在匹配项解析
					UpdatePhotoTPOutputJson();
					break;
				}
			}
		}
	}		
	//m_surveysInfo 刺点信息保存为xml文件
	saveXmlToFile();

	//更新控制点潜在匹配项
	m_krGcp->updataPhotos();
	if (UpdatePhotoOutputJson())
	{
		emit ui.matchButton->clicked();
	}
}

void SurveyWidget::onTreeListItemClicked(QTreeWidgetItem * item, int)
{
	if (item->parent())
	{
		QString imgFile = item->data(0, Qt::UserRole).toString();
		QImage img;
		if (img.load(imgFile))
		{
			
			ui.imageNameLabel->setText(QFileInfo(imgFile).fileName());
			ui.imageSizeLabel->setText(QString("%1x%2").arg(img.width()).arg(img.height()));
		
			ui.viewer_Image->showImage(img);
			// TODO 找到已经刺的点
			//ui.viewer_Image->setControlPoints();
			QTreeWidgetItem * pointItem = ui.controlPointListWidget->currentItem();
			QTreeWidgetItem * point1 = pointItem->parent();
			QTreeWidgetItem * point2 = pointItem->parent()->parent();

			if (pointItem != nullptr && pointItem->parent() != nullptr && pointItem->parent()->parent() == nullptr)
			{
				const int id = pointItem->parent()->data(0, Qt::UserRole).toInt();

				const QString name = pointItem->parent()->data(0, Qt::UserRole).toString();

				if (pointItem->parent()->type() ==enTypeGCP)
				{
					//找到对应控制点数据
					for (auto &p : m_surveysInfo.ControlPoints)
					{
						if (p.control_id == id)
						{
							if (m_map_imageThum.find(imgFile.toStdString()) == m_map_imageThum.end())
							{
								return;
							}
							QString imgthumFile = QString::fromLocal8Bit(m_map_imageThum[imgFile.toStdString()].c_str());
							updateMeasurementPoints(imgthumFile, imgFile);
							calcEpipolarLineData(p, imgFile, imgthumFile);
							break;
						}
					}
				}
				else if (pointItem->parent()->type() == enTypeTP)
				{
					//找到对应连接点数据
					for (auto &p : m_surveysInfo.TiePoints.tie_points)
					{
						if (p.id == id)
						{
							if (m_map_imageThum.find(imgFile.toStdString()) == m_map_imageThum.end())
							{
								return;
							}
							QString imgthumFile = QString::fromLocal8Bit(m_map_imageThum[imgFile.toStdString()].c_str());
							updateMeasurementPointsTP(imgthumFile, imgFile);
							calcEpipolarLineDataTP(p, imgFile, imgthumFile);
							break;
						}
					}
				}


#if 0
				//找到对应控制点数据
				for (auto &p : m_surveysInfo.ControlPoints)
				{
					if (p.control_id == id)
					{
						if (m_map_imageThum.find(imgFile.toStdString()) == m_map_imageThum.end())
						{
							return;
						}
						QString imgthumFile = QString::fromLocal8Bit(m_map_imageThum[imgFile.toStdString()].c_str());
						updateMeasurementPoints(imgthumFile, imgFile);
						calcEpipolarLineData(p, imgFile, imgthumFile);
						break;
					}
				}
#endif

			}

		}
    }
    else 
    {
        on_matchButton_clicked();
    }
}

void SurveyWidget::initUI()
{
	cleanUI();
	ui.btnDelete->setEnabled(false);
	ui.imgListWidget->setViewMode(QListView::IconMode);
	ui.imgListWidget->setIconSize(QSize(100, 80));
	ui.imgListWidget->setResizeMode(QListWidget::Adjust);

	//控件屏蔽
#if 1
	ui.btnImport->hide();
	ui.btnEdit->hide();
	ui.tabWidget_survey->removeTab(1);

#endif
	
#ifdef FOXRENDERFARM
	ui.matchButton->setFixedWidth(140);
#endif
	//ui.imgListWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
}
void SurveyWidget::initSignalSlot()
{
	// 选择的图片改变了
	connect(ui.imgListWidget, &QListWidget::currentItemChanged, this, &SurveyWidget::selectedImageChanged);
	 
	// 动态加载缩略图
	connect(ui.imgListWidget->verticalScrollBar(), &QScrollBar::valueChanged, this, &SurveyWidget::onListWidgetValueChanged);

	// 刺点
	connect(ui.viewer_Image, SIGNAL(stabPoints(const QPointF &)), this, SLOT(onStabPoints(const QPointF &)));

	// 右侧选择图片
	connect(ui.controlPointListWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(onTreeListItemClicked(QTreeWidgetItem *, int)));
	
	//tp点选择
	connect(ui.controlPointListWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(setBtnDeleteEnable(QTreeWidgetItem *, int)));
	
	// 右键删除点
	connect(ui.controlPointListWidget, &QWidget::customContextMenuRequested, this, [=](const QPoint &pos) {
		auto item = ui.controlPointListWidget->itemAt(pos);
		if (item && item->parent())
		{
			// 弹菜单
			QMenu menu;
			auto delAction = menu.addAction(tr(u8"删除"));

			auto chooseAction = menu.exec(QCursor::pos());

			if (chooseAction == delAction)
			{
				// TODO 删除点和对应的item
				QString img_path = item->data(0, Qt::UserRole).toString();
				// 通过图片找到对应的点删除
				QString cur_gpcID =  item->parent()->data(0, Qt::UserRole).toString();
				// 删除对应控制点
				if (item->parent()->type() == enTypeGCP)
				{
					for (auto &p : m_surveysInfo.ControlPoints)
					{
						if (p.control_id == cur_gpcID.toShort())
						{
							for (auto &cp : p.photos)
							{
								if (img_path.compare(cp.image_path) == 0)
								{
									SURVEYS_INFO::imageMeasurement t = cp;
									p.photos.removeOne(t);
									item->parent()->removeChild(item);
									return;
								}
							}
						}
					}
				}
				//删除对应连接点
				else if (item->parent()->type() == enTypeTP)
				{
					for (auto &p : m_surveysInfo.TiePoints.tie_points)
					{
						if (p.id == cur_gpcID.toShort())
						{
							for (auto &tp : p.photos)
							{
								if (img_path.compare(tp.image_path) == 0)
								{
									SURVEYS_INFO::imageMeasurement t = tp;
									p.photos.removeOne(t);
									item->parent()->removeChild(item);
									return;
								}
							}
						}
					}
				}
			}
		}
		//ui.controlPointListWidget->clear();
		//showControlPointList();

	});
}

void SurveyWidget::cleanUI()
{
    m_initListWidget = false;
	m_images.clear();
	m_thumbnail.clear();
    m_map_imageThum.clear();
    m_map_thumimage.clear();
	m_map_Local2XML.clear();
	m_map_XML2localthumimg.clear();
	while (ui.controlPointListWidget->topLevelItemCount() > 0)
		delete ui.controlPointListWidget->takeTopLevelItem(0);
	ui.imgListWidget->clear();
//	ui.viewer_Image->PixmapItemDataClear();

	std::vector<std::vector<QPoint>> emptyLines;
	ui.viewer_Image->setLines(emptyLines);
	std::vector<ControlPointInfo> emptyGCP;
	ui.viewer_Image->setControlPoints(emptyGCP);
	QPointF emptyPoint;
	ui.viewer_Image->setCurrentStabPoint(emptyPoint);



}

void SurveyWidget::initFirstImage()
{
	//初始化原图显示
	if (m_thumbnail.size()<1)
	{
		return;
	}
	if (m_map_thumimage.find(m_thumbnail[0].c_str()) == m_map_thumimage.end())
	{
		return;
	}

	QString imgFile = QString::fromLocal8Bit(m_map_thumimage[m_thumbnail[0]].c_str());
	QImage img;
	img.load(imgFile);
	ui.viewer_Image->showImage(img, true);
	ui.imageNameLabel->setText(QFileInfo(imgFile).fileName());
	ui.imageSizeLabel->setText(QString("%1x%2").arg(img.width()).arg(img.height()));
}

void SurveyWidget::showControlPointList()
{	
	ui.controlPointListWidget->clear();
	//m_surveysInfo.ControlPoints
	for (auto & point : m_surveysInfo.ControlPoints)
	{
		QTreeWidgetItem * item = new QTreeWidgetItem(QStringList() << point.control_name,enTypeGCP);
		item->setData(0, Qt::UserRole, point.control_id);
		ui.controlPointListWidget->addTopLevelItem(item);

		for (auto &img : point.photos)
		{
			std::string s = img.image_path.toStdString();
			auto new_tiem = new QTreeWidgetItem(QStringList() << QFileInfo(s.c_str()).fileName());
			new_tiem->setData(0, Qt::UserRole, s.c_str());
			item->addChild(new_tiem);
		}
	}
	
	//m_surveysInfo.TiePoints
	for (auto & tpoint : m_surveysInfo.TiePoints.tie_points)
	{
		QTreeWidgetItem * item = new QTreeWidgetItem(QStringList() << tpoint.name, enTypeTP);
		item->setData(0, Qt::UserRole, tpoint.id);
		ui.controlPointListWidget->addTopLevelItem(item);

		for (auto &img : tpoint.photos)
		{
			std::string s = img.image_path.toStdString();
			auto new_tiem = new QTreeWidgetItem(QStringList() << QFileInfo(s.c_str()).fileName());
			new_tiem->setData(0, Qt::UserRole, s.c_str());
			item->addChild(new_tiem);
		}
	}

}

void SurveyWidget::wheelEvent(QWheelEvent *event)
{
	//int numDegrees = event->angleDelta().y();
	//qDebug() << "numDegrees" << numDegrees ;

	//event->accept(); //表示此处已经处理了鼠标的滚动事件，不需要传给父部件处理
}


//设置生成控制点surveyPoint XML文件导出路径
void SurveyWidget::setSurveyPointXMLPath(const QString &_dirPath)
{
	m_surveyPointXml_path = _dirPath;
}

//设置任务名
void SurveyWidget::setSenceName(const QString &_senceName)
{
	m_SenceName = _senceName;
}

void SurveyWidget::on_btnSubmitTask_clicked()
{
	m_blockLisFrames.clear();   //所有帧	
	m_failListFrames.clear();  //失败空三	
	m_mergeFrames.clear();   //合并帧

#ifdef FOXRENDERFARM
	if (Util::ShowMessageBoxQuesion(tr("The GCP has been stabbed, are you sure to upload?"), tr("Tips")) == QDialog::Rejected)
		return;
#else
	if (Util::ShowMessageBoxQuesion(tr("控制点已刺完,确定上传？"), tr("提示")) == QDialog::Rejected)
		return;
#endif


	
#if 0
	HttpCmdManager::getInstance()->taskSubmitATSurvey(m_at_taskID,m_surveyPointXml_path,m_SenceName , this);
#endif 
	HttpCmdManager::getInstance()->qureyTaskFrameRenderingInfo(m_at_taskID, 1, 50, this);
//	JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_at_taskID);
//
//	t_taskid at_parents_id = jobstate->getTaskParentId();
//
//	if (jobstate->getStatus() == JobState::enRenderTaskFinished) {
//		//JobState::DetailInfo  info = JobState::DetailInfo;
//		HttpCmdManager::getInstance()->uploadTaskSurveyFile(at_parents_id, m_surveyPointXml_path, this);
//	}
//	else
//	{
//#ifdef FOXRENDERFARM
//		Util::ShowMessageBoxError(QObject::tr("After the AT is successful, upload the control point data again！(ID:%1 Status:%2)").arg(m_at_taskID).arg(jobstate->getStatus()));
//#else
//		Util::ShowMessageBoxError(QObject::tr("初始空三失败，请完成初次空三，成功后，在进行控制点刺点重提 ！(ID:%1 Status:%2)").arg(m_at_taskID).arg(jobstate->getStatus()));
//#endif		
//		return;
//	}	
//
}

void SurveyWidget::CreateLocalToXML()
{
	//读入所有内容
	QFile file(m_imagePos_path);
	file.open(QIODevice::ReadOnly);
	auto data = file.readAll();
	file.close();
	//解析
	QJsonParseError parseError;
	QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

	if (parseError.error != QJsonParseError::NoError)
	{
		LOGFMTI("[SurveyWidget] CreateLocalToXML  TFailed to read image_pos.json file!");
		return;
	}
	// 此处后续还需要添加本地影像与xml影像的基本数据验证，xml中影像是否在本地数据中都能对应

	QJsonObject obj = doc.object();
	auto keys = obj.keys();
	for (auto &k : keys)
	{
		QString imagegroup = k;
		QJsonObject group = obj[k].toObject();
		QString width = group["Width"].toString();
		QString height = group["Height"].toString();

		auto photos = group["photos"].toObject();
		QStringList img_paths = photos.keys();
		for (auto &image : img_paths)
		{	

			for (auto& thumImg : m_thumbnail)
			{
				// rootThumImageDir 缩略图指定标识根目录 
				int temp_pos = thumImg.rfind("rootThumImageDir");
				string rootdir = "rootThumImageDir";
				string tempImg = thumImg.substr(temp_pos + rootdir.size());
				

				if (image.toStdString().rfind(tempImg) != string::npos)
				{
					ImageGroupInfo tempGI;
					tempGI.group = imagegroup;
					tempGI.imagpath = image;
					tempGI.height = height.toUShort();
					tempGI.width = width.toUShort();

					m_map_Local2XML.insert(std::make_pair(thumImg, tempGI));
					m_map_XML2localthumimg.insert(std::make_pair(tempGI.imagpath.toStdString(), thumImg));

					break;
				}
				
			}			
		}

		if (m_map_XML2localthumimg.size() < 1)
		{
			//提示本地影像路径除根目录的外，根目录二级路径甚至更多路径的文件夹层级名字发生改变
			LOGFMTI("[SurveyWidget] CreateLocalToXML  The name of the path hierarchy other than the original data root path has changed!");
			exit(-1);
		}
	}

}


void SurveyWidget::on_allImgButton_clicked()
{
	ui.matchButton->setChecked(false);
	ui.selectedButton->setChecked(false);

	LOGFMTI("[SurveyWidget] thumbnail clicked  imagenum   %d  first path %s ", m_thumbnail.size(), m_thumbnail[0].c_str());
    m_currentSelectedBtnIndex = enIndexAll;
	//清空当前列表
	ui.imgListWidget->clear();
	m_initListWidget = false;
	showImageList(m_thumbnail);
}

void SurveyWidget::on_matchButton_clicked()
{
	ui.allImgButton->setChecked(false);
	ui.selectedButton->setChecked(false);

    m_currentSelectedBtnIndex = enIndexMatched;
	//清空当前列表
	ui.imgListWidget->clear();

	//如果有对控制点进行选取，显示对应控制点的潜在匹配项
	QTreeWidgetItem * pointItem = ui.controlPointListWidget->currentItem();

    m_initListWidget = false;
    m_matchThumbnail.clear();


	if (pointItem != nullptr && pointItem->parent() == nullptr) // 如果为空说明是顶层的点
	{
		const int id = pointItem->data(0, Qt::UserRole).toInt();	

		if (pointItem->type() == enTypeGCP)
		{
			// 找到对应的相控点
			for (auto &p : m_surveysInfo.ControlPoints)
			{
				if (p.control_id == id)
				{

					//当前点的刺点信息保存为 update_photos_tp_input.json				
					SaveCurrGcpInfoToJson(p);
					m_krGcp->updataPhotos();
					//连接点信息更新&&连接点潜在匹配项解析
					UpdatePhotoOutputJson();

					//显示对应的缩略图列表
					for (auto &m : p.potential_match)
					{
						m_matchThumbnail.push_back(m.image_path.toStdString());
					}
					break;
				}
			}
		}
		else if (pointItem->type() == enTypeTP)
		{
			// 找到对应的连接点
			for (auto &p : m_surveysInfo.TiePoints.tie_points)
			{
				if (p.id == id)
				{
					//计算连接点的潜在匹配项

					//当前点的刺点信息保存为 update_photos_tp_input.json				
					SaveCurrTpInfotoJson(p);
					m_krGcp->updataPhotosTP();
					//连接点信息更新&&连接点潜在匹配项解析
					UpdatePhotoTPOutputJson();

					//显示对应的缩略图列表
					for (auto &m : p.potential_match)
					{
						m_matchThumbnail.push_back(m.image_path.toStdString());
					}
					break;
				}
			}
		}

        
	}else if (pointItem != nullptr && pointItem->parent() != nullptr && pointItem->parent()->parent() == nullptr)
	{
		const int id = pointItem->parent()->data(0, Qt::UserRole).toInt();

		if (pointItem->parent()->type() == enTypeGCP)
		{
			//找到对应控制点数据
			for (auto &p : m_surveysInfo.ControlPoints)
			{
				if (p.control_id == id)
				{
					//显示对应的缩略图列表
					for (auto &m : p.potential_match)
					{
						m_matchThumbnail.push_back(m.image_path.toStdString());
					}
					break;
				}
			}
		}
		else if (pointItem->parent()->type() == enTypeTP)
		{
			//找到对应连接点数据
			for (auto &p : m_surveysInfo.TiePoints.tie_points)
			{
				if (p.id == id)
				{
					//当前点的刺点信息保存为 update_photos_tp_input.json				
					SaveCurrTpInfotoJson(p);
					m_krGcp->updataPhotosTP();
					//连接点信息更新&&连接点潜在匹配项解析
					UpdatePhotoTPOutputJson();

					//显示对应的缩略图列表
					for (auto &m : p.potential_match)
					{
						m_matchThumbnail.push_back(m.image_path.toStdString());
					}
					break;
				}
			}
		}
	}
	else //没有对控制点进行选取，显示所有控制点的潜在匹配项
	{
		//显示所有控制点缩略图列表
		for (auto &p : m_surveysInfo.ControlPoints)
		{
            for (auto &m : p.potential_match)
            {
                m_matchThumbnail.push_back(m.image_path.toStdString());
            }
		}

// 		for (auto &p : m_surveysInfo.TiePoints.tie_points)
// 		{
// 			for (auto &m : p.potential_match)
// 			{
// 				m_matchThumbnail.push_back(m.image_path.toStdString());
// 			}
// 		}
	}

    // 去重
    std::sort(m_matchThumbnail.begin(), m_matchThumbnail.end());
    vector<std::string>::iterator iter = std::unique(m_matchThumbnail.begin(), m_matchThumbnail.end());
    m_matchThumbnail.erase(iter, m_matchThumbnail.end());
    // 加载图片
//	LOGFMTI("[SurveyWidget] matchThumbnail clicked  imagenum   %d  first path %s ", m_matchThumbnail.size(), m_matchThumbnail[0].c_str());
	LOGFMTI("[SurveyWidget] matchThumbnail clicked  imagenum   %d  ", m_matchThumbnail.size());
    showImageList(m_matchThumbnail);

}
void SurveyWidget::on_selectedButton_clicked()
{
	ui.matchButton->setChecked(false);
	ui.allImgButton->setChecked(false);

    m_currentSelectedBtnIndex = enIndexSurvey;
	//清空当前列表
	m_initListWidget = false;
	ui.imgListWidget->clear();
    m_surveyThumbnail.clear();
	//m_surveysInfo.ControlPoints photos信息显示

	//如果有对控制点进行选取，显示对应控制点的已刺点项
	QTreeWidgetItem * pointItem = ui.controlPointListWidget->currentItem();

	if (pointItem != nullptr && pointItem->parent() == nullptr) // 如果为空说明是顶层的相控点
	{
		const int id = pointItem->data(0, Qt::UserRole).toInt();
		// 找到对应的相控点
		for (auto &p : m_surveysInfo.ControlPoints)
		{
			if (p.control_id == id)
			{
				//显示对应的已刺点列表
				for (auto &m : p.photos)
				{
					if (m_map_imageThum.find(m.image_path.toStdString()) == m_map_imageThum.end())
					{
						return;
					}
					QString imgFile = QString::fromLocal8Bit(m_map_imageThum[m.image_path.toStdString()].c_str());
					m_surveyThumbnail.push_back(imgFile.toStdString());
				}
				break;
			}
		}

	}
	else if (pointItem != nullptr && pointItem->parent() != nullptr && pointItem->parent()->parent() == nullptr)
	{
		const int id = pointItem->parent()->data(0, Qt::UserRole).toInt();
		//找到对应控制点数据
		for (auto &p : m_surveysInfo.ControlPoints)
		{
			if (p.control_id == id)
			{
				//显示对应的已刺点列表
				for (auto &m : p.photos)
				{
					if (m_map_imageThum.find(m.image_path.toStdString()) == m_map_imageThum.end())
					{
						return;
					}
					QString imgFile = QString::fromLocal8Bit(m_map_imageThum[m.image_path.toStdString()].c_str());
					m_surveyThumbnail.push_back(imgFile.toStdString());
				}
				break;
			}
		}

	}
	else //没有对控制点进行选取，显示所有控制点的潜在匹配项
	{
		//显示所有控制点已刺点列表
		for (auto &p : m_surveysInfo.ControlPoints)
		{
			for (auto &m : p.photos)
			{
				if (m_map_imageThum.find(m.image_path.toStdString()) == m_map_imageThum.end())
				{
					return;
				}
				QString imgFile = QString::fromLocal8Bit(m_map_imageThum[m.image_path.toStdString()].c_str());
				m_surveyThumbnail.push_back(imgFile.toStdString());
			}
		}
	}


    // 加载图片
//	LOGFMTI("[SurveyWidget] surveyThumbnail clicked  imagenum   %d  first path %s ", m_surveyThumbnail.size(), m_surveyThumbnail[0].c_str());
	LOGFMTI("[SurveyWidget] surveyThumbnail clicked  imagenum   %d  ", m_surveyThumbnail.size());
    showImageList(m_surveyThumbnail);
}

void SurveyWidget::onRecvResponse(QSharedPointer<ResponseHead> response)
{
	int type = response->cmdType;
	if (type == RESTAPI_UPLOAD_JSON_FILE_URL) {
		if (response->code == HTTP_ERROR_SUCCESS) {
			QSharedPointer<ReportLogResponse> resp = qSharedPointerCast<ReportLogResponse>(response);
			
			if (m_blockMergeStateSubmit)
			{
				if (m_failListFrames.size() != 0)
				{
					for (int j = 0; j < m_failListFrames.size(); j++)
					{
						HttpCmdManager::getInstance()->frameOperatorRecommit(m_at_taskID, m_failListFrames.at(j).id, false);
					}
					MSGBOXPARAM *param = new MSGBOXPARAM;
					CGlobalHelp::Response(UM_RESUBMIT_TASK_FREAME_RET, m_at_taskID, (intptr_t)param);
				}
				else
				{
					for (int j = 0; j < m_mergeFrames.size(); j++)
					{
						HttpCmdManager::getInstance()->frameOperatorRecommit(m_at_taskID, m_mergeFrames.at(j).id, false);
					}
					MSGBOXPARAM *param = new MSGBOXPARAM;
					CGlobalHelp::Response(UM_RESUBMIT_TASK_FREAME_RET, m_at_taskID, (intptr_t)param);
				}
			}
			else
			{
				HttpCmdManager::getInstance()->taskOperatorResubAtSurvey(enReSubFinishedTask, m_at_taskID, m_surveyPointXml_path); // 状态传4
			}			
		}
	}
	if (type == RESTAPI_JOB_FRAME_RENDER_INFO) {

		if (response->code == HTTP_ERROR_SUCCESS) {
			QSharedPointer<TaskFrameRenderingInfoResponse> task = qSharedPointerCast<TaskFrameRenderingInfoResponse>(response);

//			QList<JobState::DetailInfo> m_blockLisFrames;   //所有帧
//			QList<JobState::DetailInfo> m_failListFrames;  //失败空三
//			QList<JobState::DetailInfo> m_mergeFrames;   //合并帧

			if (task->detailItems.size() > 1)
			{
				m_blockMergeStateSubmit = true;
			}

			for (int i = 0; i < task->detailItems.size(); i++)
			{
				JobState::DetailInfo * info = task->detailItems.at(i);
				int status = info->frameStatus;
				if (status == 4)
				{
					m_blockLisFrames.append(*info);
				}
				if (status == 5)
				{
					m_failListFrames.append(*info);
				}
				QString kmlName = info->kmlName;
				if (kmlName == NULL)
				{
					m_mergeFrames.append(*info);
				}
			}

/*			if (task->detailItems.size() == m_blockLisFrames.size() && task->detailItems.size() != 0)
			{
				for (int j = 0; j < m_mergeFrames.size(); j++)
				{
					HttpCmdManager::getInstance()->frameOperatorRecommit(m_at_taskID, m_mergeFrames.at(j).id, false);
				}
			}
			else if (m_blockLisFrames.size() >= 1 && m_blockLisFrames.size() < task->detailItems.size())
			{
				for (int j = 0; j < m_failListFrames.size(); j++)
				{
					HttpCmdManager::getInstance()->frameOperatorRecommit(m_at_taskID, m_failListFrames.at(j).id, false);
				}
			}
			else */
			{
				JobState* jobstate = JobStateMgr::Inst()->LookupTaskstate(m_at_taskID);
				t_taskid at_parents_id = jobstate->getTaskParentId();

				if (jobstate->getStatus() == JobState::enRenderTaskFinished || m_blockMergeStateSubmit) {
					//JobState::DetailInfo  info = JobState::DetailInfo;
					HttpCmdManager::getInstance()->uploadTaskSurveyFile(at_parents_id, m_surveyPointXml_path, this);
				}
				else
				{
#ifdef FOXRENDERFARM
					Util::ShowMessageBoxError(QObject::tr("After the AT is successful, upload the control point data again！(ID:%1 Status:%2)").arg(m_at_taskID).arg(jobstate->getStatus()));
#else
					Util::ShowMessageBoxError(QObject::tr("初始空三失败，请完成初次空三，成功后，在进行控制点刺点重提 ！(ID:%1 Status:%2)").arg(m_at_taskID).arg(jobstate->getStatus()));
#endif		
					return;
				}
			}

		}
	}
}

void SurveyWidget::parseEpipolarCoefficient(std::vector <EpipolarCoefficient>& _eps)
{
	//读入所有内容
	QFile file(m_compute_line_output);
	file.open(QIODevice::ReadOnly);
	auto data = file.readAll();
	file.close();

	//解析
	QJsonParseError parseError;
	QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

	if (parseError.error != QJsonParseError::NoError)
	{
		qDebug() << "读取compute_line_output文件失败";
		return ;
	}

	QJsonObject obj = doc.object();
	auto keys = obj.keys();

	for (auto &k : keys)
	{
		QString cpname = k;
		QJsonArray array = obj[k].toArray();
		int size = array.size();
		for (auto &arr : array)
		{
			QJsonObject oec = arr.toObject();

			EpipolarCoefficient ec;

			ec.a = oec["a"].toString().toDouble();
			ec.b = oec["b"].toString().toDouble();
			ec.c = oec["c"].toString().toDouble();
			ec.d = oec["d"].toString().toDouble();
			ec.mode = oec["mode"].toString();
			ec.min = oec["min"].toString().toDouble();
			ec.max = oec["max"].toString().toDouble();

			_eps.push_back(ec);
		}

	}
}

void SurveyWidget::creatLines(const std::vector <EpipolarCoefficient>& _eps, const ushort& _width, const ushort &_height, std::vector<std::vector<QPoint>> & _lines)
{
	for (auto &ep : _eps)
	{
		vector< QPoint> line_points;
		bool epc = true;
		// bool lessThanMIn = false;  //如果有一次<0 和一次>Max ,就退出循环计算 
		// bool moreThanMax = false;  
		if (ep.mode == "fx")
		{
			for (size_t i = 0; i < _width; i++)
			{
				if (ep.min > 0 && epc)
				{
					double pix_y = ep.a * pow(ep.min, 3) + ep.b * pow(ep.min, 2) + ep.c * ep.min + ep.d;

					if (pix_y< 0 - 10 || pix_y >_height + 10) //10个像素的外扩
					{
						i = int(ep.min);
						epc = false;
						continue;
					}
					QPoint temp_point(ep.min, pix_y);
					line_points.push_back(temp_point);
					i = int(ep.min);
					epc = false;
					continue;
				}
				if (i<ep.max && i + 1 >ep.max)
				{
					double pix_y = ep.a * pow(ep.max, 3) + ep.b * pow(ep.max, 2) + ep.c * ep.max + ep.d;
					if (pix_y<0-10 || pix_y>_height+10)  
						break;
					QPoint temp_point(ep.max, pix_y);
					line_points.push_back(temp_point);
					break;
				}
				double pix_y = ep.a * pow(i, 3) + ep.b * pow(i, 2) + ep.c * i + ep.d;
				if (pix_y<0 - 10 || pix_y>_height + 10) 
					//添加多项式求解
					continue;
				QPoint temp_point(i, pix_y);
				line_points.push_back(temp_point);
			}
		}

		if (ep.mode == "fy")
		{
			for (size_t i = 0; i < _height; i++)
			{
				if (ep.min > 0 && epc)
				{
					double pix_x = ep.a * pow(ep.min, 3) + ep.b * pow(ep.min, 2) + ep.c * ep.min + ep.d;
					if (pix_x<0 - 10 || pix_x>_width + 10)
					{
						i = int(ep.min);
						epc = false;
						continue;
					}
					QPoint temp_point(pix_x, ep.min);
					line_points.push_back(temp_point);
					i = int(ep.min);
					epc = false;
					continue;
				}

				if (i<ep.max && i + 1 >ep.max)
				{
					double pix_x = ep.a * pow(ep.max, 3) + ep.b * pow(ep.max, 2) + ep.c * ep.max + ep.d;
					if (pix_x<0 - 10 || pix_x>_width + 10)
						break;
					QPoint temp_point(pix_x, ep.max);
					line_points.push_back(temp_point);
					break;
				}

				double pix_x = ep.a * pow(i, 3) + ep.b * pow(i, 2) + ep.c * i + ep.d;
				if (pix_x<0 - 10 || pix_x>_width + 10)
					//添加多项式求解
					continue;
				QPoint temp_point(pix_x, i);
				line_points.push_back(temp_point);
			}
		}
#if 0
		for (size_t i = 0; i < _width; i++)
		{
			double pix_y = ep.a * pow(i, 3) + ep.b * pow(i, 2) + ep.c * i + ep.d;
			if (pix_y<0 || pix_y>_height)
				continue;
			QPoint temp_point(i, pix_y);
			line_points.push_back(temp_point);
		}
#endif
		if (line_points.size() < 2)
		{
			return;
		}
		//for (size_t i = 0;i<line_points.size()-1;i++)
		//{
		// QLine temp_line(line_points[i], line_points[i + 1]);
		// _lines.push_back(temp_line);
		//}

		_lines.emplace_back(std::move(line_points));

		//line_points.clear();
	}
}
void SurveyWidget::SaveCurrGcpInfoToJson(const SURVEYS_INFO::surveys_point& _point)
{
	QString outdir = m_rootDir + "/input";
	QDir().mkpath(outdir);
	QFile file(m_update_photos_input);
	if (!file.open(QIODevice::WriteOnly))
	{
		return;
	}
	QJsonObject json;

	QJsonObject point;
	point["name"] = _point.control_name;
	point["x"] = _point.coordinate.x;
	point["y"] = _point.coordinate.y;
	point["z"] = _point.coordinate.z;
	int tep_srs = _point.srs_define.mid(_point.srs_define.indexOf(":") + 1).toInt();
	point["SRS"] = tep_srs;
	json["point"] = point;   //point


	QJsonArray photolist;
	for (auto & e : _point.photos)
	{
		QJsonObject pobj;
		pobj["photo"] = e.image_path;
		pobj["photoGroup"] = e.photoGroup;
		pobj["u"] = e.pix_x;
		pobj["v"] = e.pix_y;

		photolist.push_back(pobj);
	}

	json["photoList"] = photolist;  //pointList
	QByteArray data = QJsonDocument(json).toJson();
	file.write(data);
	file.close();

	if (QFile::exists(m_update_photos_input))
		cout << "file is exist!" << endl;
}

bool SurveyWidget::saveXmlToFile()
{
	QFile file(m_surveyPointXml_path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		return false;
	}

	QXmlStreamWriter writer(&file);
	writer.setAutoFormatting(true);
	writer.writeStartDocument();
	writer.writeStartElement("SurveysData");

	// SpatialReferenceSystems
	{
		writer.writeStartElement("SpatialReferenceSystems");

		for (int i = 0; i < m_surveysInfo.SpatialReferenceSystems.size(); i++)
		{
			SURVEYS_INFO::SRS srs = m_surveysInfo.SpatialReferenceSystems.at(i);

			writer.writeStartElement("SRS");
			writer.writeTextElement("Id", "99");  //QString::number(srs.srs_id)
			if (m_surveysInfo.TiePoints.tie_points.size()>0)
			{
				QString srsDefine = QString("EPSG:%1").arg(m_surveysInfo.TiePoints.srs_id);
				writer.writeTextElement("Name", srsDefine);
				writer.writeTextElement("Definition", srsDefine);
			}
			else if(m_surveysInfo.ControlPoints.size()>0)
			{
				writer.writeTextElement("Name", srs.srs_name);
				writer.writeTextElement("Definition", srs.srs_define);
			}
			writer.writeEndElement(); // end SRS
		}
		writer.writeEndElement(); // end SpatialReferenceSystems
	}

	// ControlPoints
	{
		writer.writeStartElement("ControlPoints");

		// ControlPoint
		{
			for (int i = 0; i < m_surveysInfo.ControlPoints.size(); i++)
			{
				SURVEYS_INFO::surveys_point sp = m_surveysInfo.ControlPoints.at(i);

				writer.writeStartElement("ControlPoint");

				writer.writeTextElement("Id", QString::number(sp.control_id));
				writer.writeTextElement("SRSId","99");  //QString::number(sp.srs_id)
				writer.writeTextElement("Name", sp.control_name);
				writer.writeTextElement("Category", "Full");   //sp.category
				writer.writeTextElement("HorizontalAccuracy", sp.HorizontalAccuracy);
				writer.writeTextElement("VerticalAccuracy", sp.VerticalAccuracy);
				writer.writeTextElement("CheckPoint", sp.CheckPoint ? "true" : "false");

				// Position
				{
					writer.writeStartElement("Position");

					writer.writeTextElement("x", sp.coordinate.x);
					writer.writeTextElement("y", sp.coordinate.y);
					writer.writeTextElement("z", sp.coordinate.z);

					writer.writeEndElement(); // end Position
				}

				// Measurement
				{
					// photos
					for (int j = 0; j < sp.photos.size(); j++)
					{
						SURVEYS_INFO::imageMeasurement im = sp.photos.at(j);

						writer.writeStartElement("Measurement");

						writer.writeTextElement("PhotoId", QString::number(im.image_id));

						std::string local_img = im.image_path.toStdString();
						if (m_map_imageThum.find(local_img) == m_map_imageThum.end())
						{
							return false;
						}
						std::string  local_img_thum = m_map_imageThum[local_img];
						if (m_map_Local2XML.find(local_img_thum) == m_map_Local2XML.end())
						{
							return false;
						}
						std::string  xml_image_path = m_map_Local2XML[local_img_thum].imagpath.toStdString();

						writer.writeTextElement("ImagePath", QString::fromLocal8Bit(xml_image_path.c_str()));
						writer.writeTextElement("x", QVariant(im.pix_x).toString());
						writer.writeTextElement("y", QVariant(im.pix_y).toString());

						writer.writeEndElement(); // end Measurement				
					}
#if 0
					// potential_match photos
					for (int j = 0; j < sp.potential_match.size(); j++)
					{
						imageMeasurement im = sp.potential_match.at(j);

						writer.writeStartElement("Measurement");

						writer.writeTextElement("PhotoId", QString::number(im.image_id));
						writer.writeTextElement("ImagePath", im.image_path);
						writer.writeTextElement("x", QVariant(im.pix_x).toString());
						writer.writeTextElement("y", QVariant(im.pix_y).toString());

						writer.writeEndElement(); // end Measurement
					}
#endif
				}
				writer.writeEndElement(); // end ControlPoint	
			}
		}
		writer.writeEndElement(); // end ControlPoints
	}
	

	// TiePoints
	{
		writer.writeStartElement("TiePoints");
		writer.writeTextElement("SRSId", "99");    //QString::number(m_surveysInfo.TiePoints.srs_id) SRS_ID 的索引

		//TiePoint
		for (int i = 0; i < m_surveysInfo.TiePoints.tie_points.size(); i++)
		{
			SURVEYS_INFO::surveys_tiePnt tp = m_surveysInfo.TiePoints.tie_points.at(i);

			writer.writeStartElement("TiePoint");

			writer.writeTextElement("Id", QString::number(tp.id));
			writer.writeTextElement("Name", tp.name);
			// Position
			{
				writer.writeStartElement("Position");

				writer.writeTextElement("x", tp.coordinate.x);
				writer.writeTextElement("y", tp.coordinate.y);
				writer.writeTextElement("z", tp.coordinate.z);

				writer.writeEndElement(); // end Position
			}
			writer.writeTextElement("Type", tp.type);

			// Measurement
			{
				// photos
				for (int j = 0; j < tp.photos.size(); j++)
				{
					SURVEYS_INFO::imageMeasurement im = tp.photos.at(j);

					writer.writeStartElement("Measurement");

					writer.writeTextElement("PhotoId", QString::number(im.image_id));
//					writer.writeTextElement("ImagePath", im.image_path);
					std::string local_img = im.image_path.toStdString();
					if (m_map_imageThum.find(local_img) == m_map_imageThum.end())
					{
						return false;
					}
					std::string  local_img_thum = m_map_imageThum[local_img];
					if (m_map_Local2XML.find(local_img_thum) == m_map_Local2XML.end())
					{
						return false;
					}
					std::string  xml_image_path = m_map_Local2XML[local_img_thum].imagpath.toStdString();

					writer.writeTextElement("ImagePath", QString::fromLocal8Bit(xml_image_path.c_str()));

					writer.writeTextElement("x", QVariant(im.pix_x).toString());
					writer.writeTextElement("y", QVariant(im.pix_y).toString());

					writer.writeEndElement(); // end Measurement				
				}

			}
			writer.writeEndElement(); // end TiePoint	
		}

		writer.writeEndElement(); // end TiePoints
	}


	writer.writeEndElement(); // end SurveysData
	writer.writeEndDocument();
	file.close();

	return true;
}

void SurveyWidget::calcEpipolarLineData(const SURVEYS_INFO::surveys_point& p,const QString& imgFile,const QString& imgthumFile)
{
	ushort image_width, image_height;

	if (p.photos.size() > 0)
	{
		// 构建 compute_line_input.json 
		QString out_file_path = m_compute_line_input;
		QFile file(out_file_path);
		if (!file.open(QIODevice::WriteOnly))
		{
			return;
		}
		QJsonObject json;
		QJsonObject gcpID;
		QJsonObject source;

		QJsonArray sourse_list;
		for (auto & e : p.photos)
		{
			if (e.image_path == imgFile)
			{
				continue;
			}

			QJsonObject eobj;

			std::string local_img = e.image_path.toStdString();

			if (m_map_imageThum.find(local_img) == m_map_imageThum.end())
			{
				return;
			}
			std::string  local_img_thum = m_map_imageThum[local_img];
			if (m_map_Local2XML.find(local_img_thum) == m_map_Local2XML.end())
			{
				return;
			}
			std::string  xml_image_path = m_map_Local2XML[local_img_thum].imagpath.toStdString();

			eobj["photo"] = QString::fromLocal8Bit(xml_image_path.c_str());	           //xml文件中的影像路径
//			eobj["photoGroup"] = e.photoGroup;          //e.photoGroup  刺控制点影像对应的组

			eobj["photoGroup"] = m_map_Local2XML[local_img_thum].group;
			eobj["u"] = e.pix_x;
			eobj["v"] = e.pix_y;

			sourse_list.push_back(eobj);
		}
		gcpID["sourse_list"] = sourse_list;

		QJsonObject target;

		std::string t_local_img = imgFile.toStdString();

		if (m_map_imageThum.find(t_local_img) == m_map_imageThum.end())
		{
			return;
		}
		std::string  t_local_img_thum = m_map_imageThum[t_local_img];
		if (m_map_Local2XML.find(t_local_img_thum) == m_map_Local2XML.end())
		{
			return;
		}
		std::string t_xml_image_path = m_map_Local2XML[t_local_img_thum].imagpath.toStdString();
		target["photo"] = QString::fromLocal8Bit(t_xml_image_path.c_str());   //此处用xml中的影像信息

		if (m_map_Local2XML.find(imgthumFile.toStdString()) == m_map_Local2XML.end())
		{
			return;
		}
		QString photogroup = m_map_Local2XML[imgthumFile.toStdString()].group;
		image_width = m_map_Local2XML[imgthumFile.toStdString()].width;
		image_height = m_map_Local2XML[imgthumFile.toStdString()].height;

		target["photoGroup"] = photogroup;   // 需要获取影像对应的影像组信息
		gcpID["m_target"] = target;
		source[to_string(p.control_id).c_str()] = gcpID;
		json["source"] = source;

		QByteArray data = QJsonDocument(json).toJson();
		file.write(data);
		file.close();

		m_krGcp->computeLine();
	}
	//解析输出的数据
	std::vector<EpipolarCoefficient> point_ec;
	std::vector<std::vector<QPoint>> EC_lines;
	parseEpipolarCoefficient(point_ec);
	if (point_ec.size() > 0)
	{
		creatLines(point_ec, image_width, image_height, EC_lines);  // 可优化，比如求出边界值起止位置在开始创建line；->QPainterPath
		ui.viewer_Image->setLines(EC_lines);
	}
}


void SurveyWidget::calcEpipolarLineDataTP(const SURVEYS_INFO::surveys_tiePnt& p, const QString &imgFile, const QString& imgthumFile)
{
	ushort image_width, image_height;

	if (p.photos.size() > 0)
	{
		// 构建 compute_line_input.json 
		QString out_file_path = m_compute_line_input;
		QFile file(out_file_path);
		if (!file.open(QIODevice::WriteOnly))
		{
			return;
		}
		QJsonObject json;
		QJsonObject gcpID;
		QJsonObject source;

		QJsonArray sourse_list;
		for (auto & e : p.photos)
		{
			if (e.image_path == imgFile)
			{
				continue;
			}

			QJsonObject eobj;

			std::string local_img = e.image_path.toStdString();

			if (m_map_imageThum.find(local_img) == m_map_imageThum.end())
			{
				return;
			}
			std::string  local_img_thum = m_map_imageThum[local_img];
			if (m_map_Local2XML.find(local_img_thum) == m_map_Local2XML.end())
			{
				return;
			}
			std::string  xml_image_path = m_map_Local2XML[local_img_thum].imagpath.toStdString();

			eobj["photo"] = QString::fromLocal8Bit(xml_image_path.c_str());	           //xml文件中的影像路径
//			eobj["photoGroup"] = e.photoGroup;          //e.photoGroup  刺控制点影像对应的组

			eobj["photoGroup"] = m_map_Local2XML[local_img_thum].group;
			eobj["u"] = e.pix_x;
			eobj["v"] = e.pix_y;

			sourse_list.push_back(eobj);
		}
		gcpID["sourse_list"] = sourse_list;

		QJsonObject target;

		std::string t_local_img = imgFile.toStdString();

		if (m_map_imageThum.find(t_local_img) == m_map_imageThum.end())
		{
			return;
		}
		std::string  t_local_img_thum = m_map_imageThum[t_local_img];
		if (m_map_Local2XML.find(t_local_img_thum) == m_map_Local2XML.end())
		{
			return;
		}
		std::string t_xml_image_path = m_map_Local2XML[t_local_img_thum].imagpath.toStdString();
		target["photo"] = QString::fromLocal8Bit(t_xml_image_path.c_str());   //此处用xml中的影像信息

		if (m_map_Local2XML.find(imgthumFile.toStdString()) == m_map_Local2XML.end())
		{
			return;
		}
		QString photogroup = m_map_Local2XML[imgthumFile.toStdString()].group;
		image_width = m_map_Local2XML[imgthumFile.toStdString()].width;
		image_height = m_map_Local2XML[imgthumFile.toStdString()].height;

		target["photoGroup"] = photogroup;   // 需要获取影像对应的影像组信息
		gcpID["m_target"] = target;
//		source[to_string(p.control_id).c_str()] = gcpID;  //ID 换成name
		source[p.name] = gcpID;
		json["source"] = source;

		QByteArray data = QJsonDocument(json).toJson();
		file.write(data);
		file.close();

		m_krGcp->computeLine();
	}
	//解析输出的数据
	std::vector<EpipolarCoefficient> point_ec;
	std::vector<std::vector<QPoint>> EC_lines;
	parseEpipolarCoefficient(point_ec);
	if (point_ec.size() > 0)
	{
		creatLines(point_ec, image_width, image_height, EC_lines);  // 可优化，比如求出边界值起止位置在开始创建line；->QPainterPath
		ui.viewer_Image->setLines(EC_lines);
	}
}
bool SurveyWidget::loadXmlFileToCP()
{
	QFile file(m_surveyPointXml_path);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		return false;
	}
	QXmlStreamReader reader;
	reader.setDevice(&file);

	QList<SURVEYS_INFO::surveys_point> cps;
	SURVEYS_INFO::surveys_tiePnts tps;

	while (reader.readNextStartElement() || !reader.atEnd())
	{
		if (reader.name() == "SurveysData")
		{
			while (reader.readNextStartElement())
			{		
				if (reader.name() == "ControlPoints")
				{
					while (reader.readNextStartElement())
					{
						if (reader.name() == "ControlPoint")
						{
							SURVEYS_INFO::surveys_point scp;
							while (reader.readNextStartElement())
							{
								if (reader.name() == "Id") {
									scp.control_id = reader.readElementText().toInt();
								}
								else if (reader.name() == "SRSId") {
									scp.srs_id = reader.readElementText().toInt();
								}
								else if (reader.name() == "Name") {
									scp.control_name = reader.readElementText();
								}
								else if (reader.name() == "Category") {
									scp.category = reader.readElementText();
								}
								else if (reader.name() == "HorizontalAccuracy") {
									scp.HorizontalAccuracy = reader.readElementText();
								}
								else if (reader.name() == "VerticalAccuracy") {
									scp.VerticalAccuracy = reader.readElementText();
								}
								else if (reader.name() == "CheckPoint") {
									scp.CheckPoint = reader.readElementText() == "true" ? true : false;
								}
								else if (reader.name() == "Position") {

									while (reader.readNextStartElement()) {

										QString tokenName5 = reader.name().toString();
										if (reader.name() == "x") {
											scp.coordinate.x = reader.readElementText();
										}
										else if (reader.name() == "y") {
											scp.coordinate.y = reader.readElementText();
										}
										else if (reader.name() == "z") {
											scp.coordinate.z = reader.readElementText();
										}
										else {
											reader.skipCurrentElement();
										}
									}
								}
								else if (reader.name() == "Measurement")
								{
									SURVEYS_INFO::imageMeasurement imgMt;
									while (reader.readNextStartElement()) {
										if (reader.name() == "PhotoId")
										{
											imgMt.image_id = reader.readElementText().toInt();
										}
										else if (reader.name() == "ImagePath")
										{
											imgMt.image_path = reader.readElementText();

											std::string  xml_image_path = imgMt.image_path.toStdString();
											if (m_map_XML2localthumimg.find(xml_image_path) != m_map_XML2localthumimg.end())
											{
												std::string  t_local_img_thum_g = m_map_XML2localthumimg[xml_image_path];
												if (m_map_Local2XML.find(t_local_img_thum_g) != m_map_Local2XML.end())
												{
													ImageGroupInfo curr_igInfo = m_map_Local2XML[t_local_img_thum_g];
													imgMt.photoGroup = curr_igInfo.group;
												}
											}

										}
										else if (reader.name() == "x")
										{
											imgMt.pix_x = reader.readElementText().toDouble();
										}
										else if (reader.name() == "y")
										{
											imgMt.pix_y = reader.readElementText().toDouble();
										}
										else {
											reader.skipCurrentElement();
										}
									}
									scp.photos.push_back(imgMt);
									
								}
								else {
									reader.skipCurrentElement();
								}
							}

							cps.push_back(scp);
						}
						else
						{
							reader.skipCurrentElement();
						}
					}					
				}
				else if (reader.name() == "TiePoints")
				{
					while (reader.readNextStartElement())
					{
						if (reader.name() == "SRSId")
						{
							tps.srs_id = reader.readElementText().toInt();
						}
						else if (reader.name() == "TiePoint")
						{
							SURVEYS_INFO::surveys_tiePnt currTP;
							while (reader.readNextStartElement())
							{
								if (reader.name() == "Id")
								{
									currTP.id = reader.readElementText().toInt();
								}
								else if (reader.name() == "Name")
								{
									currTP.name = reader.readElementText();
								}
								else if (reader.name() == "Position")
								{
									while (reader.readNextStartElement()) {

										if (reader.name() == "x") {
											currTP.coordinate.x = reader.readElementText();
										}
										else if (reader.name() == "y") {
											currTP.coordinate.y = reader.readElementText();
										}
										else if (reader.name() == "z") {
											currTP.coordinate.z = reader.readElementText();
										}
										else {
											reader.skipCurrentElement();
										}
									}
								}
								else if (reader.name() == "Type")
								{
									currTP.type = reader.readElementText();
								}
								else if (reader.name() == "Measurement")
								{
									SURVEYS_INFO::imageMeasurement imgMt;
									while (reader.readNextStartElement()) {
										if (reader.name() == "PhotoId")
										{
											imgMt.image_id = reader.readElementText().toInt();
										}
										else if (reader.name() == "ImagePath")
										{
											QString xml_image = reader.readElementText();
											std::string  xml_image_path = xml_image.toStdString();
											if (m_map_XML2localthumimg.find(xml_image_path) != m_map_XML2localthumimg.end())
											{
												std::string  t_local_img_thum_g = m_map_XML2localthumimg[xml_image_path];
												if (m_map_Local2XML.find(t_local_img_thum_g) != m_map_Local2XML.end())
												{
													ImageGroupInfo curr_igInfo = m_map_Local2XML[t_local_img_thum_g];
													imgMt.photoGroup = curr_igInfo.group;

													if (m_map_thumimage.find(t_local_img_thum_g.c_str()) != m_map_thumimage.end())
													{
														imgMt.image_path = QString::fromLocal8Bit(m_map_thumimage[t_local_img_thum_g].c_str());
													}													
												}
											}
										}
										else if (reader.name() == "x")
										{
											imgMt.pix_x = reader.readElementText().toDouble();
										}
										else if (reader.name() == "y")
										{
											imgMt.pix_y = reader.readElementText().toDouble();
										}
										else {
											reader.skipCurrentElement();
										}
									}
									currTP.photos.push_back(imgMt);
								}
								else
								{
									reader.skipCurrentElement();
								}
							}

							m_surveysInfo.TiePoints.tie_points.push_back(currTP);
						}
						else
						{
							reader.skipCurrentElement();
						}
					}

				}
				else
				{
					reader.skipCurrentElement();
				}

			}
		}
		else
		{
			reader.skipCurrentElement();
		}
	}

	file.close();
	for (size_t i = 0; i < m_surveysInfo.ControlPoints.size(); i++) 
	{
		for (size_t j = 0; j < cps.size(); j++) 
		{
			if (m_surveysInfo.ControlPoints[i].coordinate.x == cps[j].coordinate.x &&
				m_surveysInfo.ControlPoints[i].coordinate.y == cps[j].coordinate.y &&
				m_surveysInfo.ControlPoints[i].coordinate.z == cps[j].coordinate.z
				)
			{
				for (size_t m = 0; m < cps[j].photos.size(); m++)
				{
					SURVEYS_INFO::imageMeasurement temp;
					temp.image_id = cps[j].photos[m].image_id;
					temp.pix_x = cps[j].photos[m].pix_x;
					temp.pix_y = cps[j].photos[m].pix_y;
						
					if (m_map_XML2localthumimg.find(cps[j].photos[m].image_path.toStdString()) == m_map_XML2localthumimg.end())
					{
						return false;
					}
					std::string  t_local_img_thum_g = m_map_XML2localthumimg[cps[j].photos[m].image_path.toStdString()];

					if (m_map_thumimage.find(t_local_img_thum_g.c_str()) == m_map_thumimage.end())
					{
						return false;
					}

					QString localImgFile = QString::fromLocal8Bit(m_map_thumimage[t_local_img_thum_g].c_str());
					temp.image_path = localImgFile;

					temp.photoGroup = cps[j].photos[m].photoGroup;

					m_surveysInfo.ControlPoints[i].photos.push_back(temp);
				}

//				m_surveysInfo.ControlPoints[i].photos = cps[j].photos;
			}
		}
	}
	
	//test 连接点
	m_surveysInfo.TiePoints.srs_id = 99;

	//计算核线

#if 0

	bool calc_flag = false;

	for (size_t i = 0;i<m_surveysInfo.ControlPoints.size();i++)
	{
		for (size_t j = 0;j<m_surveysInfo.ControlPoints[i].photos.size();j++)
		{
			if (m_map_XML2localthumimg.find(m_surveysInfo.ControlPoints[i].photos[j].image_path.toStdString()) == m_map_XML2localthumimg.end())
			{
				continue;
			}
			QString localthumimg = QString::fromLocal8Bit( m_map_XML2localthumimg[m_surveysInfo.ControlPoints[i].photos[j].image_path.toStdString()].c_str());

			if (m_map_thumimage.find(localthumimg.toStdString()) == m_map_thumimage.end())
			{
				continue;
			}
			QString localImg = QString::fromLocal8Bit(m_map_thumimage[localthumimg.toStdString()].c_str());

			initCPEpipolarLine(m_surveysInfo.ControlPoints[i], localImg, localthumimg);
			calc_flag = true;
			break;
		}	
		if (calc_flag)
		{
			break;
		}
	}

	//更新潜在匹配项
	m_krGcp->updataPhotos();
#endif
	
	showControlPointList();

	return true;
}

//初始化控制点数据显示
void SurveyWidget::initCPEpipolarLine(const SURVEYS_INFO::surveys_point& p, const QString& imgFile, const QString& imgthumFile)
{
	ushort image_width, image_height;

	if (p.photos.size() > 0)
	{
		// 构建 compute_line_input.json 
		QString out_file_path = m_compute_line_input;
		QFile file(out_file_path);
		if (!file.open(QIODevice::WriteOnly))
		{
			return;
		}
		QJsonObject json;
		QJsonObject gcpID;
		QJsonObject source;

		QJsonArray sourse_list;
		for (auto & e : p.photos)
		{
			QJsonObject eobj;
			std::string  xml_image_path = e.image_path.toStdString();
			if (m_map_XML2localthumimg.find(xml_image_path) == m_map_XML2localthumimg.end())
			{
				return;
			}
			std::string  t_local_img_thum_g = m_map_XML2localthumimg[xml_image_path];
			if (m_map_Local2XML.find(t_local_img_thum_g) == m_map_Local2XML.end())
			{
				return;
			}
			ImageGroupInfo curr_igInfo = m_map_Local2XML[t_local_img_thum_g];

			eobj["photo"] = QString::fromLocal8Bit(xml_image_path.c_str());	           //xml文件中的影像路径
			eobj["photoGroup"] = curr_igInfo.group;          //e.photoGroup  刺控制点影像对应的组
			eobj["u"] = e.pix_x;
			eobj["v"] = e.pix_y;

			sourse_list.push_back(eobj);
		}
		gcpID["sourse_list"] = sourse_list;

		QJsonObject target;

		std::string t_local_img = imgFile.toStdString();

		if (m_map_imageThum.find(t_local_img) == m_map_imageThum.end())
		{
			return;
		}
		std::string  t_local_img_thum = m_map_imageThum[t_local_img];
		if (m_map_Local2XML.find(t_local_img_thum) == m_map_Local2XML.end())
		{
			return;
		}
		std::string t_xml_image_path = m_map_Local2XML[t_local_img_thum].imagpath.toStdString();
		target["photo"] = QString::fromLocal8Bit(t_xml_image_path.c_str());   //此处用xml中的影像信息

		if (m_map_Local2XML.find(imgthumFile.toStdString()) == m_map_Local2XML.end())
		{
			return;
		}
		QString photogroup = m_map_Local2XML[imgthumFile.toStdString()].group;
		image_width = m_map_Local2XML[imgthumFile.toStdString()].width;
		image_height = m_map_Local2XML[imgthumFile.toStdString()].height;

		target["photoGroup"] = photogroup;   // 需要获取影像对应的影像组信息
		gcpID["m_target"] = target;
		source[to_string(p.control_id).c_str()] = gcpID;
		json["source"] = source;

		QByteArray data = QJsonDocument(json).toJson();
		file.write(data);
		file.close();

		m_krGcp->computeLine();
	}
	//解析输出的数据
	std::vector<EpipolarCoefficient> point_ec;
	std::vector<std::vector<QPoint>> EC_lines;
	parseEpipolarCoefficient(point_ec);
	if (point_ec.size() > 0)
	{
		creatLines(point_ec, image_width, image_height, EC_lines);  
		showImageView(imgFile);
		ui.viewer_Image->setLines(EC_lines);
	}
}

void SurveyWidget::showImageView(const QString & _localImage)
{
	QImage img;
	img.load(_localImage);
	ui.viewer_Image->showImage(img, true);
	ui.imageNameLabel->setText(QFileInfo(_localImage).fileName());
	ui.imageSizeLabel->setText(QString("%1x%2").arg(img.width()).arg(img.height()));
}

void SurveyWidget::updateMeasurementPoints(const QString& imgthumFile,const QString &imgFile)
{
	std::vector<ControlPointInfo> points;
	// 查找图片对应的点
	// 通过本地实际的影像路径找到xml文件中对应的路径
	if (m_map_Local2XML.find(imgthumFile.toStdString()) == m_map_Local2XML.end())
	{
		return;
	}
	QString imgFileblock = m_map_Local2XML[imgthumFile.toStdString()].imagpath;
	QPointF stabp;

	for (auto &p : m_surveysInfo.ControlPoints)
	{
		for (auto &match : p.potential_match)
		{
			if (match.image_path == imgthumFile)
			{
				points.emplace_back(ControlPointInfo{ QPoint(match.pix_x, match.pix_y), p.control_name });
				break;
			}
		}
		for (auto &sp : p.photos)
		{
			if (sp.image_path == imgFile)
			{
				stabp = QPointF(sp.pix_x, sp.pix_y);
				break;
			}
		}
	}
	ui.viewer_Image->setControlPoints(points);

	ui.viewer_Image->setCurrentStabPoint(stabp);
}

void SurveyWidget::updateMeasurementPointsTP(const QString& imgthumFile, const QString &imgFile)
{
	std::vector<ControlPointInfo> points;
	// 查找图片对应的点
	// 通过本地实际的影像路径找到xml文件中对应的路径
	if (m_map_Local2XML.find(imgthumFile.toStdString()) == m_map_Local2XML.end())
	{
		return;
	}
	QString imgFileblock = m_map_Local2XML[imgthumFile.toStdString()].imagpath;
	QPointF stabp;

	for (auto &p : m_surveysInfo.TiePoints.tie_points)
	{
		for (auto &match : p.potential_match)
		{
			if (match.image_path == imgthumFile)
			{
				points.emplace_back(ControlPointInfo{ QPoint(match.pix_x, match.pix_y), p.name });
				break;
			}
		}
		for (auto &sp : p.photos)
		{
			if (sp.image_path == imgFile)
			{
				stabp = QPointF(sp.pix_x, sp.pix_y);
				break;
			}
		}
	}
	ui.viewer_Image->setControlPoints(points);
	ui.viewer_Image->setCurrentStabPoint(stabp);
}


void SurveyWidget::setLicenseUrl(const QString& _url)
{
	m_license_url = _url;
}

void SurveyWidget::setLicenseCode(const QString& _code)
{
	m_license_code = _code;
}

bool SurveyWidget::findFiles(const QString& path, QMap<QString, QStringList>& files)
{
	QDir dir(path);
	if (!dir.exists())
		return false;

	QFileInfoList filst;
	QFileInfoList::iterator curFi;
	QStringList pathList;
	pathList << path;

	for (int i = 0; i < pathList.size(); i++) {
		dir.setPath(pathList[i]);
		filst = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot, QDir::DirsFirst);
//		if (m_stop) return false;

		if (filst.count() > 0) {
			curFi = filst.begin();
			while (curFi != filst.end()) {
//				if (m_stop) return false;
				if (curFi->isDir()) {
					if (m_isIncludeSubDir) { // 需要扫描子文件夹
						pathList.push_back(curFi->absoluteFilePath());
					}
				}
				else {
					if (isPictureFile(curFi->absoluteFilePath())) {
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

bool SurveyWidget::isPictureFile(const QString& path)
{
	QString file_suffix;
	QFileInfo fileinfo;
	fileinfo = QFileInfo(path);
	file_suffix = fileinfo.suffix();

	if (!file_suffix.compare("JPG"))
	{
		return true;
	}else if (!file_suffix.compare("jpg"))
	{
		return true;
	}else if (!file_suffix.compare("PNG"))
	{
		return true;
	}else if (!file_suffix.compare("png"))
	{
		return true;
	}
	else if (!file_suffix.compare("BMP"))
	{
		return true;
	}else if (!file_suffix.compare("bmp"))
	{
		return true;
	}else if (!file_suffix.compare("JEPG"))
	{
		return true;
	}else if (!file_suffix.compare("jepg"))
	{
		return true;
	}else if (!file_suffix.compare("TIFF"))
	{
		return true;
	}else if (!file_suffix.compare("tiff"))
	{
		return true;
	}
	else
	{
		return false;
	}



#if 0
	cv::Mat image = cv::imread(path.toStdString());
	if (image.data == NULL)
	{
		return false;
	}
	return true;
#endif
#if 0
	QImage img;
	img.load(path);
	if (!img.isNull())
		return true;

	return false;
#endif
}

void SurveyWidget::updataThumimageMap()
{
	m_thumbnail.clear();
	m_map_imageThum.clear();

	std::map<std::string, std::string> new_map_thumimage;

	std::map<std::string, std::string>::iterator it = m_map_XML2localthumimg.begin();
	while (it != m_map_XML2localthumimg.end())
	{
		m_thumbnail.push_back(it->second);
		it++;
	}	
	for (size_t i = 0; i < m_thumbnail.size(); i++)
	{
		if (m_map_thumimage.find(m_thumbnail[i]) == m_map_thumimage.end())
		{
			return;
		}
		new_map_thumimage.insert(std::make_pair(m_thumbnail[i], m_map_thumimage[m_thumbnail[i]]));
		m_map_imageThum.insert(std::make_pair(m_map_thumimage[m_thumbnail[i]], m_thumbnail[i]));
	}
	m_map_thumimage.clear();
	m_map_thumimage = new_map_thumimage;
}

void SurveyWidget::setIsIncludeSubDir(const bool& _bool)
{
	m_isIncludeSubDir = _bool;
}

//清空当前作业数据
void SurveyWidget::clearSurveyData()
{
	m_surveysInfo.ControlPoints.clear();
	m_surveysInfo.SpatialReferenceSystems.clear();
	m_surveysInfo.TiePoints.tie_points.clear();
}


void SurveyWidget::on_btnAdd_clicked()  // 目前只针对可以添加连接点
{
	SurveyInfoWindow *ptr = new SurveyInfoWindow(m_at_taskID);
	Qt::WindowFlags flags = ptr->windowFlags();
	ptr->setWindowFlags(flags | Qt::MSWindowsFixedSizeDialogHint);

	//获取m_surveysInfo 信息中的连接点数据，确认连接点个数，自动生成连接点名称
	int tpID = 0;
	int tpnum = m_surveysInfo.TiePoints.tie_points.size();
	if (tpnum == 0)
	{
		tpID = 0;
	}
	else
	{
		tpID = m_surveysInfo.TiePoints.tie_points[tpnum - 1].id +1;
	}

//	ptr->setTiePointNum(tpnum);
	ptr->setTiePointMaxID(tpID);
	ptr->initUi(true);

	int ref = ptr->exec();

	if (ref ==QDialog::Accepted)
	{
		QString ptName = ptr->getPointName();
		int ptType = ptr->getCurrentIndex();  //判断是添加连接点还是 控制点  ，当前只有连接点
		QString ptTyoeName = ptr->getCurrentText();

		SURVEYS_INFO::surveys_tiePnt temp;
		temp.name = ptName;
		temp.id = tpID;
		temp.type= "User";
		m_surveysInfo.TiePoints.tie_points.push_back(temp);
	}
	delete ptr;

	showControlPointList();
	
}

void SurveyWidget::SaveCurrTpInfotoJson(const SURVEYS_INFO::surveys_tiePnt& _point)
{
#if 1
	QString outdir = m_rootDir + "/input";
	QDir().mkpath(outdir);
	QFile file(m_update_photos_tp_input);
	if (!file.open(QIODevice::WriteOnly))
	{
		return;
	}
	QJsonObject json;

	QJsonObject point;
	point["name"] = _point.name;
	json["point"] = point;   //point

	QJsonArray photolist;
	for (auto & e : _point.photos)
	{
		QJsonObject pobj;

		std::string local_img = e.image_path.toStdString();
		if (m_map_imageThum.find(local_img) == m_map_imageThum.end())
		{
			return ;
		}
		std::string  local_img_thum = m_map_imageThum[local_img];
		if (m_map_Local2XML.find(local_img_thum) == m_map_Local2XML.end())
		{
			return ;
		}
		std::string  xml_image_path = m_map_Local2XML[local_img_thum].imagpath.toStdString();
		pobj["photo"] = QString::fromLocal8Bit(xml_image_path.c_str());
		pobj["photoGroup"] = e.photoGroup;
		pobj["u"] = e.pix_x;
		pobj["v"] = e.pix_y;

		photolist.push_back(pobj);
	}

	json["photoList"] = photolist;  //pointList
	QByteArray data = QJsonDocument(json).toJson();
	file.write(data);
	file.close();

	if (QFile::exists(m_update_photos_tp_input))
		cout << "file is exist!" << endl;

#endif
}

bool SurveyWidget::UpdatePhotoTPOutputJson()
{

	//读入所有内容
	QFile file(m_update_photos_tp_output);
	file.open(QIODevice::ReadOnly);
	auto data = file.readAll();
	file.close();

	//解析
	QJsonParseError parseError;
	QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

	if (parseError.error != QJsonParseError::NoError)
	{
		qDebug() << "读取update_photos_tp_output文件失败";
		return false;
	}

	QJsonObject obj = doc.object();

	m_surveysInfo.TiePoints.srs_id = obj["SRS"].toInt();
	QString curr_tpName = obj["name"].toString();

	QString tp_x = QString::number(obj["x"].toDouble(),'f',8);
	QString tp_y = QString::number(obj["y"].toDouble(),'f',8);
	QString tp_z = QString::number(obj["z"].toDouble(),'f',3);

	for (auto &tp : m_surveysInfo.TiePoints.tie_points)
	{
		if (tp.name == curr_tpName)
		{
			tp.potential_match.clear();
			tp.coordinate.x = tp_x;
			tp.coordinate.y = tp_y;
			tp.coordinate.z = tp_z;
			QJsonObject photo_list = obj["photo_list"].toObject();
			auto keys = photo_list.keys();
			for (auto &k : keys)
			{
				QJsonArray tpList = photo_list[k].toArray();
				for (auto &tpimage : tpList)
				{
					auto tpimgkey = tpimage.toObject().keys();
					for (auto tpk : tpimgkey)
					{
						SURVEYS_INFO::imageMeasurement curr_im;
						curr_im.image_id = -1;

						//潜在匹配项->缩略图
						std::string t_local_img = tpk.toStdString();
						if (m_map_XML2localthumimg.find(t_local_img) == m_map_XML2localthumimg.end())
						{
							continue;
						}
						std::string  t_local_img_thum = m_map_XML2localthumimg[t_local_img];
						curr_im.image_path = QString::fromLocal8Bit(t_local_img_thum.c_str());
						QJsonObject uv = tpimage.toObject()[tpk].toObject();

						curr_im.pix_x = uv["u"].toDouble();
						curr_im.pix_y = uv["v"].toDouble();

						tp.potential_match.push_back(curr_im);
					}

				}
			}
			break;
		}
	}
	return true;
}

void SurveyWidget::on_btnDelete_clicked()
{
	QTreeWidgetItem *item = ui.controlPointListWidget->currentItem();

	if (!item) { return; }

	if (item->type()==enTypeTP)
	{
		short tpid = item->data(0, Qt::UserRole).toInt();
		// 删除对应连接点
		for (auto &tp : m_surveysInfo.TiePoints.tie_points)
		{
			if (tp.id == tpid)
			{
 				SURVEYS_INFO::surveys_tiePnt t = tp;
 				m_surveysInfo.TiePoints.tie_points.removeOne(t);
				if (item->parent()==nullptr)
				{
					delete ui.controlPointListWidget->takeTopLevelItem(ui.controlPointListWidget->currentIndex().row());
				}
				else
				{
					delete item->parent()->takeChild(ui.controlPointListWidget->currentIndex().row());
				}
				
			}
		}
	}
}

void SurveyWidget::setBtnDeleteEnable(QTreeWidgetItem * item, int)
{
	if (item->type()==enTypeTP &&item->parent() == nullptr)
	{
		ui.btnDelete->setEnabled(true);
	}
	else if (item->type() == enTypeGCP &&item->parent() == nullptr)
	{
		ui.btnDelete->setEnabled(false);
	}
	else
	{
		ui.btnDelete->setEnabled(false);
	}
}