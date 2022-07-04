#include "stdafx.h"
#include "TaskCloneWindow.h"
#include "HttpCommand/HttpCmdManager.h"

TaskCloneWindow::TaskCloneWindow(t_taskid taskId, QWidget *parent)
    : Dialog(parent)
    , m_cloneTaskId(taskId)
    , m_isWindow(true)
{
    ui.setupUi(this); 

    // 限制瓦片大小输入范围
    QDoubleValidator *validator = new QDoubleValidator(ui.lineEdit_tile_size);
    validator->setRange(50, 150, 2);
    ui.lineEdit_tile_size->setValidator(validator);

    ui.lineEdit_x_coord->setValidator(new QDoubleValidator(-999999999.999999, 999999999.999999, 6, ui.lineEdit_x_coord));
    ui.lineEdit_y_coord->setValidator(new QDoubleValidator(-999999999.999999, 999999999.999999, 6, ui.lineEdit_y_coord));
    ui.lineEdit_z_coord->setValidator(new QDoubleValidator(-999999999.999999, 999999999.999999, 6, ui.lineEdit_z_coord));

	ui.lineEdit_x_offset_clonewindow->setValidator(new QDoubleValidator(-999999999.999999, 999999999.999999, 6, ui.lineEdit_x_offset_clonewindow));
	ui.lineEdit_y_offset_clonewindow->setValidator(new QDoubleValidator(-999999999.999999, 999999999.999999, 6, ui.lineEdit_y_offset_clonewindow));
	ui.lineEdit_z_offset_clonewindow->setValidator(new QDoubleValidator(-999999999.999999, 999999999.999999, 6, ui.lineEdit_z_offset_clonewindow));
}

TaskCloneWindow::~TaskCloneWindow()
{
}

void TaskCloneWindow::initUi(bool isWindow)
{
    ui.checkBox_enable_custom->setChecked(false);
    ui.checkBox_lockRange->setChecked(true);
	ui.checkBox_xyz_offset_cloneWindow->setChecked(false);
    ui.lineEdit_x_coord->clear();
    ui.lineEdit_y_coord->clear();
    ui.lineEdit_z_coord->clear();
	ui.lineEdit_x_offset_clonewindow->clear();
	ui.lineEdit_y_offset_clonewindow->clear();
	ui.lineEdit_z_offset_clonewindow->clear();
    ui.comboBox_tile_mode->setCurrentIndex(0);

    // 如果不是独立窗口，则隐藏title和底部按钮
    if (!isWindow) {
        ui.widgetTitle->hide();
        ui.widget_ok->hide();
        m_isWindow = false;
    }

    // 初始化stackedWidget中的数据
    ui.comboOutputType->clear();
    for (int i = 0; i < OUTPUT_TYPE_TOTAL; i++) {
//        if(i == OUTPUT_TYPE_TIFF) continue; // 不显示TIFF
        ui.comboOutputType->addItem(RBHelper::getOutputTypeDisplay((OutputType)i), RBHelper::getOutputTypeString((OutputType)i));
    }
    ui.comboOutputType->setCurrentText(QStringList() << "OSGB");

    // 初始化坐标系
    ui.widget_coord_edit->initCtrl(COORD_REF_PARAM_SET_KEY);

    // 隐藏范围
    if (ui.checkBox_lockRange->isChecked()) {
        ui.widget_range_file->hide();
    }

    // 自定义原点
    ui.widget_coord_set->setEnabled(ui.checkBox_enable_custom->isChecked());

	// 坐标偏移
	ui.widget_coord_offset->setEnabled(ui.checkBox_xyz_offset_cloneWindow->isChecked());

    // 隐藏瓦片大小框
    ui.widget_tile_size->hide();

}

void TaskCloneWindow::initUi(const double& lon, const double& lat, bool isWindow)
{
	ui.checkBox_enable_custom->setChecked(false);
	ui.checkBox_lockRange->setChecked(true);
	ui.checkBox_xyz_offset_cloneWindow->setChecked(false);
	ui.lineEdit_x_coord->clear();
	ui.lineEdit_y_coord->clear();
	ui.lineEdit_z_coord->clear();
	ui.lineEdit_x_offset_clonewindow->clear();
	ui.lineEdit_y_offset_clonewindow->clear();
	ui.lineEdit_z_offset_clonewindow->clear();
	ui.comboBox_tile_mode->setCurrentIndex(0);

	// 如果不是独立窗口，则隐藏title和底部按钮
	if (!isWindow) {
		ui.widgetTitle->hide();
		ui.widget_ok->hide();
		m_isWindow = false;
	}

	// 初始化stackedWidget中的数据
	ui.comboOutputType->clear();
	for (int i = 0; i < OUTPUT_TYPE_TOTAL; i++) {
		//        if(i == OUTPUT_TYPE_TIFF) continue; // 不显示TIFF
		ui.comboOutputType->addItem(RBHelper::getOutputTypeDisplay((OutputType)i), RBHelper::getOutputTypeString((OutputType)i));
	}
	ui.comboOutputType->setCurrentText(QStringList() << "OSGB");

	// 初始化坐标系
	ui.widget_coord_edit->initCtrl(COORD_REF_PARAM_SET_KEY, lon, lat);

	// 隐藏范围
	if (ui.checkBox_lockRange->isChecked()) {
		ui.widget_range_file->hide();
	}

	// 自定义原点
	ui.widget_coord_set->setEnabled(ui.checkBox_enable_custom->isChecked());

	// 坐标偏移
	ui.widget_coord_offset->setEnabled(ui.checkBox_xyz_offset_cloneWindow->isChecked());

	// 隐藏瓦片大小框
	ui.widget_tile_size->hide();
}


QWidget* TaskCloneWindow::getParamWidget()
{
    return ui.widget_param;
}

bool TaskCloneWindow::getRebuildParams(RebuildOptionParams &param)
{
    QStringList outputTypes = ui.comboOutputType->selectedItemDatas();
    QString coordSys = ui.widget_coord_edit->getCoordSystemValue();
    QString coordSysFull = ui.widget_coord_edit->getCoordSystem();
    bool lockRange = ui.checkBox_lockRange->isChecked();
    QString rangeFile = ui.lineEdit_range_file->text();

    CoordValue customOriginCoord; // 自定义原点
    if (ui.checkBox_enable_custom->isChecked()) {
        if (ui.lineEdit_x_coord->text().isEmpty()) {
            SetErrorMessage(QObject::tr("请输入自定义原点X坐标"));
            return false;
        }
        if (ui.lineEdit_y_coord->text().isEmpty()) {
            SetErrorMessage(QObject::tr("请输入自定义原点Y坐标"));
            return false;
        }
        if (ui.lineEdit_z_coord->text().isEmpty()) {
            SetErrorMessage(QObject::tr("请输入自定义原点Z坐标"));
            return false;
        }
        customOriginCoord.coord_x = ui.lineEdit_x_coord->text().toDouble();
        customOriginCoord.coord_y = ui.lineEdit_y_coord->text().toDouble();
        customOriginCoord.coord_z = ui.lineEdit_z_coord->text().toDouble();
        customOriginCoord.enable = true;
    }

	CoordValue xyz_offset; // 坐标偏移值
	if (ui.checkBox_xyz_offset_cloneWindow->isChecked()) {
		if (ui.lineEdit_x_offset_clonewindow->text().isEmpty()) {
			SetErrorMessage(QObject::tr("请输入X坐标偏移值"));
			return false;
		}
		if (ui.lineEdit_y_offset_clonewindow->text().isEmpty()) {
			SetErrorMessage(QObject::tr("请输入Y坐标偏移值"));
			return false;
		}
		if (ui.lineEdit_z_offset_clonewindow->text().isEmpty()) {
			SetErrorMessage(QObject::tr("请输入Z坐标偏移值"));
			return false;
		}
		xyz_offset.coord_x = ui.lineEdit_x_offset_clonewindow->text().toDouble();
		xyz_offset.coord_y = ui.lineEdit_y_offset_clonewindow->text().toDouble();
		xyz_offset.coord_z = ui.lineEdit_z_offset_clonewindow->text().toDouble();
		xyz_offset.enable = true;
	}

    if (outputTypes.isEmpty()) {
        SetErrorMessage(QObject::tr("请选择输出格式"));
        return false;
    }

    if (coordSys.isEmpty()) {
        SetErrorMessage(QObject::tr("请选择坐标系"));
        return false;
    }

    if (!lockRange && rangeFile.isEmpty()) {
        SetErrorMessage(QObject::tr("请选择范围文件"));
        return false;
    }

    // 用户自定义坐标内容
    QString userCoordContent = CProfile::Inst()->getUserCoordSystemContent(coordSys);

    // 瓦片
    int tileMode = ui.comboBox_tile_mode->currentIndex();
    QString tileSize = ui.lineEdit_tile_size->text();
    if (tileMode == 0) {
        // if (tileSize.isEmpty()) {
        //     SetErrorMessage(QObject::tr("请输入瓦片大小"));
        //     return false;
        // }
        // 
        // if (tileSize.toInt() < 50 || tileSize.toInt() > 150) {
        //     SetErrorMessage(QObject::tr("瓦片大小不合法，请输入50-150范围内的值"));
        //     return false;
        // }
    }

    param.coordSys = coordSys;
    param.coordSysFull = coordSysFull;
    param.lockRange = lockRange;
    param.originCoord = customOriginCoord;
	param.offsetCoord = xyz_offset;
    param.outputTypes = outputTypes;
    param.rangeFile = rangeFile;
    param.usrCoordContent = userCoordContent;
    param.tileMode = tileMode;
    param.tileSize = tileSize;

    return true;
}

void TaskCloneWindow::on_btnOk_clicked()
{
    if (!getRebuildParams(m_rebuildParams)) {
        return;
    }

    // 请求接口
    // HttpCmdManager::getInstance()->submitCloneTask(m_cloneTaskId, m_rebuildParams);

    done(QDialog::Accepted);
}

void TaskCloneWindow::on_checkBox_lockRange_stateChanged(int state)
{
    bool bCheck = (state == Qt::Checked ? true : false);
    ui.widget_range_file->setVisible(!bCheck);
    ui.widget_range_file->setEnabled(!bCheck);
    ui.btnRangeFile->setEnabled(!bCheck);
}

void TaskCloneWindow::on_checkBox_enable_custom_stateChanged(int state)
{
    ui.widget_coord_set->setEnabled(state == Qt::Checked ? true : false);
}
void TaskCloneWindow::on_checkBox_xyz_offset_cloneWindow_stateChanged(int state)
{
	ui.widget_coord_offset->setEnabled(state == Qt::Checked ? true : false);
}

void TaskCloneWindow::SetErrorMessage(const QString &message)
{
    if (message.isEmpty())
        return;

    if (m_isWindow) {
        new ToastWidget(ui.widgetTitle, message, QColor("#e25c59"));
    } else {
        new ToastWidget(this->parentWidget(), message, QColor("#e25c59"));
    }  
}

void TaskCloneWindow::on_btnRangeFile_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("选择范围文件"), QString(), "KML (*.kml)");
    if (file.isEmpty()) return;
	if (checkIllegalpath(file)) return;
    ui.lineEdit_range_file->setText(file);
}

void TaskCloneWindow::on_comboBox_tile_mode_currentIndexChanged(int index)
{
    return; // 输入框一直隐藏
    if (index == 0) {
        ui.widget_tile_size->show();
    } else if (index == 1) {
        ui.widget_tile_size->hide();
    }
}

bool TaskCloneWindow::checkIllegalpath(const QString& path)
{
	//非法路径
	QStringList illPath;
	illPath << "C:/"
		<< "D:/work"
		<< "D:/7-Zip"
		<< "D:/enfwork"
		<< "D:/log"
		<< "D:/luts"
		<< "D:/plugins"
		<< "D:/renderwork"
		<< "D:/apache-flume-1.8.0"
		<< "D:/renderman"
		<< "D:/filebeat"
		;

	for (size_t i = 0; i < illPath.size(); i++)
	{
		if (path.indexOf(illPath[i]) == 0)
		{
			Util::ShowMessageBoxError(QObject::tr("非法路径，该路径平台脚本不支持，不支持的路径:<br> C:/ <br> D:/work <br>D:/7-Zip<br>D:/enfwork<br>D:/log<br>D:/luts<br>D:/plugins<br>D:/renderwork<br>D:/apache-flume-1.8.0<br>D:/renderman<br>D:/filebeat<br><br> 当前选择路径ID:%1<br>请更改数据本机存储路径并重新选择").arg(path));
			return true;
		}
	}
	return false;

}
