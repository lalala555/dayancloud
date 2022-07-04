#ifndef PLATFORMITEM_H
#define PLATFORMITEM_H

#include <QWidget>
#include "ui_PlatformItem.h"
#include "UserProfile/WSConfig.h"

struct PlatformStatus;

class PlatformItem : public QWidget
{
    Q_OBJECT

public:
    PlatformItem(const QString& name, const QIcon& state, QWidget *parent = 0);
    PlatformItem(int platform, const QString& name, const QString& state, QWidget *parent = 0);
    ~PlatformItem();
    QString getPlatformName(){ return m_name; }
    bool isChecked();
    void setChecked(bool checked);
    void setPlatformInfo(const platform_info& info){m_pltInfo = info;}
    platform_info getPlatformInfo(){return m_pltInfo;}
    QRadioButton* getBtnObj();
signals:
    void itemChecked(int);

private slots:
    void onItemClicked(bool);

private:
    Ui::PlatformItem ui;
    QString m_name;
    QIcon m_state;
    platform_info m_pltInfo;
    int m_platformId;
};

#endif // PLATFORMITEM_H
