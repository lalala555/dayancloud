#pragma once

#include <QObject>
#include <QListView>
#include <QPaintEvent>


class CustListView : public QListView
{
    Q_OBJECT

public:
    CustListView(QWidget *parent);
    ~CustListView();

    void showLoading(bool bShow);
    void setEmptyHint(const QString& hint);

protected:
    // virtual void paintEvent(QPaintEvent *e);
    virtual void resizeEvent(QResizeEvent *e);

private:
    QString m_emptyHint;
    bool m_isShowLoading;
    QProgressIndicator* m_busyIcon;
};
