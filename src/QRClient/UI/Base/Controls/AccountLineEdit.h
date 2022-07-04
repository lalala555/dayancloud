/***********************************************************************
* Module:  AccountLineEdit.h
* Author:  hqr
* Created: 2020/03/23 18:26:00
* Modifier: hqr
* Modified: 2020/03/23 18:26:00
* Purpose: Declaration of the class
***********************************************************************/
#ifndef ACCOUNTLINEEDIT_H
#define ACCOUNTLINEEDIT_H

#include <QtWidgets>
#include <QLineEdit>
#include <QPaintEvent>
#include <QPixmap>
#include <QAction>
#include "BasePopup.h"

struct UserAccount {
    QString name;
    QString password;
    bool remember;
    bool autologin;
};

class AccountWidget : public QWidget
{
    Q_OBJECT

public:
    AccountWidget(const UserAccount& account, QWidget *parent = 0)
        : QWidget(parent)
        , m_account(account)
    {
        this->setObjectName("AccountWidget");
        this->setFixedHeight(35);
        auto layout = new QHBoxLayout();

        auto label = new QLabel(account.name, this);
        label->setStyleSheet("QLabel {background:transparent;}");
        layout->addWidget(label);

        auto button = new QPushButton(this);
        button->setFixedSize(11, 11);
        button->setToolTip(QObject::tr("删除账号信息"));
        button->setStyleSheet("QPushButton {border-image:url(:/images/main/close_normal.png);border:none;background:transparent;}"
                              "QPushButton:hover {border-image:url(:/images/main/close_hover.png);}");
        layout->addWidget(button);
        this->setLayout(layout);

        this->setStyleSheet("#AccountWidget {border: 1px solid #383838;border-top:none;}"
                            "#AccountWidget:hover {background: rgba(255, 255, 255, 10)}"
                            "#AccountWidget:focus {background-color: red;}");
        this->setFocus();
        connect(button, &QPushButton::clicked, this, [=] {
            emit removeWidget(this, m_account);
        });
    }

protected:
    virtual void mouseReleaseEvent(QMouseEvent * event)
    {
        emit selected(m_account);
        QWidget::mouseReleaseEvent(event);
    }
    virtual void paintEvent(QPaintEvent *event)
    {
        QStyleOption option;
        option.initFrom(this);
        QPainter painter(this);
        style()->drawPrimitive(QStyle::PE_Widget, &option, &painter, this);

        QWidget::paintEvent(event);
    };

signals:
    void selected(const UserAccount&);
    void removeWidget(QWidget*, const UserAccount&);

private:
    UserAccount m_account;
    QLabel* m_nameLabel;
    QLabel* m_delButton;
};

class AccountList : public BasePopup
{
    Q_OBJECT

public:
    AccountList(QWidget *parent = 0) : BasePopup(parent)
    {
        QWidget* centralWidget = new QWidget(this);
        centralWidget->setObjectName("centralWidget");
        centralWidget->setStyleSheet("#centralWidget {background-color: #1f1f27;border: 1px solid #383838;border-top:none;}");

        m_layout = new QVBoxLayout();
        m_layout->setMargin(0);
        m_layout->setSpacing(0);
        centralWidget->setLayout(m_layout);

        QScrollArea* scrollArea = new QScrollArea(this);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollArea->setWidgetResizable(true);
        scrollArea->setWidget(centralWidget);

        scrollArea->setMaximumHeight(175);

        auto layout = new QVBoxLayout();
        layout->setMargin(0);
        layout->setSpacing(0);
        layout->addWidget(scrollArea);

        this->setLayout(layout);
    };
    ~AccountList() {};
    void showList()
    {
        if(this->isVisible()) {
            this->hide();
        } else {
            this->popup();
        }
    }
    void addAccounts(const UserAccount& account)
    {
        auto widget = new AccountWidget(account, this);
        connect(widget, &AccountWidget::selected, this, &AccountList::selected);
        connect(widget, &AccountWidget::removeWidget, this, &AccountList::removeWidget);
        m_layout->addWidget(widget);
    }
    void addAccountSpacer()
    {
        QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
        m_layout->addItem(verticalSpacer);
    }
    void resetLayout()
    {
        QList<QWidget *> widgets = findChildren<QWidget *>();
        QEvent e(QEvent::LayoutRequest);
        foreach(QWidget * w, widgets) {
            QCoreApplication::sendEvent(w, &e);
            w->updateGeometry();
        }
        QCoreApplication::sendEvent(this, &e);
        updateGeometry();

        //resize dlg to its content
        QRect currentGeometry = geometry();
        QSize sizeHint = layout()->sizeHint();

        // customize
        if(m_layout->count() < 6) {
            sizeHint.setHeight((m_layout->count() - 1) * 35);
        }

        QPoint p = currentGeometry.center();
        currentGeometry.setWidth(sizeHint.width());
        currentGeometry.setHeight(sizeHint.height());
        currentGeometry.moveCenter(p);
        setMinimumSize(QSize(0, 0));//reset a previously set minimum size!
        setGeometry(currentGeometry);
    }

signals:
    void deleted(const UserAccount&);
    void selected(const UserAccount&);

private slots:
    void removeWidget(QWidget* widget, const UserAccount& account)
    {
        widget->setParent(0);
        m_layout->removeWidget(widget);
        this->resetLayout();

        emit deleted(account);
    }

private:
    QVBoxLayout* m_layout;
};

class AccountLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    AccountLineEdit(QWidget *parent = 0);
    ~AccountLineEdit();
    void setIcon(const QPixmap& pixmap);
    void setDropdownIcon();
    void addAccounts(const QList<UserAccount>& accounts)
    {
        QStringList userNames;
        foreach (auto account, accounts) {
            m_accountPopup->addAccounts(account);
            userNames.append(account.name);
        }
        m_accountPopup->addAccountSpacer();

        QCompleter *completer = new QCompleter(userNames, this);
        completer->setCompletionMode(QCompleter::InlineCompletion);
        connect(this, &QLineEdit::textChanged, this, [=](const QString &text) {
            foreach(auto account, accounts)
            {
                if(account.name == text) {
                    emit selected(account);
                    return;
                }
            }

            emit selected(UserAccount({ text , QString() , 0, 0 }));
        });
        this->setCompleter(completer);
    }
    void supportInputMethod(bool support)
    {
        m_supportIM = support;
    }

signals:
    void deleted(const UserAccount&);
    void selected(const UserAccount&);
    void focussed(bool hasFocus);

private slots:
    void onIconChanged();
    void onRevealTriggered();

protected:
    virtual void paintEvent(QPaintEvent *p);
    virtual void focusInEvent(QFocusEvent *event);
    virtual void focusOutEvent(QFocusEvent *event);
    virtual QVariant inputMethodQuery(Qt::InputMethodQuery query) const;

private:
    QPixmap m_pixmap;
    QAction* m_dropdownAction;
    AccountList* m_accountPopup;
    bool m_supportIM;
};

#endif // ACCOUNTLINEEDIT_H
