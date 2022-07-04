#include "stdafx.h"
#include "StyleButton.h"

StyleButton::StyleButton(QWidget * parent)
    : QPushButton(parent)
{
    setStyleSheet("QPushButton{background-color: #675ffe;color: #ffffff;font-size:14px;border-radius:4px;}"
                  "QPushButton:hover{background-color:#7879ff;}"
                  "QPushButton:pressed{background-color:#7879ff;}"
    );
}

StyleButton90::StyleButton90(QWidget * parent)
    : StyleButton(parent)
{
    setStyleSheet("QPushButton{width: 90px; min-height: 34px; max-height: 34px;}");
}

StyleButton140::StyleButton140(QWidget * parent)
    : StyleButton(parent)
{
    setStyleSheet("QPushButton{width: 140px; min-height: 34px; max-height: 34px;}");
}

StyleButton140x40::StyleButton140x40(QWidget * parent)
    : StyleButton(parent)
{
    setStyleSheet("QPushButton{width: 140px; min-height: 40px; max-height: 40px;}");
}
