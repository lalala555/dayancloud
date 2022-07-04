/***********************************************************************
* Module:  userrswindow.h
* Author:  hqr
* Modified: 2021/03/01 17:41:53
* Purpose: Declaration of the class
***********************************************************************/
#ifndef USERRSWINDOW_H
#define USERRSWINDOW_H
#include <QDialog>

namespace Ui
{
class UserRSWindow;
};

class UserRSWindow : public QDialog
{
    Q_OBJECT

public:
    UserRSWindow(QWidget *parent = 0);
    ~UserRSWindow();
    QString getUserCoordName();
    void initWindow(const QString& coordName);

private slots:
    void on_btnOk_clicked();
    void on_btnSelect_clicked();

private:
    Ui::UserRSWindow *ui;
    QString m_coordFilePath;
    QByteArray m_content;
};

#endif // CRSWINDOW_H
