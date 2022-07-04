/***********************************************************************
* Module:  ISetting.h
* Author:  hqr
* Modified: 2016/11/16 15:16:22
* Purpose: Declaration of the class
***********************************************************************/
#pragma once
#ifndef ISETTING_H_
#define ISETTING_H_

#include <QSettings>

class ISetting
{
public:
    ISetting() { }
    virtual ~ISetting() { delete m_settings; }

    virtual bool init() = 0;
protected:
    void save()
    {
        if(m_settings)
            m_settings->sync();
    };
    QSettings* m_settings;
};


#endif // ISETTING_H_