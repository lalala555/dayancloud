#ifndef NETWORKACCESSER_H
#define NETWORKACCESSER_H

#include "NetworkAccesserBase.h"

class NetworkAccesser : public NetworkAccesserBase
{
    Q_OBJECT
public:
    NetworkAccesser(QObject *parent = 0) {};
    ~NetworkAccesser() {};

private:
    virtual void setHeaderData(QNetworkRequest &request);
};

class SSONetworkAccesser : public NetworkAccesserBase
{
    Q_OBJECT
public:
    SSONetworkAccesser(QObject *parent = 0) {};
    ~SSONetworkAccesser() {};

private:
    virtual void setHeaderData(QNetworkRequest &request);
};

#endif // NETWORKACCESSER_H
