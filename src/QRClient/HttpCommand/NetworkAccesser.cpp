#include "stdafx.h"
#include "NetworkAccesser.h"
#include "Common/SystemUtil.h"

void NetworkAccesser::setHeaderData(QNetworkRequest &request)
{
    request.setHeader(QNetworkRequest::UserAgentHeader, QVariant(System::getUserAgent()));
    // request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    request.setRawHeader(QByteArray("channel"), QString::number(m_header.channel).toLocal8Bit());
    request.setRawHeader(QByteArray("platform"), QString::number(m_header.platform).toLocal8Bit());
    request.setRawHeader(QByteArray("languageFlag"), QString::number(m_header.languageFlag).toLocal8Bit());
    request.setRawHeader(QByteArray("signature"), m_header.signature.toLocal8Bit());
    request.setRawHeader(QByteArray("version"), m_header.version.toLocal8Bit());
    request.setRawHeader(QByteArray("userKey"), m_header.userKey.toLocal8Bit());
    request.setRawHeader(QByteArray("sTraceId"), m_header.sTraceId.toLocal8Bit());
}

void SSONetworkAccesser::setHeaderData(QNetworkRequest & request)
{
    request.setHeader(QNetworkRequest::UserAgentHeader, QVariant(System::getUserAgent()));
    request.setRawHeader(QByteArray("signature"), m_header.signature.toLocal8Bit());
    request.setRawHeader(QByteArray("version"), m_header.version.toLocal8Bit());
}
