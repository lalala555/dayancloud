/***********************************************************************
* Module:  AboutWindow.h
* Author:  hqr
* Modified: 2016/11/14 14:58:42
* Purpose: Declaration of the class
***********************************************************************/
#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include "Dialog.h"

namespace Ui
{
class AboutWindowClass;
}

class AboutWindow : public Dialog
{
    Q_OBJECT

public:
    AboutWindow(QWidget *parent = 0);
    ~AboutWindow();

private:
    virtual void changeEvent(QEvent *event);

private slots:
    void on_btnChangelog_clicked();

private:
    Ui::AboutWindowClass* ui;
};

#endif // ABOUTWINDOW_H
