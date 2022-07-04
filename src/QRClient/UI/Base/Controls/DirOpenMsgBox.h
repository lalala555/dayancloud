#ifndef DIROPENMSGBOX_H
#define DIROPENMSGBOX_H

#include "Dialog.h"
#include "ui_DirOpenMsgBox.h"

class DirOpenMsgBox : public Dialog
{
    Q_OBJECT

public:
    DirOpenMsgBox(QWidget *parent = 0);
    ~DirOpenMsgBox();
    void setMsgContent(const QString& msg, const QString& keyWord = "");
    QString getSavePath();
    QString fileDialogOpen();

public slots:
    void labelLinkClicked(const QString &);
    void btnRedownloadClicked();

private:
    Ui::DirOpenMsgBox ui;
    QString m_savePath;
};

#endif // DIROPENMSGBOX_H
