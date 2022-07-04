#pragma once

#include <QTreeWidget>

class QProgressIndicator;

class TreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    TreeWidget(QWidget *parent = 0);
    ~TreeWidget();

    void showLoading(bool bShow);
    void setEmptyHint(const QString& hint);

protected:
    virtual void paintEvent(QPaintEvent *e);    
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void showEvent(QShowEvent *e);
    virtual void resizeEvent(QResizeEvent *e);
    virtual void keyPressEvent(QKeyEvent* e);
    virtual void enterEvent(QEvent *e);
    virtual void leaveEvent(QEvent *e);

protected:
    QString m_emptyHint;
    bool m_isShowLoading;
    QProgressIndicator* m_busyIcon;
};
