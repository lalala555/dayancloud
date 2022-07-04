#include "stdafx.h"
#include "EditableComboBox.h"

EditableComboBox::EditableComboBox(QWidget *parent)
    : QComboBox(parent)
    , m_mouseClicked(false)
    , m_boxEdit(nullptr)
    , m_editWidget(nullptr)
{
    //this->setStyleSheet("QComboBox QListView::item:hover{background-color: #ffffff;}");

    // model/view
    this->setEditable(true);
    m_listWidget = new QListWidget(this);
    this->setModel(m_listWidget->model());
    this->setView(m_listWidget);

    // slot
    connect(m_listWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(onListItemClicked(QListWidgetItem*)));
    // connect(this, SIGNAL(activated(int)), this, SLOT(onHighLightItem(int)));
}

EditableComboBox::~EditableComboBox()
{

}

void EditableComboBox::addEditItem()
{
    // 设置combobox的编辑框
    m_boxEdit = new QLineEdit(this);
    this->setLineEdit(m_boxEdit);
    m_boxEdit->setEnabled(false);
    m_boxEdit->setStyleSheet("background:transparent;border-width:0;border-style:outset");
    m_boxEdit->setFocusPolicy(Qt::NoFocus);
    QRegExp rx("^[0-9a-zA-Z_]{1,}$");
    QRegExpValidator *validator = new QRegExpValidator(rx, m_boxEdit);
    m_boxEdit->setValidator(validator);

    // 设置可编辑的控件
    m_editWidget = new ComboBoxEdit(this);
    m_editWidget->setAddBtnIcon(":/icon/images/icon/btn.png");
    m_editWidget->setSubBtnIcon(":/images/success.png");
    m_editWidget->setCancelBtnIcon(":/images/main/close_hover.png");
    m_editWidget->setFixedSize(this->width() - 10, 30);
    QListWidgetItem* widgetItem = new QListWidgetItem(m_listWidget);
    widgetItem->setFlags(Qt::ItemIsEnabled);
    m_listWidget->setItemWidget(widgetItem, m_editWidget);

    connect(m_editWidget, SIGNAL(submitClicked(const QString&)), this, SIGNAL(submitProjName(const QString&)));
}

void EditableComboBox::addBoxItem(const QString& txt, const QVariant& data, const QString& icon, int type)
{
    ComboBoxItem* boxItem = new ComboBoxItem(txt, data, icon, type, this);
    boxItem->setFocusPolicy(Qt::NoFocus);
    boxItem->setFixedSize(this->width() - 10, 30);
    QListWidgetItem* widgetItem = new QListWidgetItem(m_listWidget);
    widgetItem->setFlags(Qt::ItemIsEnabled);
    widgetItem->setData(Qt::UserRole, QVariant::fromValue(boxItem));
    widgetItem->setToolTip(txt);
    m_listWidget->setItemWidget(widgetItem, boxItem);
    connect(boxItem, SIGNAL(btnClicked(const QString&)), this, SIGNAL(deleteProject(const QString&)));
}

int EditableComboBox::currentIndex() const
{
    QString cutTxt = this->currentText();
    int count = m_listWidget->count();
    ComboBoxItem* boxItem = nullptr;
    for(int i = 0; i < count; i++) {
        boxItem = m_listWidget->item(i)->data(Qt::UserRole).value<ComboBoxItem*>();
        if(boxItem != nullptr) {
            QString txt = ComboBoxItem::elidText(boxItem->itemText(), m_boxEdit->font());
            if(!txt.compare(cutTxt)) {
                return i;
            }
        }
    }
    return -1;
}

void EditableComboBox::showPopup()
{
    QComboBox::showPopup();
}

void EditableComboBox::hidePopup()
{
    if(m_editWidget != nullptr)
        m_editWidget->onShowButton();
    m_listWidget->scrollToTop(); // 处理莫名奇妙的item消失问题
    QComboBox::hidePopup();
}

void EditableComboBox::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton) {
        m_mouseClicked = true;
    }
}

void EditableComboBox::mouseReleaseEvent(QMouseEvent *e)
{
    if(m_mouseClicked) {
        this->showPopup();
        m_mouseClicked = false;
    }
}

void EditableComboBox::onListItemClicked(QListWidgetItem *item)
{
    ComboBoxItem* boxItem = item->data(Qt::UserRole).value<ComboBoxItem*>();
    if(boxItem != nullptr) {
        m_boxEdit->setText(ComboBoxItem::elidText(boxItem->itemText(), m_boxEdit->font()));
        m_boxEdit->setToolTip(boxItem->itemText());
        this->hidePopup();
        emit currentIndexChanged(getCurrentIndex(boxItem->itemText()));
    }
}

void EditableComboBox::onHighLightItem(int index)
{
    if(index >= m_listWidget->count())
        return;

    ComboBoxItem *boxItem = m_listWidget->item(index)->data(Qt::UserRole).value<ComboBoxItem*>();
    if(boxItem != nullptr) {
        boxItem->setBtnVisible(true);
    }
}

void EditableComboBox::setCurrentEditText(const QString& text)
{
    if(m_boxEdit) {
        m_boxEdit->setText(ComboBoxItem::elidText(text, m_boxEdit->font()));
        m_boxEdit->setToolTip(text);
    }
}

void EditableComboBox::setCurrentIndex(int index)
{
    if(index >= m_listWidget->count())
        return;

    ComboBoxItem *boxItem = m_listWidget->item(index)->data(Qt::UserRole).value<ComboBoxItem*>();
    if(boxItem != nullptr) {
        m_boxEdit->setText(ComboBoxItem::elidText(boxItem->itemText(), m_boxEdit->font()));
        m_boxEdit->setToolTip(boxItem->itemText());
    }
}

QVariant EditableComboBox::getItemData(int index)
{
    QVariant vdata;
    if(index >= m_listWidget->count())
        return vdata;

    ComboBoxItem *boxItem = m_listWidget->item(index)->data(Qt::UserRole).value<ComboBoxItem*>();
    if(boxItem != nullptr) {
        vdata = boxItem->itemData();
    }
    return vdata;
}

int EditableComboBox::getCurrentIndex(const QString& text)
{
    int count = m_listWidget->count();
    ComboBoxItem* boxItem = nullptr;
    for(int i = 0; i < count; i++) {
        boxItem = m_listWidget->item(i)->data(Qt::UserRole).value<ComboBoxItem*>();
        if(boxItem != nullptr) {
            QString txt = boxItem->itemText();
            if(!txt.compare(text)) {
                return i;
            }
        }
    }
    return -1;
}

void EditableComboBox::clearItems()
{
    m_listWidget->clear();
}

void EditableComboBox::hideEidtWidget(int countType)
{
    if(countType == CHILD_ACCOUNT) {
        if(m_listWidget->count() > 1) {
            m_listWidget->item(0)->setHidden(true);
            LOGFMTE("[EditableComboBox] hideEidtWidget %d", countType);
        }
    }
}

void EditableComboBox::hidePopupWidget()
{
    this->hidePopup();
}

void EditableComboBox::paintEvent(QPaintEvent *p)
{
    QStylePainter style(this);
    // 用style画背景 (会使用setstylesheet中的内容)
    QStyleOption option;
    option.initFrom(this);
    option.rect=rect();
    style.drawPrimitive(QStyle::PE_Widget, option);
    QComboBox::paintEvent(p);
}

void EditableComboBox::wheelEvent(QWheelEvent * e)
{
    return;
}

void EditableComboBox::retranslateUi()
{
    m_editWidget->retranslateUi();
}

QString EditableComboBox::getCurrentText()
{
    int idx = this->currentIndex();
    if (idx >= m_listWidget->count() || idx <0)
        return QComboBox::currentText();

    ComboBoxItem *boxItem = m_listWidget->item(idx)->data(Qt::UserRole).value<ComboBoxItem*>();
    if (boxItem != NULL)
        return boxItem->itemText();

    return QComboBox::currentText();
}
//////////////////////////////////////////////////////////////////////////

ComboBoxItem::ComboBoxItem(QWidget *parent)
    : QWidget(parent)
{
    this->setAttribute(Qt::WA_Hover, true);
    m_btnStyle = QString("border:none;border-image:url(%1);max-width:12px;max-height:12px;");
    m_layout = new QHBoxLayout(this);
    m_pushBtn = new QPushButton(this);
    m_itemText = new QLabel(this);
}

ComboBoxItem::ComboBoxItem(const QString& txt, const QVariant& data, const QString& iconPath, int btnPos, QWidget *parent)
    : QWidget(parent)
    , m_text(txt)
    , m_iconPath(iconPath)
    , m_btnPos(btnPos)
    , m_data(data)
{
    m_layout = new QHBoxLayout(this);
    m_pushBtn = new QPushButton(this);
    m_itemText = new QLabel(this);
    m_itemText->setAlignment(Qt::AlignVCenter);

    this->setContentsMargins(0, 0, 0, 0);
    m_btnStyle = QString("border:none;border-image:url(%1);max-width:12px;max-height:12px;");
    this->initWidget();
}

ComboBoxItem::~ComboBoxItem()
{

}

QString ComboBoxItem::itemText()
{
    return m_text;
}

QVariant ComboBoxItem::itemData()
{
    return m_data;
}

void ComboBoxItem::setText(const QString& txt)
{
    m_text = txt;
    m_itemText->setText(m_text);
    this->initWidget();
}

void ComboBoxItem::setBtnIcon(const QString& icon)
{
    m_iconPath = icon;
    QString style = m_btnStyle.arg(icon);
    m_pushBtn->setStyleSheet(style);
    this->initWidget();
}

void ComboBoxItem::setBtnPosition(int btnPos)
{
    m_btnPos = btnPos;
    this->initWidget();
}

void ComboBoxItem::initWidget()
{
    QString txt = ComboBoxItem::elidText(m_text, m_itemText->font());
    m_itemText->setText(txt);
    QString style = m_btnStyle.arg(m_iconPath);
    m_pushBtn->setStyleSheet(style);

    m_layout->setContentsMargins(2, 0, 20, 0);
    m_layout->setAlignment(Qt::AlignVCenter);
    if(m_btnPos == Normal) {
        m_layout->addWidget(m_itemText);
    } else if(m_btnPos == Button_Head_Icon) {
        m_layout->addWidget(m_pushBtn, 2);
        m_layout->addWidget(m_itemText, 8);
    } else if(m_btnPos == Button_Tail_Icon) {
        m_layout->addWidget(m_itemText, 8);
        m_layout->addWidget(m_pushBtn, 2);
    }
    this->setLayout(m_layout);

    connect(m_pushBtn, SIGNAL(clicked()), this, SLOT(onBtnClicked()));
}

void ComboBoxItem::onBtnClicked()
{
    emit btnClicked(m_text);
}

void ComboBoxItem::enterEvent(QEvent *e)
{
    // m_pushBtn->show();
}

void ComboBoxItem::leaveEvent(QEvent *e)
{
    // m_pushBtn->hide();
}

void ComboBoxItem::setBtnVisible(bool visible)
{
    m_pushBtn->setVisible(visible);
}

QString ComboBoxItem::elidText(const QString& txt, const QFont& font, int maxWidth)
{
    QString elidtxt = txt;
    QFontMetrics titleMet(font);
    elidtxt = titleMet.elidedText(txt, Qt::ElideRight, maxWidth);
    return elidtxt;
}

void ComboBoxItem::paintEvent(QPaintEvent *p)
{
    QStylePainter style(this);
    // 用style画背景 (会使用setstylesheet中的内容)
    QStyleOption option;
    option.initFrom(this);
    option.rect=rect();
    style.drawPrimitive(QStyle::PE_Widget, option);
    QWidget::paintEvent(p);
}

//////////////////////////////////////////////////////////////////////////

ComboBoxEdit::ComboBoxEdit(QWidget *parent)
    : QWidget(parent)
{
    this->initUi();
}

ComboBoxEdit::ComboBoxEdit(const QString& addBtnIcon, const QString& subBtnIcon, const QString& cancelBtnIcon, QWidget *parent)
    : QWidget(parent)
    , m_addBtnIcon(addBtnIcon)
    , m_subBtnIcon(subBtnIcon)
    , m_cancelBtnIcon(cancelBtnIcon)
{
    this->initUi();
    this->initBtnStyle();
}

ComboBoxEdit::~ComboBoxEdit()
{

}

void ComboBoxEdit::initUi()
{
    m_btnStyle = QString("border:none;border-image:url(%1);max-width:%2px;max-height:%3px;");

    m_lineEdit = new LineEdit;
    m_lineEdit->setStyleSheet("background:transparent;border-width:0;border-style:outset");
    m_lineEdit->setPlaceholderText(QObject::tr("请输入项目名"));
    m_lineEdit->setToolTip(QObject::tr("请输入项目名"));
    m_lineEdit->setMaxLength(20);
    m_lineEdit->setFocusPolicy(Qt::StrongFocus);

    // 设置正则
    QRegExp rx("^[0-9a-zA-Z_]{1,}$");
    m_lineEdit->setRegExp(rx);
    m_lineEdit->setInvalidTooltip(QObject::tr("非法输入,只允许数字，字母或下划线组成，且不超过20个字符"));

    // 编辑区
    QSize size = QSize(12, 12);
    m_submit = new QPushButton(m_lineEdit);
    m_submit->setMinimumSize(size);
    m_submit->setMaximumSize(size);
    m_submit->setCursor(QCursor(Qt::PointingHandCursor));
    m_submit->setFlat(true);
    m_submit->setDefault(true);

    m_cancel = new QPushButton(m_lineEdit);
    m_cancel->setMinimumSize(size);
    m_cancel->setMaximumSize(size);
    m_cancel->setFlat(true);
    m_cancel->setCursor(QCursor(Qt::PointingHandCursor));

    m_editLayout = new QHBoxLayout(m_lineEdit);
    m_editLayout->setContentsMargins(0, 0, 0, 0);
    m_editLayout->addStretch();
    m_editLayout->addWidget(m_submit);
    m_editLayout->addWidget(m_cancel);
    // 设置输入框中文件输入区，不让输入的文字在被隐藏在按钮下
    m_lineEdit->setTextMargins(0, 1, size.width() * 2, 1);
    m_lineEdit->setLayout(m_editLayout);

    // 添加按钮
    m_addBtn = new QPushButton(QObject::tr("+新建项目"));
    m_addBtn->setSizeIncrement(this->size());
    m_addBtn->setFlat(true);

    // 设置布局
    m_mainLayout = new QHBoxLayout();
    m_mainLayout->setContentsMargins(0, 0, 20, 0);
    m_mainLayout->addWidget(m_lineEdit);
    m_mainLayout->addWidget(m_addBtn);

    this->setLayout(m_mainLayout);
    m_lineEdit->hide();

    connect(m_submit, SIGNAL(clicked()), this, SLOT(onSubmitClicked()));
    connect(m_cancel, SIGNAL(clicked()), this, SIGNAL(cancelClicked()));
    connect(m_addBtn, SIGNAL(clicked()), this, SLOT(onHideButton()));
    connect(m_cancel, SIGNAL(clicked()), this, SLOT(onShowButton()));
    connect(m_lineEdit, SIGNAL(returnPressed()), this, SLOT(onSubmitClicked()));
}

void ComboBoxEdit::initBtnStyle()
{
    if(!m_addBtnIcon.isEmpty()) {
        this->setAddBtnIcon(m_addBtnIcon);
    }
    if(!m_subBtnIcon.isEmpty()) {
        this->setAddBtnIcon(m_addBtnIcon);
    }
    if(!m_cancelBtnIcon.isEmpty()) {
        this->setAddBtnIcon(m_addBtnIcon);
    }
}

void ComboBoxEdit::setAddBtnIcon(const QString& addBtnIcon)
{
    m_addBtnIcon = addBtnIcon;
    // add btn
    QSize addBtnSize = m_addBtn->sizeHint();
    QString addStyle = m_btnStyle.arg(m_addBtnIcon).arg(m_addBtn->width()).arg(m_addBtn->height());
    m_addBtn->setStyleSheet(addStyle);
}

void ComboBoxEdit::setSubBtnIcon(const QString& subBtnIcon)
{
    m_subBtnIcon = subBtnIcon;
    // submit btn
    QSize subBtnSize = m_submit->sizeHint();
    QString subStyle = m_btnStyle.arg(m_subBtnIcon).arg(12).arg(12);
    m_submit->setStyleSheet(subStyle);
}

void ComboBoxEdit::setCancelBtnIcon(const QString& cancelBtnIcon)
{
    m_cancelBtnIcon = cancelBtnIcon;
    // cancel btn
    QSize cancelBtnSize = m_cancel->sizeHint();
    QString cancelStyle = m_btnStyle.arg(m_cancelBtnIcon).arg(12).arg(12);
    m_cancel->setStyleSheet(cancelStyle);
}

void ComboBoxEdit::paintEvent(QPaintEvent *p)
{
    QStylePainter style(this);
    // 用style画背景 (会使用setstylesheet中的内容)
    QStyleOption option;
    option.initFrom(this);
    option.rect=rect();
    style.drawPrimitive(QStyle::PE_Widget, option);
    QWidget::paintEvent(p);
}

void ComboBoxEdit::onHideButton()
{
    m_addBtn->hide();
    m_lineEdit->clear();
    m_lineEdit->show();
    m_lineEdit->setFocus();
    m_submit->setDefault(true);
    m_cancel->setShortcut(Qt::Key_No);
}

void ComboBoxEdit::onShowButton()
{
    m_addBtn->show();
    m_lineEdit->hide();
}

void ComboBoxEdit::onSubmitClicked()
{
    QString projName = m_lineEdit->text().trimmed();
    if(projName.isEmpty())
        return;
    emit submitClicked(projName);
}

void ComboBoxEdit::keyPressEvent(QKeyEvent *e)
{
    int key = e->key();
    if(key == Qt::Key_Enter || key == Qt::Key_Return) {
        this->onSubmitClicked();
        e->accept();
    }
    QWidget::keyPressEvent(e);
}

void ComboBoxEdit::retranslateUi()
{
    m_lineEdit->setPlaceholderText(QObject::tr("请输入项目名"));
    m_lineEdit->setInvalidTooltip(QObject::tr("非法输入,只允许数字，字母或下划线组成，且不超过20个字符"));
    m_lineEdit->setToolTip(QObject::tr("请输入项目名"));
    m_addBtn->setText(QObject::tr("+新建项目"));
}
