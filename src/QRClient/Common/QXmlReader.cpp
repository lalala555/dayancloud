#include "stdafx.h"
#include "QXmlReader.h"

QXmlReader::QXmlReader(QObject *parent)
    : QObject(parent)
{
}

QXmlReader::~QXmlReader()
{
}

bool QXmlReader::loadXml(const QString& content)
{
    QFileInfo fileInfo(content);
    if (!fileInfo.isFile()) {
        if(!m_doc.setContent(content)){
            return false;
        }
        return true;
    } else {

        QFile file(content);
        if (!file.open(QFile::ReadOnly)) {
            return false;
        }

        QString errmsg;
        int line;
        int col;
        if (!m_doc.setContent(&file, &errmsg, &line, &col)) {
            file.close();
            qDebug() << errmsg << " line " << line << " col " << col;
            return false;                                    
        }
        file.close();

        return true;
    }
}

QDomElement QXmlReader::getRootElement()
{
    m_root = m_doc.documentElement();
    return m_root;
}

QDomElement QXmlReader::getElementByTagName(const QString & tagName, const QDomElement & parent)
{
    QDomElement root = parent;
    if(root.isNull()){
        root = getRootElement();    
    }

    QDomNode child = root.firstChild();
    QDomElement element;
    while (!child.isNull()) {
        QDomElement e = child.toElement();
        if (!e.isNull() && e.tagName() == tagName) {
            element = e;
            break;
        }
        child = child.nextSibling();
    }

    return element;
}