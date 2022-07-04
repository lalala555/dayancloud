#ifndef SURVEYINFOWINDOW_H
#define SURVEYINFOWINDOW_H

#include <QWidget>
#include <QDialog>
#include "ui_SurveyInfoWindow.h"
//#include "Dialog.h"
namespace Ui {
class SurveyInfoWindow;
}

class SurveyInfoWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SurveyInfoWindow(t_taskid taskId,QWidget *parent = nullptr);
    ~SurveyInfoWindow();
	void initUi(bool tptype = true);  // 连接点类型

	QString getPointName();
	int getCurrentIndex();
	QString getCurrentText();
	void setTiePointNum(const short& num);
	void setTiePointMaxID(const short& MaxID);
	void resetView() { initUi(false); }

private:
    Ui::SurveyInfoWindow *ui;
	t_taskid m_SurveyAtId;
	short m_tpSize;
	short m_tpMaxID;
};

#endif // SURVEYINFOWINDOW_H
