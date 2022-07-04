#pragma once

#include <QWidget>
#include "ui_ATGroupBox.h"

namespace Ui {
    class ATGroupBox;
}

class ATGroupBox : public QWidget
{
    Q_OBJECT

public:
    ATGroupBox(QWidget *parent = Q_NULLPTR);
    ~ATGroupBox();


private:
    Ui::ATGroupBox ui;
};
