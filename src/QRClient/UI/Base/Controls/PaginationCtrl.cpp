#include "stdafx.h"
#include "PaginationCtrl.h"

PaginationCtrl::PaginationCtrl(QWidget *parent)
    : QWidget(parent)
    , m_currentPage(1)
    , m_totalPage(1)
    , m_btnCount(5)
    , m_displayItemCount(16)
    , m_totalSize(0)
{
    ui.setupUi(this);

    connect(ui.lineEdit_jump_page, &QLineEdit::editingFinished, this, &PaginationCtrl::onPageEditFinished);
#ifdef FOXRENDERFARM
	ui.comboBox_counts->hide();
	ui.widget_jump_page->setFixedWidth(130);
	ui.label_3->setFixedWidth(60);
	ui.label_4->setFixedWidth(40);
	ui.lineEdit_jump_page->setFixedWidth(30);
#endif
}

PaginationCtrl::~PaginationCtrl()
{
}

void PaginationCtrl::setPageInfo(int totalSize, int displayCount, int btnCount)
{
    m_totalPage = totalSize;
    m_btnCount = btnCount;
    m_displayItemCount = displayCount;
    m_totalPage = getTotalPageCount(totalSize, displayCount);
    this->setTotalCountText(totalSize);
    this->setDisplayItemCount(QList<qint32>() << displayCount);
    initPageBtn();

    // 输入框限制输入类型
    QIntValidator *validator = new QIntValidator(ui.lineEdit_jump_page);
    validator->setRange(0, m_totalPage);
    ui.lineEdit_jump_page->setValidator(validator);
}

qint32 PaginationCtrl::getCurrDisplayItemCount()
{
    return m_displayItemCount;
}

void PaginationCtrl::setDisplayItemCount(const QList<qint32>& displays)
{
    if (displays.isEmpty())
        return;

    ui.comboBox_counts->clear();

    foreach(qint32 count, displays) {
#ifdef FOXRENDERFARM
		QString text = QString("%1/page").arg(count);
#else 
		QString text = QObject::tr("%1条/页").arg(count);
#endif          
        ui.comboBox_counts->addItem(text, count);
    }
}

void PaginationCtrl::initPageBtn()
{
    // 移除旧的按钮
    QHBoxLayout* layout = qobject_cast<QHBoxLayout*>(ui.widget_btns->layout());
    int count = layout->count();
    for (int i = 0; i < count; i++) {
        QLayoutItem *item = layout->itemAt(i);
        if(item == nullptr) continue;
        QPushButton* btn = qobject_cast<QPushButton*>(item->widget());

        if (btn != nullptr) {
            btn->hide();
            btn->deleteLater();
        }
    }

    // 添加新按钮
    QVector<QPushButton*> btns = getPageBtns(getTotalPageSize());
    foreach(QPushButton* btn, btns) {
        layout->addWidget(btn);     
    }
    // 更新ui.widget_btns宽度
    ui.widget_btns->setFixedWidth(btns.size() * 30);

    // 设置按钮风格
    setPageBtnDynamicPos(getTotalPageSize(), getCurrentPage());
}

void PaginationCtrl::on_btnPagePrev_clicked()
{
    qint32 total = getTotalPageSize();
    m_currentPage = m_currentPage - 1;
    if (m_currentPage <= 0)
        m_currentPage = 1;

    setPageBtnDynamicPos(total, m_currentPage);

    emit pageChanged(m_currentPage, m_displayItemCount);
}

void PaginationCtrl::on_btnPageNext_clicked()
{
    qint32 total = getTotalPageSize();
    m_currentPage = m_currentPage + 1;
    if (m_currentPage >= total)
        m_currentPage = total;

    setPageBtnDynamicPos(total, m_currentPage);

    emit pageChanged(m_currentPage, m_displayItemCount);
}

void PaginationCtrl::onPageEditFinished()
{
    if (ui.lineEdit_jump_page->text().isEmpty())
        return;

    qint32 total = getTotalPageSize();
    qint32 page = ui.lineEdit_jump_page->text().toInt();
    if (page > getTotalPageSize()) {
        m_currentPage = total;
    } else if (page <= 0) {
        m_currentPage = 1;
    } else {
        m_currentPage = page;
    }

    setPageBtnDynamicPos(total, m_currentPage);

    emit pageChanged(m_currentPage, m_displayItemCount);
}

void PaginationCtrl::onPageBtnClicked()
{
    QObject* recv = (QObject*)sender();
    if (recv == nullptr) return;

    QPushButton *btn = qobject_cast<QPushButton*>(recv);
    qint32 page = btn->property("page").toInt();
    if (btn->text() == "...") return;

    qint32 total = getTotalPageSize();
    m_currentPage = page;
    setPageBtnDynamicPos(total, m_currentPage);

    emit pageChanged(m_currentPage, m_displayItemCount);
}

void PaginationCtrl::on_comboBox_counts_currentIndexChanged(int index)
{
    QVariant data = ui.comboBox_counts->itemData(index);
    if (!data.isValid()) return;

    m_displayItemCount = data.toInt();

    emit displayItemCountChanged(m_displayItemCount);
}

QVector<QPushButton*> PaginationCtrl::getPageBtns(int totalPage)
{
    QVector<QPushButton*> btns;
    qint32 page = totalPage > 5 ? 5 : totalPage;

    for (int i = 0; i < page; i++) {
        QPushButton* btn = new QPushButton(QString::number(i + 1), ui.widget_btns);
        connect(btn, &QPushButton::clicked, this, &PaginationCtrl::onPageBtnClicked);
        btn->setProperty("page", i + 1);
        QString objName = QString("btn_%1").arg(QString::number(i + 1));
        btn->setObjectName(objName);
        btn->setStyleSheet(getBtnNormalStyle());
        btn->setFixedSize(24, 24);
        btns.append(btn);
    }

    return btns;
}

QString PaginationCtrl::getBtnNormalStyle()
{
    QString btnStyle = "QPushButton{background-color: transparent; border: 1px solid #4a4952; color:#4a4952; border-radius:0px;}\
                        QPushButton:hover{border: 1px solid #675ffe; color:#675ffe;}\
                        QPushButton:pressed{border: 1px solid #675ffe; color:#675ffe;}";

    return btnStyle;
}

QString PaginationCtrl::getBtnSelectedStyle()
{
    QString btnStyle = "QPushButton{background-color: transparent; border: 1px solid #675ffe; color:#675ffe; border-radius:0px;}\
                        QPushButton:hover{border: 1px solid #675ffe; color:#675ffe;}\
                        QPushButton:pressed{border: 1px solid #675ffe; color:#675ffe;}";

    return btnStyle;
}

qint32 PaginationCtrl::getTotalPageSize()
{
    if (m_totalPage <= 0)
        return 1;
    return m_totalPage;
}

qint32 PaginationCtrl::getCurrentPage()
{
    if (m_currentPage <= 0)
        return 1;
    return m_currentPage;
}

void PaginationCtrl::setPageBtnDynamicPos(int totalPage, int curPage)
{
    // lineedit
    ui.lineEdit_jump_page->setText(QString::number(curPage));

    // 按钮
    QHBoxLayout* layout = qobject_cast<QHBoxLayout*>(ui.widget_btns->layout());
    int count = layout->count();
    // total < 5 -> 1 2 3 4 5
    if (totalPage <= 5) {
        for (int i = 0; i < count; i++) {
            QLayoutItem *item = layout->itemAt(i);
            if (item != NULL && item->widget() != nullptr) {
                QPushButton* btn = qobject_cast<QPushButton*>(item->widget());
                qint32 page = btn->property("page").toInt();
                btn->setStyleSheet(getBtnNormalStyle());
                if (page == curPage) {
                    btn->setStyleSheet(getBtnSelectedStyle());
                }
            }
        }
    } else {
        // total > 5 
        // curpage <= 2 || curpage >= total-1 -> 1 2 .. total-1 total
        // curpage > 2 && curpage < total-1   -> 1 .. curpage .. total
        for (int i = 0; i < count; i++) {
            QLayoutItem *item = layout->itemAt(i);
            if (item != NULL && item->widget() != nullptr) {
                QPushButton* btn = qobject_cast<QPushButton*>(item->widget());
                QString btnName = btn->objectName();
                qint32 page = btn->property("page").toInt();
                btn->setStyleSheet(getBtnNormalStyle());

                if (curPage <= 2 || curPage >= totalPage - 1) {                   
                    if (btnName == "btn_1") {
                        btn->setText(QString::number(1));
                        btn->setProperty("page", 1);

                    } else if (btnName == "btn_3") {// 省略号
                        btn->setText("...");
                        btn->setStyleSheet("QPushButton{background-color: transparent; border: none; color:#4a4952; border-radius:0px;}");

                    } else if (btnName == "btn_2") {
                        btn->setText(QString::number(2));
                        btn->setProperty("page", 2);

                    } else if (btnName == "btn_4") {
                        btn->setText(QString::number(totalPage - 1));
                        btn->setProperty("page", totalPage - 1);

                    } else if (btnName == "btn_5") {
                        btn->setText(QString::number(totalPage));
                        btn->setProperty("page", totalPage);
                    }

                    // 选中
                    if (page == curPage) {
                        btn->setStyleSheet(getBtnSelectedStyle());
                    }

                } else {
                    // 省略号
                    if (btnName == "btn_2" || btnName == "btn_4") {
                        btn->setText("...");
                        btn->setStyleSheet("QPushButton{background-color: transparent; border: none; color:#4a4952; border-radius:0px;}");

                    } else if (btnName == "btn_3") { // 中间按钮
                        btn->setText(QString::number(curPage));
                        btn->setProperty("page", curPage);
                        btn->setStyleSheet(getBtnSelectedStyle());

                    } else if (btnName == "btn_1") { // 首尾按钮 
                        btn->setText(QString::number(1));
                        btn->setProperty("page", 1);

                    } else if (btnName == "btn_5") {
                        btn->setText(QString::number(totalPage));
                        btn->setProperty("page", totalPage);
                    }
                }
            }
        }
    }
}

void PaginationCtrl::setTotalCountText(int total)
{
#ifdef FOXRENDERFARM
	QString text = QString("Total %1").arg(total);
#else 
	QString text = QObject::tr("共%1条").arg(total);
#endif
    ui.label_total->setText(text);
}

void PaginationCtrl::resizeEvent(QResizeEvent * event)
{
    QWidget::resizeEvent(event);
}

void PaginationCtrl::showEvent(QShowEvent * event)
{
    QWidget::showEvent(event);
}

void PaginationCtrl::paintEvent(QPaintEvent * p)
{
    QStylePainter painter(this);
    //用style画背景 (会使用setstylesheet中的内容)
    QStyleOption opt;
    opt.initFrom(this);
    opt.rect = rect();
    painter.drawPrimitive(QStyle::PE_Widget, opt);

    QWidget::paintEvent(p);
}

qint32 PaginationCtrl::getTotalPageCount(int totalSize, int itemCount)
{
    if (itemCount == 0) {
        itemCount = 16;
    }
    qint32 count = totalSize;
    qint32 pageSize = (count % itemCount ? (count / itemCount) + 1 : count / itemCount);

    return pageSize > 0 ? pageSize : 1;
}

qint32 PaginationCtrl::getRowCount()
{
    return m_displayItemCount;
}