#include "stdafx.h"
#include "srswindow.h"
#include "ui_srswindow.h"
#include "Common/csv.h"
#include "userrswindow.h"

SRSWindow::SRSWindow(const QString& sysDefine, const QString& regKey, QWidget *parent)
    : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
    , ui(new Ui::SRSWindow())
    , m_totalRSCount(0)
    , m_sysDefine(sysDefine)
    , m_treeItemENU(nullptr)
    , m_regKey(regKey)
    , m_userTreeItem(nullptr)
{
    ui->setupUi(this);
    ui->btnOk->setEnabled(false);

    // 限制中文输入
    QRegExp reg("^[0-9a-zA-Z_@.-\\(\\):]{1,}$");
    QRegExpValidator *regVal = new QRegExpValidator(reg);
    ui->lineEditFilter->setValidator(regVal);

    connect(ui->lineEditFilter, &QLineEdit::textChanged, [=](const QString &text) {
        m_totalRSCount = 0;
        QTreeWidgetItemIterator it(ui->treeWidget);
        while(*it) {
            auto a = (*it)->text(0);
            if((*it)->parent()) {
                if((*it)->text(0).toLower().contains(text.toLower()) || (*it)->text(1).toLower().contains(text.toLower())) {
                    (*it)->setHidden(false);
                    m_totalRSCount++;

                    QTreeWidgetItem *item = *it;
                    while(item->parent()) {
                        item->parent()->setHidden(false);
                        item = item->parent();
                    }
                } else {
                    (*it)->setHidden(true);
                }
            }
            ++it;
        }
        updateGroupTitle();
    });

    connect(ui->treeWidget, &QTreeWidget::itemClicked, this, &SRSWindow::onItemClicked);
    connect(ui->treeWidget, &QTreeWidget::itemChanged, this, &SRSWindow::onItemClicked);
    connect(ui->lineEditSRS, &QLineEdit::textChanged, [=](const QString& text) {
        ui->btnOk->setEnabled(!text.isEmpty());
    });
    connect(ui->editLat, &QLineEdit::textChanged, this, &SRSWindow::onLatLngChanged);
    connect(ui->editLng, &QLineEdit::textChanged, this, &SRSWindow::onLatLngChanged);

    initUi();
    initData();
    updateGroupTitle();
}

SRSWindow::~SRSWindow()
{
    delete ui;
}

void SRSWindow::initUi()
{
    QTreeWidgetItem *treeItem = nullptr;
    treeItem = addTreeRoot(QObject::tr("ENU 坐标系"));
    treeItem->setData(0, Qt::UserRole, SRS_ENU);
    treeItem->setExpanded(true);
    m_treeItemENU = addTreeChild(treeItem, "Local Esae-North-Up(ENU)");

    treeItem = addTreeRoot(QObject::tr("地理坐标系"));
    treeItem->setData(0, Qt::UserRole, SRS_GCS);
    QList<QStringList> gcs = CSV::parseFromFile(":/Resource/srs/gcs.csv");
    for(int i = 1; i < gcs.count(); i++) {
        auto cs = gcs.at(i);
        auto item = addTreeChild(treeItem, QString("%1 - %2").arg(cs[1]).arg(cs[3]), QString("EPSG:%1").arg(cs[0]));
        item->setData(0, Qt::UserRole, QVariant::fromValue(cs[1].trimmed()));

        m_gcsDatabase[cs[1]] = cs[3];
    }

    treeItem = addTreeRoot(QObject::tr("投影坐标系"));
    treeItem->setData(0, Qt::UserRole, SRS_PCS);
    treeItem->setExpanded(true);
    QList<QStringList> pcs = CSV::parseFromFile(":/Resource/srs/pcs.csv");
    for(int i = 1; i < pcs.count(); i++) {
        auto cs = pcs.at(i);
        auto item = addTreeChild(treeItem, QString("%1").arg(cs[1]), QString("EPSG:%1").arg(cs[0]));
        item->setData(0, Qt::UserRole, QVariant::fromValue(cs[1].split("/")[0].trimmed()));
    }

    treeItem = addTreeRoot(QObject::tr("笛卡尔坐标系"));
    treeItem->setData(0, Qt::UserRole, SRS_CCS);
    treeItem->setExpanded(true);
    addTreeChild(treeItem, "ECEF - Earth-Centered, Earth-Fixed", "EPSG:4978");
    addTreeChild(treeItem, "Local coordinate system", "Local:0");

    // 用户自定义坐标系
    if (LocalSetting::getInstance()->getEnableUserCoord() == 1) {
        treeItem = addTreeRoot(QObject::tr("用户自定义坐标系"));
        treeItem->setData(0, Qt::UserRole, SRS_USR);
        treeItem->setExpanded(true);
        m_userTreeItem = treeItem;
        addTreeChild(treeItem, QObject::tr("定义新的坐标系"));
        QList<UserCoordSystem> usrCoords = CProfile::Inst()->getUserCoordSystems();
        foreach(UserCoordSystem coord, usrCoords) {
            QString coord_name = QString::fromStdString(coord.coord_name);
            QString content = QString::fromStdString(coord.coord_system_content);
            QString define = QString::fromStdString(coord.coord_definition);
            auto item = addTreeChild(treeItem, coord_name, define);
            item->setData(0, Qt::UserRole, QVariant::fromValue(content));
        }
    }
}

void SRSWindow::initData()
{
    if(m_sysDefine.isEmpty())
        ui->treeWidget->setCurrentItem(m_treeItemENU);
    ui->treeWidget->scrollTo(ui->treeWidget->currentIndex());
    ui->treeWidget->itemClicked(ui->treeWidget->currentItem(), 0);
}

void SRSWindow::updateGroupTitle()
{
    ui->groupBoxRS->setTitle(QObject::tr("空间参考系统 : %1 项").arg(m_totalRSCount));
}

QTreeWidgetItem* SRSWindow::addTreeRoot(QString name)
{
    QFont font;
    font.setWeight(75);

    QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->treeWidget);
    treeItem->setText(0, name);
    treeItem->setFont(0, font);
    treeItem->setFlags(treeItem->flags() ^ Qt::ItemIsSelectable);

    return treeItem;
}

QTreeWidgetItem* SRSWindow::addTreeChild(QTreeWidgetItem *parent, QString name, QString desc)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem();
    treeItem->setText(0, name);
    treeItem->setText(1, desc);
    parent->addChild(treeItem);

    if(m_sysDefine == desc) {
        ui->treeWidget->setCurrentItem(treeItem);
    }

    m_totalRSCount++;

    return treeItem;
}

void SRSWindow::onItemClicked(QTreeWidgetItem *item, int column)
{
    if (item == nullptr) return;

    auto parentItem = item->parent();
    if(parentItem == nullptr) return;
    int type = parentItem->data(0, Qt::UserRole).toInt();
    switch(type) {
    case SRS_ENU:
        ui->stackedWidget->setCurrentIndex(0);
        break;
    case SRS_GCS:
    case SRS_PCS:
    {
        ui->stackedWidget->setCurrentIndex(1);

        ui->labelDatum->clear();
        auto userData = item->data(0, Qt::UserRole);
        if(userData.isValid()) {
            QString datum = m_gcsDatabase[userData.toString()];
            if(datum.isEmpty()) datum = userData.toString();
            ui->labelDatum->setText(datum);
        }
    }
    break;
    case SRS_CCS:
        ui->stackedWidget->setCurrentIndex(2);
        break;
    case SRS_USR: {
        ui->stackedWidget->setCurrentIndex(3);

        QString userData = item->data(0, Qt::UserRole).toString();
        userData = QString::fromLocal8Bit(QByteArray::fromBase64(userData.toLocal8Bit()));
        ui->plainTextEdit->clear();
        ui->plainTextEdit->insertPlainText(userData);
    }       
        break;
    default:
        break;
    }
    ui->labelType->setText(parentItem->text(0));

    // 显示选择项

    QString sysName = item->text(0);
    QString sysDefine = item->text(1);

    QString srsFullName;
    if(sysDefine.isEmpty()) srsFullName = sysName;
    else srsFullName = QString("%1 (%2)").arg(sysName).arg(sysDefine);
    ui->lineEditSRS->setText(srsFullName);

    m_coordSys = { srsFullName, sysName, sysDefine };
}

void SRSWindow::on_btnEdit_clicked()
{
    QScopedPointer<UserRSWindow> dlg(new UserRSWindow());
    dlg->initWindow(m_coordSys.sysName);
    if(dlg->exec() == QDialog::Accepted) {
        QString name = dlg->getUserCoordName();

        if (m_userTreeItem != nullptr) {
            UserCoordSystem coord = CProfile::Inst()->getUserCoordSystem(name);
            QString coord_name = QString::fromStdString(coord.coord_name);
            QString content = QString::fromStdString(coord.coord_system_content);
            QString define = QString::fromStdString(coord.coord_definition);
            auto item = addTreeChild(m_userTreeItem, coord_name, define);
            item->setData(0, Qt::UserRole, QVariant::fromValue(content));
        }
    }
}

void SRSWindow::on_btnOk_clicked()
{
    if(!m_coordSys.isValid())
        return;

    QSettings settings(m_regKey, QSettings::NativeFormat);
    settings.remove("");
    settings.setValue(m_coordSys.sysDefine, m_coordSys.sysFullName);

    accept();
}

void SRSWindow::onLatLngChanged(const QString& text)
{
    QString latitude = ui->editLat->text();
    QString longitude = ui->editLng->text();
    if(!latitude.isEmpty() && !longitude.isEmpty()) {
        m_treeItemENU->setText(1, QString("ENU:%1,%2").arg(latitude).arg(longitude));
        ui->treeWidget->itemClicked(m_treeItemENU, 0);
    } else {
        m_treeItemENU->setText(1, QString());
    }
}