/***********************************************************************
* Module:  CoordEditWindow.h
* Author:  hqr
* Modified: 2020/12/28 14:58:42
* Purpose: Declaration of the class
***********************************************************************/
#ifndef COORDEDITWINDOW_H
#define COORDEDITWINDOW_H

#include "Dialog.h"
#include "kernel/render_struct.h"

namespace Ui
{
class CoordEditWindowClass;
}

//////////////////////////////////////////////////////////////////////////
class CornerLabel : public QLabel
{
    Q_OBJECT
public:
    CornerLabel(QWidget *parent = 0);
    ~CornerLabel();
    void setPicPath(const QString& path) { m_picPath = path; }
    QString getPicPath() { return m_picPath; }

protected:
    virtual void paintEvent(QPaintEvent* ev);
    virtual void enterEvent(QEvent *e);
    virtual void leaveEvent(QEvent *e);

signals:
    void btnClicked(const QString&);

private slots:
    void onBtnClicked();

private:
    QPushButton* m_closeBtn;
    QString m_picPath;
};
//////////////////////////////////////////////////////////////////////////

class CoordEditWindow : public Dialog
{
    Q_OBJECT

public:
    CoordEditWindow(QWidget *parent = 0);
    ~CoordEditWindow();
    CoordEditInfo getCoordEditInfo();
    void setCoordEditInfo(const CoordEditInfo& info);

private:
    virtual void changeEvent(QEvent *event);

private slots:
    void on_btnAddPicture_clicked();
    void on_btnEditOk_clicked();
    void onDeleteClicked(const QString& path);

private:
    CornerLabel* addPicture(const QString& pic);
    void addDefaultCtrl();
    CoordValue getCoordValue();
    void showPictures(const QStringList& pics);
    void showCoordValues(const CoordValue& values);
    bool checkSettingsEnable();

private:
    Ui::CoordEditWindowClass* ui;
    QPushButton* m_addPicBtn;
    QSpacerItem* m_hspacerItem;
    QMap<QString, CornerLabel*> m_pictures;
};

#endif // ABOUTWINDOW_H
