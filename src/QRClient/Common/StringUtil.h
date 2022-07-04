/***********************************************************************
* Module:  StringUtil.h
* Author:  hqr
* Modified: 2016/11/14 15:04:43
* Purpose: Declaration of the class
***********************************************************************/
#ifndef STRINGUTIL_H_
#define STRINGUTIL_H_

#include <QString>
#include <QLabel>
#include <QFont>
#include <QFontMetrics>

class String
{
public:
    static QString getStringMd5(const QString& path)
    {
        QByteArray byte_array;
        byte_array.append(path);
        QByteArray hash_byte_array = QCryptographicHash::hash(byte_array, QCryptographicHash::Md5);
        QString sha1 = hash_byte_array.toHex();
        return sha1;
    }

    static QString getElidedText(QLabel* label, const QString& txt, int maxWidth)
    {
        if (label == nullptr) return txt;

        QFont font_title = label->font();
        QFontMetrics titleMet(font_title);

        return titleMet.elidedText(txt, Qt::ElideRight, maxWidth);
    }
};
#endif // STRINGUTIL_H_