#include "stdafx.h"
#include "PromoteNewsWnd.h"


PromoteNewsWnd::PromoteNewsWnd(QWidget *parent)
    : Notification(parent)
{
    ui.setupUi(this);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setFixedSize(300, 80);
    // 设置ui

    ui.btnNewsClose->setStyleSheet("border-image:url(:/images/main/close_normal.png);height: 11px;width: 11px;padding: 0px;background-color: transparent;");
    ui.widget_icon->setStyleSheet("border:none;padding: 0px;");
#ifdef FOXRENDERFARM
    ui.newsMain->setStyleSheet("background-color: #f3512b;border: 2px solid transparent;border-radius: 8px;");
    ui.label_icon->setStyleSheet("border-image:url(:/images/news_logo_fox.png);background-color: transparent;");
#else
    ui.newsMain->setStyleSheet("#newsMain{background-color: #675ffe;border: 2px solid transparent;border-radius: 8px;}");
    ui.label_icon->setStyleSheet("max-width: 40px;max-height: 40px;border-image:url(:/images/news_logo_bus.png);background-color: transparent;");
#endif

    connect(ui.label_content, SIGNAL(linkActivated(const QString &)), this, SLOT(labelLinkClicked(const QString &)));
}

PromoteNewsWnd::~PromoteNewsWnd()
{

}

void PromoteNewsWnd::closeEvent(QCloseEvent * event)
{
    this->stopAnimation();
}

void PromoteNewsWnd::changeEvent(QEvent *event)
{
    switch(event->type()) {
    case QEvent::LanguageChange: {
        this->close();
        //ui.retranslateUi(this);
    }
    break;
    default:
        Notification::changeEvent(event);
        break;
    }
}

void PromoteNewsWnd::showPromoteNews(const NewsContent& newsInfo)
{
    m_content = newsInfo;

    int maxWidth = 180;// 单行最大宽度
    // 加载标题
    QFont font_title = ui.label_title->font();
    QFontMetrics titleMet(font_title);
    QString title = newsInfo.title;
    ui.label_title->setToolTip(title);
    title = titleMet.elidedText(title, Qt::ElideRight, maxWidth);
    ui.label_title->setText(title);
    // 加载正文，实现自动调整高度
    QString content = newsInfo.content;
    ui.label_content->setWordWrap(true);
    // 超过两行就省略
    QFont font = ui.label_content->font();
    QFontMetrics fontMet(font);
    int fontWidth = fontMet.width(content);
    if(fontWidth > maxWidth ) {
        content = fontMet.elidedText(content, Qt::ElideRight, maxWidth * 2);
    }

    // 设置链接
    QString urlText = QString("<style>a{text-decoration: none}</style><a href='%1' style='color:#ffffff;'>%2</a>")
                      .arg(newsInfo.hyperlink).arg(content);
    ui.label_content->setText(urlText);
    ui.label_content->setToolTip(m_content.content);

    // 设置窗口位置
    this->move((m_desktop.screenGeometry().width()), m_desktop.screenGeometry().height() - this->height());

#ifdef Q_OS_WIN
    //设置窗口置顶
    ::SetWindowPos(HWND(this->winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
#endif

    this->showAnimation();
    this->show();
    this->activateWindow();
}

void PromoteNewsWnd::labelLinkClicked(const QString & url)
{
    QDesktopServices::openUrl(QUrl(url));
    this->close();
}

QString PromoteNewsWnd::getContentMd5()
{
    // 计算内容和标题的hash值
    QString content = m_content.title + m_content.content;
    QString md5;
    QByteArray ba, bb;
    QCryptographicHash md(QCryptographicHash::Md5);
    ba.append(content);
    md.addData(ba);
    bb = md.result();
    md5.append(bb.toHex());

    return md5;
}
