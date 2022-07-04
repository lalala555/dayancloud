#pragma once

#include <map>
#include <set>

#include <QtWidgets/QWidget>
#include "ui_SurveyWidget.h"
#include <QFileDialog>
#include <QWheelEvent>
#include "kernel/render_struct.h"
#include "HttpCommand/HttpCommon.h"
#include "Views/NewJobs/surveyinfowindow.h"

class KrGcp;
class SurveyInfoWindow;
struct ImageInfo
{
	QString imagePath;  //影像路径
	double u; //像素坐标x
	double v; //像素坐标y
};

struct ImageGroupInfo
{
	QString group;  //影像组信息
	QString imagpath; //影像路径
	QString PhotoId;  
	ushort width ;   //影像宽  不同组的影像影像size可能不同
	ushort height;   //影像高
};

struct	EpipolarCoefficient  //核线系数
{
	double a;
	double b;
	double c;
	double d;
	QString mode;
	double min;
	double max;
};

class SurveyWidget : public QWidget
{
    Q_OBJECT

public:
    enum BtnIndex
    {
        enIndexAll,
        enIndexMatched,
        enIndexSurvey,

        enAll
    };

public:
	SurveyWidget(QWidget *parent = Q_NULLPTR);

	~SurveyWidget();
	void run();

	//  获取像控点数据
	SURVEYS_INFO::surveys_data  getSurveysData();
	//  加载图像数据
	void loadImageData();
	void setImagePosPath(const QString &_filePath);
	void setSiftPhotoInput(const QString &_filePath);
	void setComputeLineInput(const QString &_filePath);
	void setSiftPhotoOutput(const QString &_filePath);
	void setComputeLineOutput(const QString &_filePath);

	void setAtTaskID(const t_taskid &_taskID);
	void initCalcParam(const at_para_calc::at_param_json &_param);

	// 清空界面
	void cleanUI();

	//创建控制点json数据
	void outPutControlJson(); 

	//创建json文件输入输出路径
	void createJsonFile();

	//解析潜在匹配json数据
	void ParsesiftPhotoOutputJson();

	//更新潜在匹配数据
	bool UpdatePhotoOutputJson();

	//更连接点数据
	bool UpdatePhotoTPOutputJson();

	void setReJson(const QString& _filePath);
	// 设置控制点数据
	void setControlPint(const SURVEYS_INFO::surveys_data& ctrlPointInfo);
	// 设置影像路径
	void setImagePath(const QString& imagePath);
	//设置区块数据
	void setBlockInfo( BLOCK_TASK::BlockExchange* blockExchange);

	//设置缩略图生成路径
	void setThumimagePath(const QString &_dirPath);

	//设置任务名
	void setSenceName(const QString &_dirPath);

	//设置生成控制点surveyPoint XML文件导出路径
	void setSurveyPointXMLPath(const QString &_dirPath);

	//设置python 调用
	bool initPythonCall();

	//设置当前任务过程文件根目录
	void setRootDir(const QString& _dirPath);

	KrGcp *getKrGcp();
	
	//释放 m_krGcp 内存
	void deleteKrGcp();

	//清空当前作业数据
	void clearSurveyData();

	// 加载刺点信息到GCP文件中
	bool loadXmlFileToCP();	

	void setLicenseUrl(const QString& _url);

	void setLicenseCode(const QString& _code);

	void setIsIncludeSubDir(const bool& _bool);
	

signals:
	void initSurveyWidgetBegin();
	void createthumbnailFinished(int);
	void initSurveyModulefinish(int);

private slots:
	void initFirst();
    void selectedImageChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void showImageList(const std::vector<std::string>& thumbnail);
    void onListWidgetValueChanged();
	void onStabPoints(const QPointF &);
	void onTreeListItemClicked(QTreeWidgetItem *, int);
    void on_btnSubmitTask_clicked(); //重新提交

	void on_allImgButton_clicked();
	void on_matchButton_clicked();
	void on_selectedButton_clicked();

	void on_btnAdd_clicked();

	void on_btnDelete_clicked();

	void setBtnDeleteEnable(QTreeWidgetItem *, int);

	void onRecvResponse(QSharedPointer<ResponseHead> response);

private:
	// 设置显示的图片列表
	void setImages(const std::vector<std::string> & imgs);

	// 获取图片列表
	void getImages(const QString &dirpath, std::list<std::string> & imgs);

	//创建缩略图
	int createthumbnail();

	// 初始化界面
	void initUI();

	// 初始化信号槽
	void initSignalSlot();

	//初始化原始图
	void initFirstImage();

	//初始化控制点数据显示
	void initCPEpipolarLine(const SURVEYS_INFO::surveys_point& p, const QString& imgFile, const QString& imgthumFile);

	//控制点列表
	void showControlPointList();

	//创建本地影像数据与对应区块文件影像映射关系
	void CreateLocalToXML();

	//核线系数解析
	void parseEpipolarCoefficient(std::vector <EpipolarCoefficient>& _eps);

	//creatlines
	void creatLines(const std::vector <EpipolarCoefficient>& _eps,const ushort& _width,const ushort &_height, std::vector<std::vector<QPoint>> & _lines);

	//保存控制点的刺点信息
	void SaveCurrGcpInfoToJson(const SURVEYS_INFO::surveys_point& _point);

	//保存当前连接点的刺点信息
	void SaveCurrTpInfotoJson(const SURVEYS_INFO::surveys_tiePnt &_tpoint);

	// 保存GCP成果文件
	bool saveXmlToFile();

	//计算核线数据 imgFile本地影像路径
	void calcEpipolarLineData(const SURVEYS_INFO::surveys_point& p, const QString &imgFile, const QString& imgthumFile);
	void calcEpipolarLineDataTP(const SURVEYS_INFO::surveys_tiePnt& p, const QString &imgFile, const QString& imgthumFile);

	//主影像窗口展示对应影像
	void showImageView(const QString & _localImage);

	//预测&刺点
	void updateMeasurementPoints(const QString& imgthumFile, const QString &imgFile);
	void updateMeasurementPointsTP(const QString& imgthumFile, const QString &imgFile);

	//文件嵌套遍历
	bool findFiles(const QString& path, QMap<QString, QStringList>& files);

	//检查是否是影像文件
	bool isPictureFile(const QString& path);

	//更新缩略图数据，空三分块后筛选不必要的缩略图文件
	void updataThumimageMap();

protected:
	virtual void wheelEvent(QWheelEvent *event) override;

private:
    Ui::Widget ui;

	// 全部照片
	std::map<std::string, ImageInfo> m_allImage;

	// 潜在匹配照片
	std::map<std::string, ImageInfo> m_matchImage;

	// 已刺点照片
	std::map<std::string, ImageInfo> m_selectedImage;

	// 原始影像列表 
	std::list<std::string>  m_images;

	// 影像缩略图
	std::vector<std::string> m_thumbnail;   //筛选过

	// 当前任务过程文件根目录
	QString m_rootDir;

	//缩略图与原始影像的map
	std::map<std::string, std::string> m_map_thumimage;

	//原始影像与缩略图的map 
	std::map<std::string, std::string> m_map_imageThum;

	//本地缩略影像与区块文件对应影像的map 
	std::map<std::string, ImageGroupInfo> m_map_Local2XML;

	//区块中的影像与本地缩略图路径的map 
	std::map<std::string, std::string> m_map_XML2localthumimg;
	
	//刺像控点数据
	SURVEYS_INFO::surveys_data m_surveysInfo;

	//区块信息数据
	BLOCK_TASK::BlockExchange* m_blockExchange;

	//影像路径
	QString m_imagePath;

	//缩略图生成根路径
	QString m_thumbnail_path;

	//测量控制点导出路径
	QString m_surveyPointXml_path;

	//listwidget 控件初始化
	bool m_initListWidget;

	//At 任务ID
	t_taskid m_at_taskID;


	//任务名
	QString m_SenceName;

	//计算参数 
	at_para_calc::at_param_json m_calc_param;

	//参数返回json路径
	QString m_re_json;
		
	QString m_imagePos_path; //区块xml文件解析的json数据 路径 
	QString m_control_point; //sift_photo_input 输入的控制点路径  
	QString m_compute_line_input;
	QString m_sift_photo_output;
	QString m_compute_line_output;

	QString m_update_photos_input;
	QString m_update_photos_output;

	QString m_update_photos_tp_input;
	QString m_update_photos_tp_output;

	QString m_license_url;
	QString m_license_code;
	QString m_validate_return_output;


	KrGcp *m_krGcp;	

    // 潜在匹配照片
    std::vector<std::string> m_matchThumbnail;

    // 刺点照片
    std::vector<std::string> m_surveyThumbnail;

    // 当前选择的按钮
    int m_currentSelectedBtnIndex;

	// 是否包含子文件夹
	bool m_isIncludeSubDir;

	bool m_blockMergeStateSubmit;  //合并重提


	QList<JobState::DetailInfo> m_blockLisFrames;   //所有帧
	QList<JobState::DetailInfo> m_failListFrames;  //失败空三
	QList<JobState::DetailInfo> m_mergeFrames;   //合并帧
};
