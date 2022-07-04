#include "stdafx.h"
#include "userrswindow.h"
#include "ui_userrswindow.h"
#include "Config/UserProfile/Profile.h"
#include <QFile>

UserRSWindow::UserRSWindow(QWidget *parent)
    : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
    , ui(new Ui::UserRSWindow())
    , m_coordFilePath("")
    , m_content("")
{
    ui->setupUi(this);
}

UserRSWindow::~UserRSWindow()
{
    delete ui;
}

QString UserRSWindow::getUserCoordName()
{
    return ui->lineEdit->text();
}

void UserRSWindow::on_btnOk_clicked()
{
    QString name = ui->lineEdit->text();
    QString definition = ui->lineEdit_define->text();
    QString content = ui->plainTextEdit->toPlainText();

    if (name.isEmpty() || definition.isEmpty()) {
        new ToastWidget(this, QObject::tr("请输入名称或自定义坐标系描述!"), QColor("#e25c59"), 2000);
        return;
    }

    if (!m_coordFilePath.isEmpty() && content.isEmpty()) {
        new ToastWidget(this, QObject::tr("选择的文件没有内容!"), QColor("#e25c59"), 2000);
        return;
    }

    if (content.isEmpty()) {
        new ToastWidget(this, QObject::tr("请输入坐标系的内容!"), QColor("#e25c59"), 2000);
        return;
    }
    
    // 记录到数据库
    UserCoordSystem userCoord;
    userCoord.coord_name = name.toStdString();
    userCoord.coord_definition = definition.toStdString();
    userCoord.coord_system_content = QString(content.toLocal8Bit().toBase64()).toStdString();
    userCoord.create_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();
    userCoord.modify_time = userCoord.create_time;
    CProfile::Inst()->insertUserCoordSystem(userCoord);

    this->accept();
}

void UserRSWindow::on_btnSelect_clicked()
{
    QStringList filters;
    filters << "WKT files (*.wkt)" << "PRJ files (*.prj)";
    QStringList files = RBHelper::getSelectedFiles(filters);
    if (files.isEmpty()) return;
    m_coordFilePath = files[0];

    QFile file(files[0]);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        new ToastWidget(this, QObject::tr("文件读取失败!"), QColor("#e25c59"), 2000);
        LOGFMTI("[UserRSWindow] File read failed!");
        return;
    }

    m_content = file.readAll();
    ui->plainTextEdit->setPlainText(m_content);
}

void UserRSWindow::initWindow(const QString& coordName)
{
    UserCoordSystem userCoord = CProfile::Inst()->getUserCoordSystem(coordName);

    m_content = QByteArray::fromBase64(QString::fromLocal8Bit(userCoord.coord_system_content.c_str()).toLocal8Bit());

    ui->lineEdit->setText(QString::fromStdString(userCoord.coord_name));
    ui->plainTextEdit->insertPlainText(QString::fromLocal8Bit(m_content));
    ui->lineEdit_define->setText(QString::fromStdString(userCoord.coord_definition));    
}