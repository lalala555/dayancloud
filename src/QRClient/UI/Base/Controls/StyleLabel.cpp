#include "stdafx.h"
#include "StyleLabel.h"

StyleLabel::StyleLabel(QWidget * parent)
    : QLabel(parent)
{
}

StyleLabel12::StyleLabel12(QWidget * parent)
    : QLabel(parent)
{
    setStyleSheet("color:#999999;font-size:12px;");
}

StyleLabel14::StyleLabel14(QWidget * parent)
    : QLabel(parent)
{
    setStyleSheet("color:#ffffff;font-size:14px;");
}

StyleLabel16::StyleLabel16(QWidget * parent)
    : QLabel(parent)
{
    setStyleSheet("color:#675ffe;font-size:16px;line-height: 35px; ");
}

StyleLabel18::StyleLabel18(QWidget * parent)
    : QLabel(parent)
{
    setStyleSheet("color:#ffffff;font-size:18px;");
}