/***********************************************************************
* Module:  SRSWINDOW_H.h
* Author:  hqr
* Modified: 2021/02/24 17:41:53
* Purpose: Declaration of the class
***********************************************************************/
#ifndef SRSWINDOW_H
#define SRSWINDOW_H
#include <QDialog>
#include <QTreeWidgetItem>
#include "kernel/render_struct.h"

namespace Ui
{
class SRSWindow;
};

class SRSWindow : public QDialog
{
    Q_OBJECT

    enum {
        SRS_ENU,
        SRS_GCS,
        SRS_PCS,
        SRS_CCS,
        SRS_USR
    };

public:
    SRSWindow(const QString& sysDefine, const QString& regKey, QWidget *parent = 0);
    ~SRSWindow();

    CoordRefSystem getCoordinateSystem() { return m_coordSys; };

private slots:
    void on_btnEdit_clicked();
    void on_btnOk_clicked();
    void onLatLngChanged(const QString& text);
    void onItemClicked(QTreeWidgetItem *item, int column);

private:
    void initUi();
    void initData();
    void updateGroupTitle();
    QTreeWidgetItem* addTreeRoot(QString name);
    QTreeWidgetItem* addTreeChild(QTreeWidgetItem *parent, QString name, QString desc = "");

private:
    Ui::SRSWindow *ui;
    QTreeWidgetItem *m_treeItemENU;

    QMap<QString, QString> m_gcsDatabase;
    int m_totalRSCount;

    QString m_sysDefine;
    CoordRefSystem m_coordSys;

    QString m_regKey;


    QTreeWidgetItem *m_userTreeItem;
};

#endif // SRSWINDOW_H
