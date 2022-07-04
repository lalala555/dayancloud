#include "stdafx.h"
#include "SearchBox.h"
#include "ui_SearchBox.h"
#include <QRegExp>

SearchBox::SearchBox(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SearchBoxClass)
{
    ui->setupUi(this);
	//ui->editSearchContent->setFixedWidth(400);
    QAction *searchAction = ui->editSearchContent->addAction(QIcon(":/view/images/view/search_normal.png"), QLineEdit::TrailingPosition);
    connect(searchAction, SIGNAL(triggered()), this, SLOT(onSearchTriggered()));
    connect(ui->editSearchContent, SIGNAL(textChanged(const QString &)), this, SLOT(onSearchContentChanged(const QString &)));
  // connect(ui->editSearchContent, SIGNAL(returnPressed()), this, SLOT(onReturnPressed()));

    QToolButton* clearButton = ui->editSearchContent->findChild<QToolButton*>();
    clearButton->setIcon(QIcon(":/view/images/view/clear.png"));
}

SearchBox::~SearchBox()
{
    delete ui;
}

void SearchBox::onSearchTriggered()
{
    QString content = ui->editSearchContent->text();
    QString strKey;
    emit searchTriggered(strKey, content);
}

void SearchBox::onReturnPressed()
{
    onSearchTriggered();
}

void SearchBox::onSearchContentChanged(const QString &text)
{
}

void SearchBox::changeEvent(QEvent * event)
{
    switch(event->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        QWidget::changeEvent(event);
        break;
    }
}

void SearchBox::reset()
{
    ui->editSearchContent->clear();
}

void SearchBox::setSearchHolderTxt(const QString& txt)
{
    ui->editSearchContent->setPlaceholderText(txt);
}

void SearchBox::setSearchTooltip(const QString & txt)
{
    ui->editSearchContent->setToolTip(txt);
}

void SearchBox::setRoundRectStyle()
{
    ui->editSearchContent->setTextMargins(8, 0, 0, 0);
}

void SearchBox::onEditLostFocus()
{
    this->hide();
    emit lostFocus();
}

void SearchBox::setEditFocus()
{
    ui->editSearchContent->setFocus();
}

QString SearchBox::getSearchContent()
{
    return ui->editSearchContent->text();
}