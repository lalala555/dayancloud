/***********************************************************************
* Module:  TabbarWidget.h
* Author:  hqr
* Modified: 2021/06/11 14:42:34
* Purpose: Declaration of the class
***********************************************************************/
#pragma once

#include <QWidget>
#include <QToolButton>

class TabbarWidget : public QToolButton
{
public:
    TabbarWidget(QWidget *parent = Q_NULLPTR) : QToolButton(parent) {}
    TabbarWidget(const QString& text, const QIcon& icon, QWidget *parent = Q_NULLPTR)
        : QToolButton(parent)
    {
        setFixedSize(QSize(64, 64));
        setIconSize(QSize(24, 24));
        setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        setIcon(icon);
        setText(text);

#ifdef FOXRENDERFARM
		setStyleSheet("QToolButton{background-color: transparent;border-radius: 6px;color:#999;padding:5px;font-size:12px;font-weight:bolder;}"
			"QToolButton:checked{background-color: #292831;}");
#else
		setStyleSheet("QToolButton{background-color: transparent;border-radius: 6px;color:#999;padding:5px;font-size:12px;}"
			"QToolButton:checked{background-color: #292831;}");
#endif

        setCheckable(true);
        setAutoExclusive(true);

    }
    ~TabbarWidget() {}
};
