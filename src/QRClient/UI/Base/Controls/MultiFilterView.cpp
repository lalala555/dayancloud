#include "stdafx.h"
#include "MultiFilterView.h"

MultiFilterView::MultiFilterView(QWidget *parent)
    : BasePopup(parent)
    , m_enable(false)
    , m_width(240)
    , m_height(320)
{
    ui.setupUi(this);

    setWindowFlags( Qt::FramelessWindowHint | Qt::Popup);
    setAttribute(Qt::WA_TranslucentBackground, true);
    this->setMouseTracking(true);
    ui.mainWidget->setMouseTracking(true);
    ui.widgetList->setMouseTracking(true);
    ui.searchEdit->setMouseTracking(true);
    ui.btnOk->setMouseTracking(true);
    ui.btnCancel->setMouseTracking(true);
    m_revealAction = ui.searchEdit->addAction(QIcon(":/images/magnifier_normal.png"), QLineEdit::LeadingPosition);
    connect(m_revealAction, SIGNAL(triggered()), this, SLOT(onRevealTriggered()));
    connect(ui.searchEdit, SIGNAL(returnPressed()), this, SLOT(onRevealTriggered()));

    connect(ui.btnOk, SIGNAL(clicked()), this, SIGNAL(okBtnClickedSignal()));
    connect(ui.btnCancel, SIGNAL(clicked()), this, SLOT(hide()));

    this->initSelectAllCheckBox();

    ui.filterList->setStyleSheet("QListWidget{background-color: #33333f; selection-background-color: #666666;}"
                                 "QListWidget::item{color: #ffffff;}"
                                 "QListWidget::item:selected{color:#ffffff; background-color: #666666;}");
}

MultiFilterView::~MultiFilterView()
{

}

void MultiFilterView::addFilterItem(const QString& txt, const QVariant& data, int defaultState)
{
    auto it = m_items.find(data);
    if(it == m_items.end()){
        QCheckBox* checkBox = this->createFilterItem(txt, data, defaultState);
        QListWidgetItem* widgetItem = new QListWidgetItem(ui.filterList);
        widgetItem->setData(Qt::UserRole, QVariant::fromValue(checkBox));
        ui.filterList->setItemWidget(widgetItem, checkBox);
        m_items.insert(data, checkBox);
    } else {
        it.value()->setText(txt);
        it.value()->setCheckState((Qt::CheckState)defaultState);
        it.value()->setProperty(txt.toLocal8Bit(), data);
    }
}

void MultiFilterView::addFilterItem(const QStringList& list, int defaultState)
{
    if(list.isEmpty())
        return;
    foreach(QString item, list) {
        this->addFilterItem(item, defaultState);
    }
}

QList<QVariant> MultiFilterView::getSelectItemProperty()
{
    QList<QVariant> list;
    if(m_checkAll->checkState() == Qt::Checked && ui.filterList->count() == getAllCheckBox().count()) {
        QVariant data = m_checkAll->property(m_checkAll->text().toLocal8Bit());
        list.append(data);
        return list;

    } else {
        QList<QCheckBox*> boxs = this->getAllCheckBox();
        foreach(QCheckBox* box, boxs) {
            QVariant data = box->property(box->text().toLocal8Bit());
            if(data.toInt() != TYPE_SHOW_ALLD) {
                if(box->isChecked()) {
                    list.append(data);
                }
            }
        }
        return list;
    }
}

QList<QVariant> MultiFilterView::getSelectItemsProperty()
{
    QList<QVariant> list;
    QList<QCheckBox*> boxs = this->getAllCheckBox();
    foreach(QCheckBox* box, boxs) {
        QVariant data = box->property(box->text().toLocal8Bit());
        if(data.toInt() != TYPE_SHOW_ALLD) {
            if(box->isChecked()) {
                list.append(data);
            }
        }
    }
    return list;
}

QCheckBox* MultiFilterView::createFilterItem(const QString& txt, const QVariant& data, int checkState)
{
    QCheckBox* item = new QCheckBox(txt);
    item->setStyleSheet("QCheckBox{color:#ffffff}");
    item->setMouseTracking(true);
    item->setCheckState((Qt::CheckState)checkState);
    item->setProperty(txt.toLocal8Bit(), data);
    connect(item, SIGNAL(clicked(bool)), this, SLOT(onCheckStateChanged(bool)));
    return item;
}

void MultiFilterView::onRevealTriggered()
{
    QString txt = ui.searchEdit->text();
    int rowCount = ui.filterList->count();
    // txt为空，显示所有行
    if(txt.isEmpty()) {
        for(int i = 0; i < rowCount; i++) {
            ui.filterList->setRowHidden(i, false);
        }

    } else {
        // 隐藏所有行(全选除外)
        for(int i = 1; i < rowCount; i++) {
            ui.filterList->setRowHidden(i, true);
        }

        // 找到符合的行
        QList<QListWidgetItem*> items = this->findAdaptItems(txt);
        // 显示符合的行
        foreach(QListWidgetItem* item, items) {
            ui.filterList->setItemHidden(item, false);
        }
    }
}

void MultiFilterView::onCheckStateChanged(bool state)
{
    QCheckBox* box = qobject_cast<QCheckBox*>(sender());
    QString name = box->text();
    QVariant data = box->property(name.toLocal8Bit());
    if(box != nullptr) {
        if(data.toInt() == TYPE_SHOW_ALLD) {
            this->updateAllCheckBoxState(state ? Qt::Checked : Qt::Unchecked);
        } else {
            this->updateCheckAllBox();
        }
    }
    
    if (m_enable) {
        emit okBtnClickedSignal();
    }
}

void MultiFilterView::updateCheckAllBox()
{
    QList<QCheckBox*> list = this->getAllCheckBox();
    if(list.isEmpty())
        return;

    int checked = 0;
    int totalCount = list.count();
    if(list.contains(m_checkAll)) {
        totalCount--;
    }

    foreach(QCheckBox* box, list) {
        if(box != nullptr) {
            QVariant data = box->property(box->text().toLocal8Bit());
            if(data.toInt() == TYPE_SHOW_ALLD) {
                continue;
            }

            if(box->checkState() == Qt::Checked) {
                checked ++;
            }
        }
    }

    if(checked == totalCount) {
        m_checkAll->setCheckState(Qt::Checked);
    } else if(checked > 0 && checked < totalCount) {
        m_checkAll->setCheckState(Qt::PartiallyChecked);
    } else if(checked == 0) {
        m_checkAll->setCheckState(Qt::Unchecked);
    }
}

QList<QCheckBox*> MultiFilterView::getAllCheckBox()
{
    QList<QCheckBox*> list;
    int count = ui.filterList->count();
    for(int i = 0; i < count; i++) {
        QListWidgetItem* item = ui.filterList->item(i);
        if(item != nullptr && !item->isHidden()) {
            QCheckBox* box = item->data(Qt::UserRole).value<QCheckBox*>();
            if(box != nullptr) {
                list.append(box);
            }
        }
    }
    return list;
}

void MultiFilterView::updateAllCheckBoxState(int state)
{
    QList<QCheckBox*> list = this->getAllCheckBox();
    if(list.isEmpty())
        return;

    foreach(QCheckBox* box, list) {
        if(box != nullptr) {
            box->setCheckState((Qt::CheckState)state);
        }
    }
}

void MultiFilterView::clearAllItems()
{
    return; // todo
    int count = ui.filterList->count();
    for(int i = 0; i < count; i++) {
        QListWidgetItem *item = ui.filterList->item(0);
        if(item != nullptr) {
            ui.filterList->removeItemWidget(item);
            delete item;
            item = nullptr;
        }
    }
    this->initSelectAllCheckBox();
    this->retranslateUi();
}

void MultiFilterView::paintEvent(QPaintEvent *p)
{
    QStylePainter style(this);
    // 用style画背景 (会使用setstylesheet中的内容)
    QStyleOption option;
    option.initFrom(this);
    option.rect = rect();
    style.drawPrimitive(QStyle::PE_Widget, option);
    QFrame::paintEvent(p);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::transparent, 2));
    painter.setBrush(QColor("#33333F"));
    int sectionWidth = m_sectionRect.width() / 2;
    QPolygonF pw;
    pw.append(QPointF(0, 10));
    pw.append(QPointF(sectionWidth - 10, 10));
    pw.append(QPointF(sectionWidth, 0));
    pw.append(QPointF(sectionWidth + 10, 10));
    pw.append(QPointF(m_width, 10));
    pw.append(QPointF(m_width, m_height));
    pw.append(QPointF(0, m_height));
    painter.drawPolygon(pw);
}

void MultiFilterView::setHeaderSectionRect(const QRect& rect)
{
    m_sectionRect = rect;
}

QList<QListWidgetItem*> MultiFilterView::findAdaptItems(const QString& txt)
{
    QList<QListWidgetItem*> list;
    int count = ui.filterList->count();
    for(int i = 0; i < count; i++) {
        QListWidgetItem* item = ui.filterList->item(i);
        if(item != NULL ) {
            QCheckBox* box = item->data(Qt::UserRole).value<QCheckBox*>();
            if(box != NULL && box->text().contains(txt)) {
                list.append(item);
            }
        }
    }
    return list;
}

void MultiFilterView::showView()
{
    if(this->isVisible()) {
        this->hide();
    } else {
        this->show();
        ui.searchEdit->clear();
        ui.searchEdit->setFocus();
        this->activateWindow();
    }
}

void MultiFilterView::updateItemState(const QVariant& data, int dataType)
{
    QList<QCheckBox*> boxList = this->getAllCheckBox();
    if(boxList.isEmpty())
        return;

    // 找到匹配项, 并勾选
    foreach(QCheckBox* box, boxList) {
        if(box != nullptr) {
            QVariant boxProp = box->property(box->text().toLocal8Bit());
            if(boxProp == data) {
                box->setCheckState(Qt::Checked);
                break;
            }
        }
    }

    this->updateCheckAllBox();
}

void MultiFilterView::retranslateUi()
{
    m_checkAll->setText(QObject::tr("全选"));
}

void MultiFilterView::changeEvent(QEvent *event)
{
    switch(event->type()) {
    case QEvent::LanguageChange: {
        ui.retranslateUi(this);
        initSelectAllCheckBox();
    }
    break;
    default:
        QWidget::changeEvent(event);
        break;
    }
}

void MultiFilterView::initSelectAllCheckBox()
{
    auto it = m_items.find(TYPE_SHOW_ALLD);
    if(it == m_items.end()){
        m_checkAll = createFilterItem(QObject::tr("全选"), TYPE_SHOW_ALLD, Qt::Checked);
        QListWidgetItem* widgetItem = new QListWidgetItem(ui.filterList);
        widgetItem->setData(Qt::UserRole, QVariant::fromValue(m_checkAll));
        ui.filterList->setItemWidget(widgetItem, m_checkAll);
        m_items.insert(TYPE_SHOW_ALLD, m_checkAll);
    }else{
        m_checkAll->setText(QObject::tr("全选"));
        m_checkAll->setCheckState(Qt::Checked);
        it.value()->setProperty(QObject::tr("全选").toLocal8Bit(), TYPE_SHOW_ALLD);
    }
}

void MultiFilterView::hideSearchBox()
{
    ui.searchEdit->hide();
}

void MultiFilterView::hideOkCancelBtn()
{
    ui.btnsWidget->hide();
}

void MultiFilterView::setCheckBoxEnableFilter(bool enable)
{
    m_enable = enable;
    if (enable) {
        hideOkCancelBtn();
    }
}

void MultiFilterView::setCustomStyleSheet(const QString& style)
{
    this->setStyleSheet(style);
}

void MultiFilterView::setViewSize(int width, int height)
{
    m_width = width;
    m_height = height;

    this->setFixedSize(width, height);
}