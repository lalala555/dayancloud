#include "stdafx.h"
#include "ATGroupBox.h"

ATGroupBox::ATGroupBox(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
}

ATGroupBox::~ATGroupBox()
{
}