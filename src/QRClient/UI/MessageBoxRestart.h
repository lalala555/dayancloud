/***********************************************************************
* Module:  MessageBoxRestart.h
* Author:  hqr
* Created: 2019/04/15 14:02:02
* Modifier: hqr
* Modified: 2019/04/15 14:02:02
* Purpose: Declaration of the class
***********************************************************************/
#ifndef MESSAGEBOXRESTART_H
#define MESSAGEBOXRESTART_H

#include "Dialog.h"

namespace Ui
{
class MessageBoxRestart;
}

class MessageBoxRestart : public Dialog
{
    Q_OBJECT

public:
    enum {
        MB_INFO,
        MB_ASK,
        MB_ERROR,
    };

public:
    explicit MessageBoxRestart(QWidget *parent = 0);
    ~MessageBoxRestart();

private slots:
    void on_btnOk_clicked();

private:
    Ui::MessageBoxRestart *ui;
};

#endif // MESSAGEBOXRESTART_H
