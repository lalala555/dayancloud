/***********************************************************************
* Module:  SummaryWindow.h
* Author:  hqr
* Modified: 2021/06/05 15:00:52
* Purpose: Declaration of the class
***********************************************************************/
#ifndef SUMMARYWINDOW_H
#define SUMMARYWINDOW_H

#include "Dialog.h"

namespace Ui
{
class SummaryBoxRebuild;
class SummaryBoxUpload;
class SurveySelectImage;
}

// �ϴ�������ʾ
class SummaryBoxUpload : public Dialog
{
    Q_OBJECT

public:
    explicit SummaryBoxUpload(quint64 totalPics, quint64 totalPixel, quint64 ctrlPoint, QWidget *parent = 0);
    ~SummaryBoxUpload();

private:
    Ui::SummaryBoxUpload *ui;
};


// �ؽ�������ʾ
class SummaryBoxRebuild : public Dialog
{
    Q_OBJECT

public:
    explicit SummaryBoxRebuild(const QString& sceneName,
                           const QStringList& outputTypes, const QString& coordSys,
                           quint64 totalPics, quint64 totalPixel, QWidget *parent = 0);
    ~SummaryBoxRebuild();

private:
    Ui::SummaryBoxRebuild *ui;
};


//�̵���Ϣ��ʾ
class SurveySelectImage : public Dialog
{
	Q_OBJECT

public:
	explicit SurveySelectImage(QWidget *parent = 0);
	~SurveySelectImage();

private:
	Ui::SurveySelectImage *ui;
};


#endif // SUMMARYWINDOW_H



