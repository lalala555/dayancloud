#include "stdafx.h"
#include "NetSetting.h"
#include "Config/UserProfile/WSConfig.h"
#include "MessageBoxRestart.h"

NetSetting::NetSetting(IResponseSink* sink, QWidget *parent)
    : QWidget(parent)
	, m_transCheckTimer(nullptr)
	, m_upload_tranChecksize(1024 * 1024 * 20)
	, m_pairEngineUploadSpeed{ "",0 }
    , m_pairEngineDownloadSpeed{ "",0 }	
    , m_pSink(sink)
{
    ui.setupUi(this);

    initUI();
    initDialog();
}

NetSetting::~NetSetting()
{

}

void NetSetting::initDialog()
{
    // 加载传输引擎
    loadEngineList();

    QString engine = ui.comboBox_transfermode->currentText();
    updateTransferRoute(engine);
    addEngineInfoTableWidget(engine);

    updateLimitNetwork();

    // 加载其他设置
    loadOtherTransSetting();

    installWidgetOpt();
}

void NetSetting::finishPingIP()
{
}

void NetSetting::updateNetworkData()
{
    QString transEngine = ui.comboBox_transfermode->currentText();
}

void NetSetting::initUI()
{
    connect(ui.comboBox_transfermode, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotTransferModeChange(QString)));
    connect(ui.save_network_setting, SIGNAL(clicked()), this, SLOT(slotSaveNetworkSetting()));
    connect(ui.btnLimitUpload, SIGNAL(clicked()), this, SLOT(slotClickedLimitUpload()));
    connect(ui.btnLimitDownload, SIGNAL(clicked()), this, SLOT(slotClickedLimitDownload()));
    connect(ui.spinDownCount, SIGNAL(valueChanged(int)), this, SLOT(slotChangedDownloadCount(int)));
    connect(ui.spinUpCount, SIGNAL(valueChanged(int)), this, SLOT(slotChangedUploadCount(int)));
    connect(ui.lineEdit_upload, SIGNAL(editingFinished()), this, SLOT(slotEditFinishLimitUpload()));
    connect(ui.lineEdit_download, SIGNAL(editingFinished()), this, SLOT(slotEditFinishLimitDownload()));

    QUIntValidator *intValidator = new QUIntValidator(0, 9999999);
    ui.lineEdit_upload->setValidator(intValidator);
    ui.lineEdit_download->setValidator(intValidator);

    setShowEditUpload(false);
    setShowEditDownload(false);

    ui.comboBoxTransCheck2->setToolTip(tr("传输引擎内置哈希校验规则"));
    ui.comboBoxTransCheck2->setItemData(0, tr("校验大小+时间,速度最快"), Qt::ToolTipRole);
    ui.comboBoxTransCheck2->setItemData(1, tr("传输过程中进行校验,速度快"), Qt::ToolTipRole);
    ui.comboBoxTransCheck2->setItemData(2, tr("严格校验整体文件内容,速度较慢"), Qt::ToolTipRole);

    // 隐藏校验
    ui.widget_check->hide();
    // 隐藏加密
    ui.widget_encrypt->hide();

#ifdef FOXRENDERFARM
	ui.label_11->setFixedWidth(120);
	ui.label_49->setFixedWidth(120);
	ui.label_50->setFixedWidth(120);
	ui.comboBoxTransCheck2->setFixedWidth(120);
#endif

#if USE_SCROLL_PAGE
#else
    ui.groupBox->setTitle("");
#endif
}

void NetSetting::loadEngineList()
{
	//std::wstring transEngine = ChineseCode::ANSIToUnicode(CLocalSettingMgr::get().getTransEngine());
    // 传输方式
	ui.comboBox_transfermode->clear();
	QString transEngine = LocalSetting::getInstance()->getTransEngine();

	QString defaultEngine = TransConfig::getInstance()->GetUserDefaultEngine();
	QList<QString> lstEngine = TransConfig::getInstance()->GetEngineList();

	ui.comboBox_transfermode->blockSignals(true);   // 锁定信号
	auto it = lstEngine.begin();
	int i = 0, index = 0, defaultIndex = 0;
	bool find = false;
	for(it; it != lstEngine.end(); ++it, i++) {
		if(transEngine == *it) {
			index = i;
			find = true;
		}
		if(defaultEngine == *it) {
			defaultIndex = i;
		}
		ui.comboBox_transfermode->addItem(*it);
	}
	ui.comboBox_transfermode->blockSignals(false);  // 锁定信号
	if(find) {
		ui.comboBox_transfermode->setCurrentIndex(index);
	} else {
		ui.comboBox_transfermode->setCurrentIndex(defaultIndex);
	}

	m_preTransEngine = ui.comboBox_transfermode->currentText();
}

void NetSetting::addEngineInfoTableWidget(const QString& engine)
{
    //std::wstring transEngine = engine.toStdWString();
    QString strHost = LocalSetting::getInstance()->getNetWorkName();

    int nsel = 0;
    for(int i = 0; i < ui.comboBox_select->count(); i++) {
        if(ui.comboBox_select->itemText(i) == strHost) {
            nsel = i;
        }
    }
    ui.comboBox_select->setCurrentIndex(nsel);

	auto config = TransConfig::getInstance()->getTransConfig();
	int size = 0;
	for(auto i = config.engines.begin() ;i!= config.engines.end();i++) {
		if(i->engine_name == engine) {
			size = i->lines.size();
			break;
		}
	}
    this->updateNetworkData();
}

void NetSetting::updateTransferRoute(const QString & engine)
{
    if (engine.toLower() == "raysync") {
        ui.widget_protocol->show();
        ui.widget_check2->show();
    }
    else {
        ui.widget_protocol->hide();
        ui.widget_check2->hide();
    }

	QString hostname = LocalSetting::getInstance()->getNetWorkName();
	if(hostname.isEmpty()) {
		hostname = TransConfig::getInstance()->GetDefaultNetServerName(engine);
	}

	int nsel = 0;
	int index = 0;
	ui.comboBox_select->clear();

	auto config = TransConfig::getInstance()->getTransConfig();
	for(auto i = config.engines.begin();i!=config.engines.end();i++ ) {
		if(i->engine_name == engine) {
			for(auto line=i->lines.begin();line != i->lines.end();line++) {
				ui.comboBox_select->addItem(line->name);
				if(line->name == hostname) {
					nsel = index;
				}
				index++;
			}

		}
	}
    ui.comboBox_select->setCurrentIndex(nsel);
}

void NetSetting::updateLimitNetwork()
{
    int nLimitUpload = LocalSetting::getInstance()->getLimitUpload();
    setShowEditUpload(nLimitUpload > 0 ? true : false);
    ui.lineEdit_upload->setText(QString::number(nLimitUpload));

    int nLimitDownload = LocalSetting::getInstance()->getLimitDownload();
    setShowEditDownload(nLimitDownload > 0 ? true : false);
    ui.lineEdit_download->setText(QString::number(nLimitDownload));

    int nLimitUpCount = LocalSetting::getInstance()->getUploadThreadCount();
    if (nLimitUpCount <= 0 || nLimitUpCount > UPLOAD_THREAD_MAX_COUNT) nLimitUpCount = 2;
    ui.spinUpCount->setValue(nLimitUpCount);

    int nLimitDownCount = LocalSetting::getInstance()->getDownloadThreadCount();
    if (nLimitDownCount <= 0 || nLimitDownCount > DOWNLOAD_THREAD_MAX_COUNT) nLimitDownCount = 2;
    ui.spinDownCount->setValue(nLimitDownCount);
}

void NetSetting::setShowEditUpload(bool bshow)
{
    ui.lineEdit_upload->setVisible(bshow);
    ui.label_unit_up->setVisible(bshow);
    ui.label_no_limit_up->setVisible(!bshow);
    ui.btnLimitUpload->setVisible(!bshow);
    emit noticeWidgetOpt();
}

void NetSetting::setShowEditDownload(bool bshow)
{
    ui.lineEdit_download->setVisible(bshow);
    ui.label_unit_down->setVisible(bshow);
    ui.label_no_limit_down->setVisible(!bshow);
    ui.btnLimitDownload->setVisible(!bshow);
    emit noticeWidgetOpt();
}

void NetSetting::saveNetworkName(const QString& engine, const QString& serverName)
{
	auto config = TransConfig::getInstance()->getTransConfig();
		for(auto i = config.engines.begin();i!=config.engines.end();i++ ) {
			if(i->engine_name == engine) {
				for(auto line=i->lines.begin();line != i->lines.end();line++) {
				if(line->name == serverName) {
                    LocalSetting::getInstance()->setNetWorkName(serverName);
                    LocalSetting::getInstance()->setNetChannel("transmit_server");
					break;
				}
			}
		}
	}
}

void NetSetting::loadOtherTransSetting()
{
    // 1.传输校验
    int transtype = LocalSetting::getInstance()->getFileCheckMode();
    ui.comboBoxTransCheck->setCurrentIndex(transtype);

    // 2.传输加密
    bool isCompression = LocalSetting::getInstance()->getTransEncrypt();
    ui.checkBox_compress->setChecked(isCompression);

    // 3.内置校验
    int checkMode = LocalSetting::getInstance()->getSparseCheckMode();
    ui.comboBoxTransCheck2->setCurrentIndex(checkMode);

    // 4.协议设置
    int protoType = LocalSetting::getInstance()->getProtocolType();
    ui.comboBoxProtocol->setCurrentIndex(protoType);
}

void NetSetting::slotSaveNetworkSetting()
{
    QString serverName = ui.comboBox_select->currentText();
    QString strTransEngine = ui.comboBox_transfermode->currentText();
    QString transEngine = strTransEngine;
    bool transEncrypt = ui.checkBox_compress->isChecked();
    bool localEncrypt = LocalSetting::getInstance()->getTransEncrypt();

    int transCheckType = ui.comboBoxTransCheck->currentIndex();
    int localCheckType = LocalSetting::getInstance()->getFileCheckMode();

    int transProtocolType = ui.comboBoxProtocol->currentIndex();
    int localProtocolType = LocalSetting::getInstance()->getProtocolType();

    int transHashCheckMode = ui.comboBoxTransCheck2->currentIndex();
    int localHashCheckMode = LocalSetting::getInstance()->getSparseCheckMode();

    if(m_preTransEngine != strTransEngine 
        || transCheckType != localCheckType
        || transEncrypt != localEncrypt
        || transHashCheckMode != localHashCheckMode
        || transProtocolType != localProtocolType) {
            LocalSetting::getInstance()->setNetWorkName("");
            LocalSetting::getInstance()->setNetChannel("");

            QPointer<MessageBoxRestart> box(new MessageBoxRestart);
            int nRet = box->exec();
            if(nRet == enIDOK) {
                LocalSetting::getInstance()->setTransEngine(transEngine);
                this->saveNetworkName(strTransEngine, serverName);
                LocalSetting::getInstance()->setTransEncrypt(transEncrypt);
                LocalSetting::getInstance()->setFileCheckMode(transCheckType);
                LocalSetting::getInstance()->setSparseCheckMode(transHashCheckMode);
                LocalSetting::getInstance()->setProtocolType(transProtocolType);
                CGlobalHelp::Response(UM_LOGOUT, 0);
            }
    } else {
        this->saveNetworkName(strTransEngine, serverName);
        CGlobalHelp::Response(UM_CHANGE_TRANS_SERVER, 0);
        Util::ShowMessageBoxInfo(QObject::tr("设置生效"));
    }
}

void NetSetting::slotTransferModeChange(const QString & text)
{
    QString transEngine = text;
    if(transEngine.isEmpty()) return;

    this->addEngineInfoTableWidget(text);

    updateTransferRoute(text);

    emit noticeWidgetOpt();
}

void NetSetting::slotDBClickedNetworkWidget(int row, int column)
{
	QString  transEngine = ui.comboBox_transfermode->currentText();

	auto config = TransConfig::getInstance()->getTransConfig();
	int size = 0;
	for(auto i = config.engines.begin();i!=config.engines.end();i++ ) {
		if(i->engine_name == transEngine) {
			size = i->lines.size();
			break;
		}
	}
	if(row < size) {
		ui.comboBox_select->setCurrentIndex(row);
	}
	//TODO
    emit noticeWidgetOpt();
}

void NetSetting::slotClickedLimitUpload()
{
    setShowEditUpload(true);
}

void NetSetting::slotClickedLimitDownload()
{
    setShowEditDownload(true);
}

void NetSetting::slotEditFinishLimitUpload()
{
    int nlimit = ui.lineEdit_upload->text().toInt() ;
    if(nlimit <= 0) {
        setShowEditUpload(false);
    } else {

        CGlobalHelp::Response(UM_SPEED_LIMIT, EN_LIMIT_UPLOAD_SPEED, nlimit);
    }
    LocalSetting::getInstance()->setLimitUpload(nlimit);
    emit noticeWidgetOpt();
}

void NetSetting::slotEditFinishLimitDownload()
{
    int nlimit = ui.lineEdit_download->text().toInt() ;
    if(nlimit <= 0) {

        setShowEditDownload(false);
    } else {

        CGlobalHelp::Response(UM_SPEED_LIMIT, EN_LIMIT_DOWNLOAD_SPEED, nlimit);
    }
    LocalSetting::getInstance()->setLimitDownload(nlimit);
    emit noticeWidgetOpt();
}

void NetSetting::setSelectStyle(bool isSelect)
{
    if(isSelect){
        ui.groupBox->setStyleSheet("QGroupBox{border-width:1px; border-style:solid; border-color:#675FFE;}"
            "QGroupBox:title{color:#675FFE;}");
    } else {
        ui.groupBox->setStyleSheet("QGroupBox{border-width:1px;border-style:solid;border-color:#333333;}"
            "QGroupBox:title{color:#675FFE;}");
    }
}

void NetSetting::slotUpdateWidgetRect(const QRect& topWidgetRect)
{
    QRect rootRct = this->geometry();
    rootRct.setY(rootRct.y() + topWidgetRect.height());
    this->setGeometry(rootRct);
}

void NetSetting::installWidgetOpt()
{
    connect(ui.save_network_setting, SIGNAL(clicked()), this, SIGNAL(noticeWidgetOpt()));
    connect(ui.btnLimitUpload, SIGNAL(clicked()), this, SIGNAL(noticeWidgetOpt()));
    connect(ui.btnLimitDownload, SIGNAL(clicked()), this, SIGNAL(noticeWidgetOpt()));

    connect(ui.lineEdit_upload, SIGNAL(editingFinished()), this, SIGNAL(noticeWidgetOpt()));
    connect(ui.lineEdit_download, SIGNAL(editingFinished()), this, SIGNAL(noticeWidgetOpt()));
    connect(ui.checkBox_compress, SIGNAL(stateChanged(int)), this, SIGNAL(noticeWidgetOpt(int)));

    connect(ui.comboBoxTransCheck2, SIGNAL(stateChanged(int)), this, SIGNAL(noticeWidgetOpt(int)));
}

void NetSetting::slotChangedDownloadCount(int count)
{
    CGlobalHelp::Response(UM_SPEED_LIMIT, EN_LIMIT_DOWNLOAD_COUNT, count);

    LocalSetting::getInstance()->setDownloadThreadCount(count);
}

void NetSetting::slotChangedUploadCount(int count)
{
    CGlobalHelp::Response(UM_SPEED_LIMIT, EN_LIMIT_UPLOAD_COUNT, count);

    LocalSetting::getInstance()->setUploadThreadCount(count);
}