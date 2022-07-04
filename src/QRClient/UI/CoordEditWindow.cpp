#include "stdafx.h"
#include "CoordEditWindow.h"
#include "ui_CoordEditWindow.h"

CoordEditWindow::CoordEditWindow(QWidget *parent)
    : Dialog(parent)
    , ui(new Ui::CoordEditWindowClass)
    , m_addPicBtn(new QPushButton("+"))
    , m_hspacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding))
{
    ui->setupUi(this);
    ui->label_logo->setPixmap(QPixmap(LOGO_ICON));
    m_addPicBtn->setFixedSize(80, 80);
    m_addPicBtn->setStyleSheet("QPushButton{border:1px solid #666666;color:#666666;background:transparent;font-size:50px;}"
                               "QPushButton:hover, QPushButton:pressed{border:1px solid #999999;color:#999999;}");

    connect(m_addPicBtn, SIGNAL(clicked()), this, SLOT(on_btnAddPicture_clicked()));

    addDefaultCtrl();
}

CoordEditWindow::~CoordEditWindow()
{
    QHBoxLayout* layout = (QHBoxLayout*)ui->widget_3->layout();
    for (int i = 0; i < layout->count(); i++) {
        QLayoutItem *it = layout->itemAt(i);
        if (it != nullptr) {
            CornerLabel *pWidget = qobject_cast<CornerLabel *>(it->widget());
            if (pWidget != nullptr) {
                pWidget->deleteLater();
            }
        }
    }

    delete ui;
}

void CoordEditWindow::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    } else
        Dialog::changeEvent(event);
}

void CoordEditWindow::on_btnAddPicture_clicked()
{
    // 添加图片item
    QStringList filters;
    filters << "Image files (*.jpg)" << "Any files (*)";
    QStringList files = RBHelper::getSelectedFiles(filters);


    showPictures(files);
}

void CoordEditWindow::on_btnEditOk_clicked()
{
    if (!checkSettingsEnable()) {
        return;
    }
    done(QDialog::Accepted);
}

CornerLabel* CoordEditWindow::addPicture(const QString& pic)
{
    CornerLabel *label = new CornerLabel;
    QImage image(pic);
    label->setPixmap(QPixmap::fromImage(image));
    label->setScaledContents(true);
    label->setFixedSize(80, 80);
    label->setPicPath(pic);  
    connect(label, SIGNAL(btnClicked(const QString&)), this, SLOT(onDeleteClicked(const QString&)));

    return label;
}

void CoordEditWindow::addDefaultCtrl()
{
    QHBoxLayout* lay = (QHBoxLayout*)ui->widget_3->layout();
    int count = lay->count();
    lay->addWidget(m_addPicBtn);
    lay->addSpacerItem(m_hspacerItem);
}

CoordValue CoordEditWindow::getCoordValue()
{
    QTableWidget* table = ui->tableWidgetSetCoord;
    CoordValue value;
    if (table->item(0, 0) != NULL) value.coord_x = table->item(0, 0)->text().toDouble();
    if (table->item(0, 1) != NULL) value.coord_y = table->item(0, 1)->text().toDouble();
    if (table->item(0, 2) != NULL) value.coord_z = table->item(0, 2)->text().toDouble();
    return value;
}

CoordEditInfo CoordEditWindow::getCoordEditInfo()
{
    CoordEditInfo info;
    info.name = ui->lineEditName->text();
    info.coordValue = getCoordValue();
    info.pictures = m_pictures.keys();

    return info;
}

void CoordEditWindow::setCoordEditInfo(const CoordEditInfo& info)
{
    // name
    ui->lineEditName->setText(info.name);

    // x y z
    showCoordValues(info.coordValue);

    // pictures
    showPictures(info.pictures);
}

void CoordEditWindow::showPictures(const QStringList& pics)
{
    QHBoxLayout* layout = (QHBoxLayout*)ui->widget_3->layout();

    for (int i = 0; i < pics.size(); i++) {
        QString pic = pics[i];
        if(m_pictures.find(pic) != m_pictures.end())
            continue;

        CornerLabel* item = addPicture(pic);
        if (m_pictures.count() > 5) break;
        m_pictures.insert(pic, item);   
        layout->insertWidget(0, item);
    }
}

void CoordEditWindow::showCoordValues(const CoordValue& values)
{
    QTableWidget* coord = ui->tableWidgetSetCoord;
    coord->blockSignals(true);

    int row = 0;
    // coord->insertRow(0);

    // x
    QTableWidgetItem *xItem = new QTableWidgetItem(QString::number(values.coord_x));
    xItem->setTextAlignment(Qt::AlignCenter);
    coord->setItem(row, 0, xItem);

    // y
    QTableWidgetItem *yItem = new QTableWidgetItem(QString::number(values.coord_y));
    yItem->setTextAlignment(Qt::AlignCenter);
    coord->setItem(row, 1, yItem);

    // z
    QTableWidgetItem *zItem = new QTableWidgetItem(QString::number(values.coord_z));
    zItem->setTextAlignment(Qt::AlignCenter);
    coord->setItem(row, 2, zItem);

    coord->blockSignals(false);
}

void CoordEditWindow::onDeleteClicked(const QString& path)
{
    if (m_pictures.isEmpty()) return;

    auto it = m_pictures.find(path);
    if (it == m_pictures.end()) return;

    m_pictures.remove(path);

    QHBoxLayout* layout = (QHBoxLayout*)ui->widget_3->layout();
    for (int i = 0; i < layout->count(); i++) {
        QLayoutItem *it = layout->itemAt(i);
        if (it != nullptr) {
            CornerLabel *pWidget = qobject_cast<CornerLabel *>(it->widget());
            if (pWidget != nullptr && pWidget->getPicPath() == path) {
                pWidget->hide();
            }
        }    
    }

    showPictures(m_pictures.keys());
}

bool CoordEditWindow::checkSettingsEnable()
{
    if (ui->lineEditName->text().isEmpty()) {
        new ToastWidget(this, QObject::tr("请输入名称!"), QColor("#e25c59"), 3000);
        return false;
    }

    if (m_pictures.isEmpty()) {
        new ToastWidget(this, QObject::tr("请添加图片!"), QColor("#e25c59"), 3000);
        return false;
    }



    return true;
}

//////////////////////////////////////////////////////////////////////////
CornerLabel::CornerLabel(QWidget *parent)
    : QLabel(parent)
    , m_closeBtn(new QPushButton(this))
{
    m_closeBtn->setStyleSheet("QPushButton{border-image:url(:/images/delete.png);border:"
        "none;background-color:transparent;max-height:15px;max-width:15px;padding:0px;}"
        "QPushButton:hover{border-image:url(:/images/delete.png);} ");
    m_closeBtn->hide();

    connect(m_closeBtn, SIGNAL(clicked()), this, SLOT(onBtnClicked()));
}

CornerLabel::~CornerLabel()
{
}

void CornerLabel::enterEvent(QEvent *e)
{ 
    QPoint pos = m_closeBtn->pos();
    pos.setX(this->width() - m_closeBtn->width());
    m_closeBtn->move(pos);
    m_closeBtn->show();
    QLabel::enterEvent(e);
}

void CornerLabel::leaveEvent(QEvent *e)
{
    m_closeBtn->hide();
    QLabel::leaveEvent(e);
}

void CornerLabel::paintEvent(QPaintEvent * ev)
{
    QLabel::paintEvent(ev);
}

void CornerLabel::onBtnClicked()
{
    emit btnClicked(m_picPath);
}