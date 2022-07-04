#include "stdafx.h"
#include "HttpCommand/HttpCmdRequest.h"

HttpCmdRequest::HttpCmdRequest(int type, QObject *parent)
    : QThread(parent)
    , m_cmdType(type)
    , m_cmdName("")
    , m_url("")
{
#if 1
    connect(&m_accesser, SIGNAL(sendResponse(const QByteArray&)), this, SLOT(analysisResponsed(const QByteArray&)));
#else
    connect(&m_accesser, SIGNAL(sendResponse(const QByteArray&)), this, SLOT(recvResponsed(const QByteArray&)));
#endif 
    initCmdProperty();
}

HttpCmdRequest::~HttpCmdRequest()
{
    quit();
    wait();
}

void HttpCmdRequest::post(const QString& url, const HttpRequestHeader& header)
{
    QNetworkRequest post;

    QUrl urlReq = QUrl(url + m_url);

    QSslConfiguration config;
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    post.setSslConfiguration(config);
    post.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    post.setUrl(urlReq);
    m_accesser.setRequestHeader(header);

    QByteArray body = serializeParams();
    qDebug() << urlReq << body;

#if 0
	QJsonDocument doc = QJsonDocument::fromJson(body);
	QJsonObject  obj = doc.object();
	QString flie_path = "D:/Git/renderfarm-dayan/dist/windows/bin/gcp_demo_result/19316795/test.json";
	RBHelper::saveJsonFile(flie_path, obj);
#endif

    // m_accesser.connectToHostEncrypted(urlReq.host());
    m_accesser.Post(post, body);
}

void HttpCmdRequest::get(const QString& url, const HttpRequestHeader& header)
{
    QNetworkRequest post;

    QUrl urlReq = QUrl(url + m_url);

    QSslConfiguration config;
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    post.setSslConfiguration(config);
    post.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    post.setUrl(urlReq);
    m_accesser.setRequestHeader(header);

    m_accesser.Get(post);
}

void HttpCmdRequest::upload(const QString & url, const HttpRequestHeader & header)
{
    QNetworkRequest post;

    QSslConfiguration config;
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    post.setSslConfiguration(config);
    post.setUrl(QUrl(url + m_url));
    m_accesser.setRequestHeader(header);

    QByteArray body = serializeParams();
    qDebug() << body;

    m_accesser.Upload(post, body);
}

QJsonObject HttpCmdRequest::qstringToJsonObject(const QString& str)
{
    QJsonObject jsonObject;

    QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8());
    if(!doc.isNull()) {
        if (doc.isObject()) {
            jsonObject = doc.object();
        }
    }

    return jsonObject;
}

void HttpCmdRequest::initCmdProperty()
{
    for(int i = 0; i < (sizeof(rest_uri_set)/sizeof(_rest_uri)); i++) {
        if(m_cmdType == rest_uri_set[i].idx) {
            m_cmdName = rest_uri_set[i].name;
            m_url = rest_uri_set[i].uri;
        }
    }
}

void HttpCmdRequest::destroyRequst()
{
    m_accesser.destroy();
    disconnect(&m_accesser, SIGNAL(sendResponse(const QByteArray&)), this, SLOT(analysisResponsed(const QByteArray&)));
}

void HttpCmdRequest::run()
{
    this->analysisResponsed(m_data);
}

void HttpCmdRequest::recvResponsed(const QByteArray& data)
{
    m_data = data;
    this->start();
}