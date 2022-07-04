/***********************************************************************
* Module:  Util.h
* Author:  hqr
* Modified: 2016/07/03 17:42:34
* Purpose: Declaration of the class
***********************************************************************/
#ifndef UTILITY_H
#define UTILITY_H

#include <QPixmap>

class Util: public QObject
{
public:
    static void setFont(const QString &fontName);
    static void setStylesheet(const QString &styleName);
    static void ShowMessageBoxInfo(const QString &info, const QString& title = "");
    static void ShowMessageBoxError(const QString &info, const QString& title = "");
    // static int ShowMessageBoxQuesion(const QString &info, const QString& title = "");
    static int ShowMessageBoxQuesion(const QString &info, const QString& title = "", const QString& okBtnText = QObject::tr("确定"), const QString& cancelBtnText = QObject::tr("取消"));
};

#endif // UTILITY_H
