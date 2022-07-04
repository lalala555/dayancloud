#pragma once

#include <QWidget>
#include "Dialog.h"
#include "ui_BlockGuideWindow.h"

class BlockGuideWindow : public Dialog
{
    Q_OBJECT

private:
    enum PageNum
    {
        enFirstPage,
        enSecondPage,
        enThirdPage,

        enTotalPage
    };

public:
    BlockGuideWindow(QWidget *parent = Q_NULLPTR);
    ~BlockGuideWindow();
    void initUi();

private slots:
    void on_btnPrevPage_clicked();
    void on_btnNextPage_clicked();

private:
    void showPageAndWidget();

private:
    Ui::BlockGuideWindow ui;
    int m_currentPage;
};
