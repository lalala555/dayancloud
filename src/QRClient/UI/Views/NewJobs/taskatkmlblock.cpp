#include "stdafx.h"
#include "taskatkmlblock.h"
#include "ui_taskatkmlblock.h"

TaskAtKmlBlock::TaskAtKmlBlock(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TaskAtKmlBlock),
	m_kmlfile("")
{
    ui->setupUi(this);
}

TaskAtKmlBlock::~TaskAtKmlBlock()
{
    delete ui;
}

void TaskAtKmlBlock::setinitView()
{
	ui->btnKmlDelete->setEnabled(false);
}

void TaskAtKmlBlock::setKmlList(const QStringList & _kmlList)
{
	m_kmlList = _kmlList;
}

QString TaskAtKmlBlock::getEditTxt()
{
	return m_kmlfile;
}

bool TaskAtKmlBlock::checkKml(const QString & _kml)
{
	//判断是否存在中文路径
	if (_kml.contains(QRegExp("[\\x4e00-\\x9fa5]+")))
	{
		Util::ShowMessageBoxError(QObject::tr("存在中文路径，请修改中文路径后提交数据！"));
		return true;
	}

	for (size_t i = 0; i < m_kmlList.size(); i++)
	{
		if (_kml == m_kmlList[i])
		{
//			int res = Util::ShowMessageBoxQuesion(QObject::tr("KML文件重复，请重新修改！"), QObject::tr("温馨提示"), QObject::tr("取消"), QObject::tr("确定"));
//			if (res == QDialog::Rejected || res == QDialog::Accepted)
//				return true;

			Util::ShowMessageBoxError(QObject::tr("KML文件重复，请勿重复提交！"));
			return true;

		}
	}
	return false;
}


void TaskAtKmlBlock::on_btnKmlDelete_clicked()
{	
	emit atXmlDelete(this);
}

void TaskAtKmlBlock::on_btnKmlOpen_clicked()
{
	QString file = QFileDialog::getOpenFileName(this, tr("选择分块kml文件"), QString(), "KML (*.kml);");
	if (file.isEmpty()) return;
	if (checkKml(file)) return;

	ui->lineEdit_atKml->setText(file);
	m_kmlfile = file;
}

void TaskAtKmlBlock::on_btnKmlAdd_clicked()
{
	if (ui->lineEdit_atKml->text().isEmpty())
	{
		Util::ShowMessageBoxError(QObject::tr("请先打开加载对应的KML文件！"));
		return;
	}

	emit atXmlNumAdd();
}