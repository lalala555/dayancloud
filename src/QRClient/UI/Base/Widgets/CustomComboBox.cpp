#include "stdafx.h"
#include "CustomComboBox.h"

CustomComboBox::CustomComboBox(QWidget *parent)
    : QComboBox(parent)
{
    this->setFocusPolicy(Qt::StrongFocus);
}

CustomComboBox::~CustomComboBox()
{

}
