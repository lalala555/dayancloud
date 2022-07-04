#ifndef TASKATKMLBLOCK_H
#define TASKATKMLBLOCK_H

#include <QWidget>

namespace Ui {
class TaskAtKmlBlock;
}

class TaskAtKmlBlock : public QWidget
{
    Q_OBJECT
public:
    explicit TaskAtKmlBlock(QWidget *parent = nullptr);
    ~TaskAtKmlBlock();

	void setinitView();
	void setKmlList(const QStringList & _kmlList);
	QString getEditTxt();
private:
	bool checkKml(const QString& _kml);

signals:
	void atXmlDelete(TaskAtKmlBlock * _atkml);  //TaskAtKmlBlock * _atkml
	void atXmlNumAdd();  //TaskAtKmlBlock * _atkml

private slots:

	void on_btnKmlAdd_clicked();
	void on_btnKmlDelete_clicked();
	void on_btnKmlOpen_clicked();

private:
    Ui::TaskAtKmlBlock *ui;
	
	QString m_kmlfile;
	QStringList m_kmlList;
};

//Q_DECLARE_METATYPE(TaskAtKmlBlock)

#endif // TASKATKMLBLOCK_H
