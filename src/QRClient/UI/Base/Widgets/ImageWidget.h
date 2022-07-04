#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QImage>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtConcurrent/QtConcurrent>
#include <QPalette>
#include <QPixmap>
#include <QBrush>
#include <QStylePainter>
#include <QMovie>
#include "HttpCommand/HttpCommon.h"
#include "ui_ImageWidget.h"


class ImageWidget : public QWidget
{
    Q_OBJECT

public:
    ImageWidget(QString host, int row, int col, int matrix = 4, QWidget *parent = 0);
    ~ImageWidget();
    void cropImage(const QByteArray& data);
    void setGrabInfo(TaskGrabInfo* grab);
    TaskGrabInfo* getGrabInfo() { return &m_grab; }
    void hideInfoWidget();
    void setRoundedCorner(qreal radius, bool topLeft, bool topRight, bool bottomLeft, bool bottomRight);

public slots:
    void downloadImage();
private slots:
    void onUpdateImage(const QPixmap& pix);
    void onDownloadImageFinished(QNetworkReply* reply);
signals:
    void updateImage(const QPixmap& pix);
    void mouseClicked();

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);
    virtual void mousePressEvent(QMouseEvent *event);
private:
    void initGrabInfo();

private:
    Ui::ImageWidget ui;
    int m_col;
    int m_row;
    int m_matrix;
    QString m_imgPath;
    QNetworkAccessManager m_download;
    TaskGrabInfo m_grab;
    QPixmap  m_pixmap;
    QMovie *m_movie;
    QString m_host;
    QString m_imgUrl;
};

#endif // IMAGEWIDGET_H
