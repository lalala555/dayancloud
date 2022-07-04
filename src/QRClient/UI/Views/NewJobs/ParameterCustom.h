/***********************************************************************
* Module:  ParameterCustom.h
* Author:  hqr
* Modified: 2016/11/14 14:58:42
* Purpose: Declaration of the class
***********************************************************************/
#ifndef PARAMETERCUSTOM_H
#define PARAMETERCUSTOM_H

#include "Dialog.h"
#include "HttpCommand/HttpCommon.h"


namespace Ui
{
class ParameterCustomClass;
}

class ParameterCustom : public Dialog
{
    Q_OBJECT

public:
    ParameterCustom(QWidget *parent = 0);
    ~ParameterCustom();

private:
    virtual void changeEvent(QEvent *event);
    virtual void showEvent(QShowEvent *e);

private:
    void showLoading(bool loading);
    bool getNewTaskIdSuccess(qint64 taskId, const QString& taskIdAlias);

private slots:
    void on_btnOk_clicked();
    void onRecvResponse(QSharedPointer<ResponseHead> response);

private:
    Ui::ParameterCustomClass* ui;
    qint64 m_oldTaskId;
    QProgressIndicator*  m_busyIcon;
};

#endif // PARAMETERCUSTOM_H
