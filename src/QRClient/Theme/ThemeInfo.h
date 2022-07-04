/***********************************************************************
* Module:  ThemeInfo.h
* Author:  hqr
* Created: 2017/08/28 11:45:37
* Modifier: hqr
* Modified: 2017/08/28 11:45:37
* Purpose: Declaration of the class
***********************************************************************/
#ifndef THEMEINFO_H
#define THEMEINFO_H

#include <QString>

class ThemeInfo
{
public:
    bool isValid() const;
    bool load(const QString& filePath);
    bool reload();

    QString name() const;
    QString author() const;
    QString version() const;
    QString description() const;
    QString style() const;
    QString stylesheet() const;
    QString font() const;
    QString path() const;
    QString rccPath() const;

private:
    struct Private {
        QString name;
        QString author;
        QString version;
        QString description;
        QString style;
        QString stylesheet;
        QString font;
        QString path;
        QString rccPath;
        QString filePath;
    } d;
};

#endif // THEMEINFO_H
