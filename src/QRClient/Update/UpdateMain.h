#ifndef UPDATEMAIN_H
#define UPDATEMAIN_H

#include <QElapsedTimer>
#include "Dialog.h"
#include "ui_UpdateMain.h"

class UpdateMain : public Dialog
{
    Q_OBJECT

public:
    UpdateMain(QWidget *parent = 0);
    ~UpdateMain();

    void startDownload();
public slots:
    void onCmdArrived(const QString& data);
    void onUpdateReturn(int updateCode);

private slots:
    void on_btnOk_clicked();

private:
    void closeEvent(QCloseEvent * event);
protected:
    virtual void changeEvent(QEvent *event);
private:
    Ui::UpdateMain ui;

    QString m_strInstallRoot;
    QString m_strDownFileName;
    QElapsedTimer m_downloadTimer;
    bool m_isAlive;
};

#endif // UPDATEMAIN_H
