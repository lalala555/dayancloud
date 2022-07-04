#pragma once

#include <QWidget>
#include "ui_QImageViewer.h"
#include "HttpCommand/HttpCommon.h"

class ResponseHead;
class TaskProcessImgResponse;

class QImageViewer : public QWidget
{
    Q_OBJECT

public:
    QImageViewer(QWidget *parent = Q_NULLPTR);
    ~QImageViewer();

    void startLoadProcessImage(JobState* job);
    void loadProgressImage(QSharedPointer<TaskProcessImgResponse> processImg);
    void updateView();
    void addImageDownload(qint64 taskId, int index, const QString& strUrl, TaskGrabInfo* grab);
    void updateJobstate(JobState* job);
    bool needRequestImageData();

private slots:
    void onLoadProcessImage();
    void onRecvResponse(QSharedPointer<ResponseHead> response);
    void onMouseClicked();
    void onFinished(qint64 taskId, int index, const QByteArray& imageData);
    void showProcessImage();

private:
    void loadMajorImage();
    void loadPhotonImage();
signals:
    void startLoadImage();
    void onItemClicked(qint64 taskId);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

private:
    Ui::QImageViewer ui;
    QString m_host;
    QTimer m_freshTimer;
    JobState* m_job;
    int m_matrix;
    QThreadPool m_threadpool;
    int m_finishedImages;
};
