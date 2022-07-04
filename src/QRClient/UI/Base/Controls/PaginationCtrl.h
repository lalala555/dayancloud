#pragma once

#include <QWidget>
#include "ui_PaginationCtrl.h"

class PaginationCtrl : public QWidget
{
    Q_OBJECT

public:
    PaginationCtrl(QWidget *parent = Q_NULLPTR);
    ~PaginationCtrl();
    void setPageInfo(int totalCount, int displayCount = 16, int btnCount = 5);
    qint32 getCurrDisplayItemCount();
    void setDisplayItemCount(const QList<qint32>& displays);
    qint32 getCurrentPage();
    qint32 getRowCount();

private slots:
    void on_btnPagePrev_clicked();
    void on_btnPageNext_clicked();
    void onPageEditFinished();
    void onPageBtnClicked();
    void on_comboBox_counts_currentIndexChanged(int index);
private:
    void initPageBtn();
    QVector<QPushButton*> getPageBtns(int totalPage);
    QString getBtnNormalStyle();
    QString getBtnSelectedStyle();
    qint32 getTotalPageSize();
    void setPageBtnDynamicPos(int totalPage, int curPage);
    void setTotalCountText(int total);
    qint32 getTotalPageCount(int totalSize, int itemCount);

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void showEvent(QShowEvent* event);
    virtual void paintEvent(QPaintEvent *p);

signals:
    void pageChanged(int, int);
    void displayItemCountChanged(int);

private:
    Ui::PaginationCtrl ui;
    qint32 m_currentPage;
    qint32 m_totalSize;
    qint32 m_totalPage;
    qint32 m_btnCount; // 动态按钮个数
    qint32 m_displayItemCount; // 展示条目个数
};
