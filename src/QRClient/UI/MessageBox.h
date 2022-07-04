/***********************************************************************
* Module:  MessageBox.h
* Author:  hqr
* Modified: 2016/11/14 15:00:52
* Purpose: Declaration of the class
***********************************************************************/
#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include "Dialog.h"

namespace Ui
{
class CMessageBox;
}

class CMessageBox : public Dialog
{
    Q_OBJECT

public:
    enum {
        MB_INFO,
        MB_ASK,
        MB_ERROR,
    };

public:
    explicit CMessageBox(QWidget *parent = 0);
    ~CMessageBox();

    void setMessage(const QString &msg, int type);
    void setTitle(const QString &title);
    void setOkBtnText(const QString& text);
    void setCancelText(const QString& text);

private slots:
    void on_btnOk_clicked();
    
signals:
    void linkClicked(const QString& link);

protected:
    bool eventFilter(QObject* obj, QEvent* event);

private:
    Ui::CMessageBox *ui;
};

#endif // MESSAGEBOX_H
