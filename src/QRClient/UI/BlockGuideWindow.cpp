#include "stdafx.h"
#include "BlockGuideWindow.h"

BlockGuideWindow::BlockGuideWindow(QWidget *parent)
    : Dialog(parent)
    , m_currentPage(enFirstPage)
{
    ui.setupUi(this);

    initUi();
}

BlockGuideWindow::~BlockGuideWindow()
{
}

void BlockGuideWindow::initUi()
{
	ui.label_logo->setPixmap(QPixmap(LOGO_LARGE_ICON));
    ui.stackedWidget->setCurrentIndex(enFirstPage);
    showPageAndWidget();

	//产品photoscan 暂未提供英文版截图，暂时只显示第一页
#ifdef FOXRENDERFARM
	ui.btnPrevPage->hide();
	ui.btnNextPage->hide();
	ui.btnClosePage->show();
#endif

}

void BlockGuideWindow::on_btnPrevPage_clicked()
{
#ifdef  FOXRENDERFARM  //海外版不显示smart3D提示
	m_currentPage = 0;

#else
	m_currentPage--;
	if (m_currentPage < 0)
		m_currentPage = enFirstPage;
#endif //  FOXRENDERFARM
	
    showPageAndWidget();
}

void BlockGuideWindow::on_btnNextPage_clicked()
{
	
#ifdef  FOXRENDERFARM  //海外版不显示smart3D提示
	m_currentPage = 2;

#else
	m_currentPage++;
	if (m_currentPage > enThirdPage)
		m_currentPage = enThirdPage;
#endif //  FOXRENDERFARM

    showPageAndWidget();
}

void BlockGuideWindow::showPageAndWidget()
{
    ui.stackedWidget->setCurrentIndex(m_currentPage);

    switch (m_currentPage) {
    case enFirstPage: {
        ui.btnPrevPage->hide();
        ui.btnNextPage->show();
        ui.btnClosePage->hide();
        ui.btnNextPage->setDefault(true);
    } 
    break;
    case enSecondPage: {
        ui.btnPrevPage->show();
        ui.btnNextPage->show();
        ui.btnClosePage->hide();
        ui.btnPrevPage->setDefault(false);
        ui.btnNextPage->setDefault(true);
    }
    break;
    case enThirdPage: {
        ui.btnPrevPage->show();
        ui.btnNextPage->hide();
        ui.btnClosePage->show();
        ui.btnPrevPage->setDefault(false);
        ui.btnClosePage->setDefault(true);
    }
    break;
    }

    QString pageNum = QString("(%1/%2)").arg(m_currentPage + 1).arg(enTotalPage);
    ui.label_page_num->setText(pageNum);
#ifdef FOXRENDERFARM
	ui.label_page_num->hide();
#endif
}