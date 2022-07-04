/***********************************************************************
* Module:  ThemeLoader.h
* Author:  hqr
* Created: 2017/08/28 11:45:44
* Modifier: hqr
* Modified: 2017/08/28 11:45:44
* Purpose: Declaration of the class
***********************************************************************/
#ifndef THEMELOADER_H
#define THEMELOADER_H

#include <QDir>
#include <QHash>
#include <QObject>
#include <QStringList>
#include "ThemeInfo.h"

class ThemeLoader : public QObject
{
    Q_OBJECT

public:
    static ThemeLoader* instance();

    QStringList themes() const;
    ThemeInfo theme(const QString& name) const;
    void setCurrTheme(const ThemeInfo& theme);
    ThemeInfo currTheme();
private:
    ThemeLoader(QObject* parent = 0);

    void load(QDir dir);

    struct Private {
        QStringList themes;
        QHash<QString, ThemeInfo> infos;
        ThemeInfo currTheme;
    } d;
};

#endif // THEMELOADER_H
