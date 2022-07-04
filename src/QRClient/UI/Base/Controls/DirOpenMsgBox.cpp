#include "stdafx.h"
#include "DirOpenMsgBox.h"

DirOpenMsgBox::DirOpenMsgBox(QWidget *parent)
    : Dialog(parent)
    , m_savePath("")
{
    ui.setupUi(this);
    ui.label_logo->setPixmap(QPixmap(LOGO_ICON));
    connect(ui.label_content, SIGNAL(linkActivated(const QString &)), this, SLOT(labelLinkClicked(const QString &)));
    connect(ui.btnRedownload, SIGNAL(clicked()), this, SLOT(btnRedownloadClicked()));
}

DirOpenMsgBox::~DirOpenMsgBox()
{

}

void DirOpenMsgBox::setMsgContent(const QString& msg, const QString& keyWord)
{
    QString content = msg;
    if(!keyWord.isEmpty()) {
        content = QString("%1。<style>a{text-decoration: none}</style><a href='%2' style='color:#675ffe;'>%3</a>")
                  .arg(msg).arg("https://localhost").arg(keyWord);
    }

    QFont font;
    font.setFamily("Microsoft YaHei");
    font.setPointSize(15);
    ui.label_content->setFont(font);
    ui.label_content->setText(content);
}

void DirOpenMsgBox::labelLinkClicked(const QString &)
{
    m_savePath = fileDialogOpen();
    if(!m_savePath.isEmpty())
        this->close();
}

void DirOpenMsgBox::btnRedownloadClicked()
{
    m_savePath = fileDialogOpen();
    if(!m_savePath.isEmpty())
        this->close();
}

QString DirOpenMsgBox::getSavePath()
{
    return m_savePath;
}

QString DirOpenMsgBox::fileDialogOpen()
{
    QFileDialog filedialog;

    filedialog.setWindowTitle(tr("选择文件夹"));
    filedialog.setDirectory(".");
    filedialog.setOption(QFileDialog::ShowDirsOnly);
    filedialog.setFileMode(QFileDialog::Directory);
    if(filedialog.exec() !=  QDialog::Accepted) {
        return "";
    }
    return filedialog.selectedFiles()[0];
}