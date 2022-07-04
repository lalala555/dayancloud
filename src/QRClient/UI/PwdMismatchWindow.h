/***********************************************************************
* Module:  PwdMismatchWindow.h
* Author:  hqr
* Modified: 2016/11/14 15:02:08
* Purpose: Declaration of the class
***********************************************************************/
#ifndef PWDMISMATCHWINDOW_H
#define PWDMISMATCHWINDOW_H

#include "Dialog.h"

namespace Ui
{
class PwdMismatchWindowClass;
}

class PwdMismatchWindow : public Dialog
{
    Q_OBJECT

public:
    PwdMismatchWindow(QWidget *parent = 0);
    ~PwdMismatchWindow();

private slots:
    void on_btnPwdLink_clicked();

protected:
    virtual void changeEvent(QEvent *event);
private:
    Ui::PwdMismatchWindowClass* ui;
};

#endif // PWDMISMATCHWINDOW_H
