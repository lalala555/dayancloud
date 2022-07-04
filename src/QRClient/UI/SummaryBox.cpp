#include "stdafx.h"
#include "SummaryBox.h"
#include "ui_SummaryBoxRebuild.h"
#include "ui_SummaryBoxUpload.h"
#include "ui_SurveySelectImage.h"
#include "Common/StringUtil.h"
#include "HttpCommand/HttpCmdManager.h"

SummaryBoxUpload::SummaryBoxUpload(quint64 totalPics, quint64 totalPixel, quint64 ctrlPoint, QWidget *parent)
    : Dialog(parent)
    , ui(new Ui::SummaryBoxUpload)
{
    ui->setupUi(this);
    ui->label_logo->setPixmap(QPixmap(LOGO_ICON));

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);

    ui->label_pic_total->setText(QString::number(totalPics));
    ui->label_pix_total->setText(RBHelper::pixelsToString(totalPixel));
    ui->label_pix_total->setToolTip(QString::number(totalPixel) + "P");
    ui->label_ctrl_point->setText(QString::number(ctrlPoint) + tr("个"));

#ifdef FOXRENDERFARM
	ui->lab_title->setText("Upload information");
	ui->label_9->setFixedWidth(150);
	ui->label_3->setFixedWidth(150);
	ui->label_7->setFixedWidth(150);
#endif

}

SummaryBoxUpload::~SummaryBoxUpload()
{
    delete ui;
}

SummaryBoxRebuild::SummaryBoxRebuild(const QString& sceneName,
                             const QStringList& outputTypes, const QString& coordSys,
                             quint64 totalPics, quint64 totalPixel, QWidget *parent)
    : Dialog(parent)
    , ui(new Ui::SummaryBoxRebuild)
{
    ui->setupUi(this);
    ui->label_logo->setPixmap(QPixmap(LOGO_ICON));

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);

    QString types;
    foreach (auto type, outputTypes) {
        types.append(type);
        types.append(";");
    }

    ui->label_project->setText(sceneName);
    ui->label_pic_total->setText(QString::number(totalPics));
    ui->label_pix_total->setText(RBHelper::pixelsToString(totalPixel));
    ui->label_pix_total->setToolTip(QString::number(totalPixel) + "P");
    ui->label_output_type->setText(types);

    QString coord = String::getElidedText(ui->label_coord, coordSys, 300);
    ui->label_coord->setText(coord);
    ui->label_coord->setToolTip(coordSys);

#ifdef FOXRENDERFARM
	ui->lab_title->setText("Three-dimensional reconstruction confirmed");
	ui->label->setFixedWidth(150);
	ui->label_9->setFixedWidth(150);
	ui->label_3->setFixedWidth(150);
	ui->label_5->setFixedWidth(150);
	ui->label_7->setFixedWidth(150);	
	ui->label_2->adjustSize();
	ui->label_2->setWordWrap(true);

#endif

}

SummaryBoxRebuild::~SummaryBoxRebuild()
{
    delete ui;
}


SurveySelectImage::SurveySelectImage( QWidget *parent)
	: Dialog(parent)
	, ui(new Ui::SurveySelectImage)
{
	ui->setupUi(this);
	ui->label_logo->setPixmap(QPixmap(LOGO_ICON));

	this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);

#ifdef FOXRENDERFARM
	ui->label->setWordWrap(true);
	ui->label->setMinimumWidth(500);
	ui->label_9->setWordWrap(true);
	ui->label_9->setMinimumWidth(500);
	this->adjustSize();
	ui->lab_title->clear();
	ui->lab_title->setText("Path information prompt");
#endif



}

SurveySelectImage::~SurveySelectImage()
{
	delete ui;
}

