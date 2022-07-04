#include "stdafx.h"
#include "ToastWidget.h"
#include <QMovie>

ToastWidget::ToastWidget(QWidget *parent, const QString& msg, const QColor& color, int displayTime)
    : QWidget(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowFlags(Qt::FramelessWindowHint);

    m_widgetOpacity = new QGraphicsOpacityEffect(this);
    m_widgetOpacity->setOpacity(0.8);
    this->setGraphicsEffect(m_widgetOpacity);

    this->setStyleSheet(QString("QLabel{color:#ffffff;font-size:12px;}"
                                "QWidget{background-color:%1;border: 1px solid %2;border-radius: 5px; padding: 0px}").arg(color.name()).arg(color.name())
                       );

    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    verticalLayout->setContentsMargins(0, 0, 0, 0);

    QWidget* main_widget = new QWidget(this);
    QHBoxLayout* horizontalLayout = new QHBoxLayout(main_widget);
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    QLabel* label = new QLabel(msg, main_widget);
    label->setAlignment(Qt::AlignCenter);
    horizontalLayout->addWidget(label);
    verticalLayout->addWidget(main_widget);

    QWidget* parentWidget = this->parentWidget();
    if(parentWidget) {
        // 计算所有文字的宽度
        QFont font = label->font();
        QFontMetrics fm(font);
        int fontWidth = fm.width(msg);

        int w = fontWidth + 30;
        int h = this->height();
        this->resize(w, h);
        QPoint pt = parentWidget->mapFromParent(parentWidget->geometry().center() - QPoint(w / 2, h / 2));
        this->move(pt);
    }

    QTimer::singleShot(displayTime, this, SLOT(hideToast()));

    this->show();
}

ToastWidget::~ToastWidget()
{
}

void ToastWidget::hideToast()
{
    // windowOpacity只能用于顶级窗口
    // QPropertyAnimation* animation = new QPropertyAnimation(this, "windowOpacity");

    QPropertyAnimation* animation = new QPropertyAnimation(m_widgetOpacity,"opacity",this);
    connect(animation, SIGNAL(finished()), this, SLOT(deleteLater()));

    qreal startValue = m_widgetOpacity->opacity();
    qreal endValue = 0.0;

    animation->setDuration(1000);
    animation->setStartValue(startValue);
    animation->setEndValue(endValue);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

//////////////////////////////////////////////////////////////////////////

TipsToastWidget::TipsToastWidget(QWidget *parent, const QString& msg, int fontSize, const QColor& color)
    : QWidget(parent)
    , m_labelMsg(nullptr)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowFlags(Qt::FramelessWindowHint);

    m_widgetOpacity = new QGraphicsOpacityEffect(this);
    m_widgetOpacity->setOpacity(1);
    this->setGraphicsEffect(m_widgetOpacity);

    this->setStyleSheet(QString(
        "QWidget{background-color:%1;border: 1px solid #ffffff;border-radius: 0px; padding: 0px}").arg(color.name())
    );

    QString label_style = QString("QLabel{color:#ffffff;font-size:%1px;border:none;}").arg(fontSize);

    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    verticalLayout->setContentsMargins(1, 1, 1, 1);

    QWidget* main_widget = new QWidget(this);
    QHBoxLayout* horizontalLayout = new QHBoxLayout(main_widget);
    horizontalLayout->setContentsMargins(20, 1, 1, 1);

    QLabel* label_gif = new QLabel(main_widget);
    label_gif->setStyleSheet(label_style);
    QMovie * move = new QMovie(":/view/images/view/rendering.gif");
    label_gif->setMovie(move);
    label_gif->setFixedSize(40, 40);
    label_gif->setScaledContents(true);
    horizontalLayout->addWidget(label_gif);
    move->start();

    m_labelMsg = new QLabel(msg, main_widget);
    m_labelMsg->setAlignment(Qt::AlignCenter);
    m_labelMsg->setStyleSheet(label_style);
    QFont font = m_labelMsg->font();
    font.setPixelSize(fontSize);
    m_labelMsg->setFont(font);
    horizontalLayout->addWidget(m_labelMsg);
    verticalLayout->addWidget(main_widget);

    QWidget* parentWidget = this->parentWidget();
    if (parentWidget) {
        // 计算所有文字的宽度
        QFont font = m_labelMsg->font();
        QFontMetrics fm(font);
        int fontWidth = fm.width(msg);

#ifdef FOXRENDERFARM
		int w = fontWidth * 2 + 60;
#else
		int w = fontWidth * 2 + 30;
#endif        
        int h = this->height() + fm.height() + 20;
        this->resize(w, h);
        QPoint pt = parentWidget->mapFromParent(parentWidget->geometry().center() - QPoint(w / 2, h / 2));
        this->move(pt);
    }

    this->show();
}

TipsToastWidget::~TipsToastWidget()
{
    m_labelMsg->deleteLater();
}

void TipsToastWidget::hideToast()
{
    // windowOpacity只能用于顶级窗口
    // QPropertyAnimation* animation = new QPropertyAnimation(this, "windowOpacity");

    QPropertyAnimation* animation = new QPropertyAnimation(m_widgetOpacity, "opacity", this);
    connect(animation, SIGNAL(finished()), this, SLOT(deleteLater()));

    qreal startValue = m_widgetOpacity->opacity();
    qreal endValue = 0.0;

    animation->setDuration(1000);
    animation->setStartValue(startValue);
    animation->setEndValue(endValue);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void TipsToastWidget::updateMessage(const QString& msg)
{
    if (m_labelMsg != nullptr) {    
        m_labelMsg->setText(msg);
    }
}
