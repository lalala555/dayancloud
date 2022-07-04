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
	QString imagePath;  //Ӱ��·��
	double u; //��������x
	double v; //��������y
};

struct ImageGroupInfo
{
	QString group;  //Ӱ������Ϣ
	QString imagpath; //Ӱ��·��
	QString PhotoId;  
	ushort width ;   //Ӱ���  ��ͬ���Ӱ��Ӱ��size���ܲ�ͬ
	ushort height;   //Ӱ���
};

struct	EpipolarCoefficient  //����ϵ��
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

	//  ��ȡ��ص�����
	SURVEYS_INFO::surveys_data  getSurveysData();
	//  ����ͼ������
	void loadImageData();
	void setImagePosPath(const QString &_filePath);
	void setSiftPhotoInput(const QString &_filePath);
	void setComputeLineInput(const QString &_filePath);
	void setSiftPhotoOutput(const QString &_filePath);
	void setComputeLineOutput(const QString &_filePath);

	void setAtTaskID(const t_taskid &_taskID);
	void initCalcParam(const at_para_calc::at_param_json &_param);

	// ��ս���
	void cleanUI();

	//�������Ƶ�json����
	void outPutControlJson(); 

	//����json�ļ��������·��
	void createJsonFile();

	//����Ǳ��ƥ��json����
	void ParsesiftPhotoOutputJson();

	//����Ǳ��ƥ������
	bool UpdatePhotoOutputJson();

	//�����ӵ�����
	bool UpdatePhotoTPOutputJson();

	void setReJson(const QString& _filePath);
	// ���ÿ��Ƶ�����
	void setControlPint(const SURVEYS_INFO::surveys_data& ctrlPointInfo);
	// ����Ӱ��·��
	void setImagePath(const QString& imagePath);
	//������������
	void setBlockInfo( BLOCK_TASK::BlockExchange* blockExchange);

	//��������ͼ����·��
	void setThumimagePath(const QString &_dirPath);

	//����������
	void setSenceName(const QString &_dirPath);

	//�������ɿ��Ƶ�surveyPoint XML�ļ�����·��
	void setSurveyPointXMLPath(const QString &_dirPath);

	//����python ����
	bool initPythonCall();

	//���õ�ǰ��������ļ���Ŀ¼
	void setRootDir(const QString& _dirPath);

	KrGcp *getKrGcp();
	
	//�ͷ� m_krGcp �ڴ�
	void deleteKrGcp();

	//��յ�ǰ��ҵ����
	void clearSurveyData();

	// ���ش̵���Ϣ��GCP�ļ���
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
    void on_btnSubmitTask_clicked(); //�����ύ

	void on_allImgButton_clicked();
	void on_matchButton_clicked();
	void on_selectedButton_clicked();

	void on_btnAdd_clicked();

	void on_btnDelete_clicked();

	void setBtnDeleteEnable(QTreeWidgetItem *, int);

	void onRecvResponse(QSharedPointer<ResponseHead> response);

private:
	// ������ʾ��ͼƬ�б�
	void setImages(const std::vector<std::string> & imgs);

	// ��ȡͼƬ�б�
	void getImages(const QString &dirpath, std::list<std::string> & imgs);

	//��������ͼ
	int createthumbnail();

	// ��ʼ������
	void initUI();

	// ��ʼ���źŲ�
	void initSignalSlot();

	//��ʼ��ԭʼͼ
	void initFirstImage();

	//��ʼ�����Ƶ�������ʾ
	void initCPEpipolarLine(const SURVEYS_INFO::surveys_point& p, const QString& imgFile, const QString& imgthumFile);

	//���Ƶ��б�
	void showControlPointList();

	//��������Ӱ���������Ӧ�����ļ�Ӱ��ӳ���ϵ
	void CreateLocalToXML();

	//����ϵ������
	void parseEpipolarCoefficient(std::vector <EpipolarCoefficient>& _eps);

	//creatlines
	void creatLines(const std::vector <EpipolarCoefficient>& _eps,const ushort& _width,const ushort &_height, std::vector<std::vector<QPoint>> & _lines);

	//������Ƶ�Ĵ̵���Ϣ
	void SaveCurrGcpInfoToJson(const SURVEYS_INFO::surveys_point& _point);

	//���浱ǰ���ӵ�Ĵ̵���Ϣ
	void SaveCurrTpInfotoJson(const SURVEYS_INFO::surveys_tiePnt &_tpoint);

	// ����GCP�ɹ��ļ�
	bool saveXmlToFile();

	//����������� imgFile����Ӱ��·��
	void calcEpipolarLineData(const SURVEYS_INFO::surveys_point& p, const QString &imgFile, const QString& imgthumFile);
	void calcEpipolarLineDataTP(const SURVEYS_INFO::surveys_tiePnt& p, const QString &imgFile, const QString& imgthumFile);

	//��Ӱ�񴰿�չʾ��ӦӰ��
	void showImageView(const QString & _localImage);

	//Ԥ��&�̵�
	void updateMeasurementPoints(const QString& imgthumFile, const QString &imgFile);
	void updateMeasurementPointsTP(const QString& imgthumFile, const QString &imgFile);

	//�ļ�Ƕ�ױ���
	bool findFiles(const QString& path, QMap<QString, QStringList>& files);

	//����Ƿ���Ӱ���ļ�
	bool isPictureFile(const QString& path);

	//��������ͼ���ݣ������ֿ��ɸѡ����Ҫ������ͼ�ļ�
	void updataThumimageMap();

protected:
	virtual void wheelEvent(QWheelEvent *event) override;

private:
    Ui::Widget ui;

	// ȫ����Ƭ
	std::map<std::string, ImageInfo> m_allImage;

	// Ǳ��ƥ����Ƭ
	std::map<std::string, ImageInfo> m_matchImage;

	// �Ѵ̵���Ƭ
	std::map<std::string, ImageInfo> m_selectedImage;

	// ԭʼӰ���б� 
	std::list<std::string>  m_images;

	// Ӱ������ͼ
	std::vector<std::string> m_thumbnail;   //ɸѡ��

	// ��ǰ��������ļ���Ŀ¼
	QString m_rootDir;

	//����ͼ��ԭʼӰ���map
	std::map<std::string, std::string> m_map_thumimage;

	//ԭʼӰ��������ͼ��map 
	std::map<std::string, std::string> m_map_imageThum;

	//��������Ӱ���������ļ���ӦӰ���map 
	std::map<std::string, ImageGroupInfo> m_map_Local2XML;

	//�����е�Ӱ���뱾������ͼ·����map 
	std::map<std::string, std::string> m_map_XML2localthumimg;
	
	//����ص�����
	SURVEYS_INFO::surveys_data m_surveysInfo;

	//������Ϣ����
	BLOCK_TASK::BlockExchange* m_blockExchange;

	//Ӱ��·��
	QString m_imagePath;

	//����ͼ���ɸ�·��
	QString m_thumbnail_path;

	//�������Ƶ㵼��·��
	QString m_surveyPointXml_path;

	//listwidget �ؼ���ʼ��
	bool m_initListWidget;

	//At ����ID
	t_taskid m_at_taskID;


	//������
	QString m_SenceName;

	//������� 
	at_para_calc::at_param_json m_calc_param;

	//��������json·��
	QString m_re_json;
		
	QString m_imagePos_path; //����xml�ļ�������json���� ·�� 
	QString m_control_point; //sift_photo_input ����Ŀ��Ƶ�·��  
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

    // Ǳ��ƥ����Ƭ
    std::vector<std::string> m_matchThumbnail;

    // �̵���Ƭ
    std::vector<std::string> m_surveyThumbnail;

    // ��ǰѡ��İ�ť
    int m_currentSelectedBtnIndex;

	// �Ƿ�������ļ���
	bool m_isIncludeSubDir;

	bool m_blockMergeStateSubmit;  //�ϲ�����


	QList<JobState::DetailInfo> m_blockLisFrames;   //����֡
	QList<JobState::DetailInfo> m_failListFrames;  //ʧ�ܿ���
	QList<JobState::DetailInfo> m_mergeFrames;   //�ϲ�֡
};
