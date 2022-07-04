#include "stdafx.h"
#include "UI/MessageBox.h"
#include "Theme/ThemeLoader.h"
#include "Util.h"
#include "Common/SystemUtil.h"

void Util::setFont(const QString &fontName)
{
    if(!QFile::exists(fontName))
        return;

    QFont font;
    int loadedFontId = QFontDatabase::addApplicationFont(fontName);
    QStringList loadedFontFamilies = QFontDatabase::applicationFontFamilies(loadedFontId);
    if(!loadedFontFamilies.empty()) {
        QString fontFamily = loadedFontFamilies.at(0);
        font.setFamily(fontFamily);
    }

    QApplication::setFont(font);
}

void Util::setStylesheet(const QString &styleName)
{
    QFile file(styleName);
    file.open(QFile::ReadOnly);
    QString qss = QLatin1String(file.readAll());

    QToolHelper::setDefaultStyle(qss);
}

void Util::ShowMessageBoxInfo(const QString &info, const QString& title)
{
    QPointer<CMessageBox> msgBox(new CMessageBox);
    msgBox->setMessage(info, CMessageBox::MB_INFO);
    msgBox->setTitle(title);
    msgBox->exec();
}

void Util::ShowMessageBoxError(const QString &info, const QString& title)
{
    QPointer<CMessageBox> msgBox(new CMessageBox);
    int maxHeight = System::getScreenHeight() * 0.8; // 高度不能超过屏幕的80%
    msgBox->setMaximumHeight(maxHeight); 

    QFont font_title = QLabel().font();
    QFontMetrics titleMet(font_title);
    int lineCount = maxHeight / titleMet.height() / 2;
    lineCount = lineCount == 0 ? 1 : lineCount; // 防止lineCount为0的情况
    QString msg = titleMet.elidedText(info, Qt::ElideMiddle, msgBox->width() * lineCount);

    msgBox->setMessage(msg, CMessageBox::MB_ERROR);
    msgBox->setTitle(title);
    msgBox->exec();
}

// int Util::ShowMessageBoxQuesion(const QString &info, const QString& title)
// {
//     QPointer<CMessageBox> msgBox(new CMessageBox);
//     msgBox->setMessage(info, CMessageBox::MB_ASK);
//     msgBox->setTitle(title);
//     return msgBox->exec();
// }

int Util::ShowMessageBoxQuesion(const QString &info, const QString& title, const QString& okBtnText, const QString& cancelBtnText)
{
    QPointer<CMessageBox> msgBox(new CMessageBox);
    msgBox->setMessage(info, CMessageBox::MB_ASK);
    msgBox->setTitle(title);
    msgBox->setOkBtnText(okBtnText);
    msgBox->setCancelText(cancelBtnText);
    return msgBox->exec();
}
