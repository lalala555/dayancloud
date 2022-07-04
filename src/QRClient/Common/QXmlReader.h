#pragma once

#include <QObject>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDomNodeList>
#include <QFile>
#include <QFileInfo>

class QXmlReader : public QObject
{
    Q_OBJECT

public:
    QXmlReader(QObject *parent = nullptr);
    ~QXmlReader();
    bool loadXml(const QString& content);
    QDomElement getRootElement();
    QDomElement getElementByTagName(const QString& tagName, const QDomElement& parent = QDomElement());

private:
    QDomDocument  m_doc;
    QDomElement   m_root;
};
