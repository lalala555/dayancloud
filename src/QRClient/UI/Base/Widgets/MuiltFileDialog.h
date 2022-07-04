#pragma once

#include <QObject>
#include <QFileDialog>
#include <QDialog>

class MuiltFileDialog : public QFileDialog
{
    Q_OBJECT

public:
    MuiltFileDialog(QWidget *parent = 0);
    ~MuiltFileDialog();

public slots:
    void onChiose();
};
