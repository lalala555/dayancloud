/***********************************************************************
* Module:  StyleButton.h
* Author:  hqr
* Modified: 2021/06/11 14:42:34
* Purpose: Declaration of the class
***********************************************************************/
#pragma once

#include <QPushButton>

class StyleButton : public QPushButton
{
public:
    StyleButton(QWidget *parent = Q_NULLPTR);
    ~StyleButton() {}
};

class StyleButton90 : public StyleButton
{
public:
    StyleButton90(QWidget *parent = Q_NULLPTR);
    ~StyleButton90() {}
};

class StyleButton140 : public StyleButton
{
public:
    StyleButton140(QWidget *parent = Q_NULLPTR);
    ~StyleButton140() {}
};


class StyleButton140x40 : public StyleButton
{
public:
    StyleButton140x40(QWidget *parent = Q_NULLPTR);
    ~StyleButton140x40() {}
};
