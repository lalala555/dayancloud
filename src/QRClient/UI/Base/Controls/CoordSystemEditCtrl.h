#pragma once

#include <QWidget>
#include "ui_CoordSystemEditCtrl.h"

class CoordSystemEditCtrl : public QWidget
{
    Q_OBJECT

public:
    CoordSystemEditCtrl(QWidget *parent = Q_NULLPTR);
    ~CoordSystemEditCtrl();

    QString getCoordSystemValue();
    QString getCoordSystem();
    void hideCoordImage();
    void setLabelText(const QString& text);
    void setLableFixWidth(int width);
    void initCtrl(const QString& key);
	void initCtrl(const QString& key, const double& lon,const double& lat);
    void setCoordSystemValue(const QString& v);

private slots:
    void on_btnEditCoord_clicked();

private: 
    void initCoordSystem();
	void initCoordSystem(const double& lon,const double& lat);

private:
    Ui::CoordSystemEditCtrl ui;
    QString m_regKey;
	double m_lon;
};
