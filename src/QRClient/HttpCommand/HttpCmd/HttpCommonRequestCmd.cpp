#include "stdafx.h"
#include "HttpCommonRequestCmd.h"

HttpCommonRequestCmd::HttpCommonRequestCmd(QObject *parent)
    : HttpCmdRequest(0, parent)
{
}

QByteArray HttpCommonRequestCmd::serializeParams()
{
    QByteArray arr;
    return arr;
}

void HttpCommonRequestCmd::analysisResponsed(const QByteArray& data)
{

}