#include "stdafx.h"
#include "TableItemDelegate.h"

void TableColorItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QStyleOptionViewItem ostyle(option);

    if(option.state.testFlag(QStyle::State_HasFocus))
        ostyle.state = ostyle.state ^ QStyle::State_HasFocus;
    ostyle.palette.setColor(QPalette::HighlightedText, QColor("#999999"));

    QStyledItemDelegate::paint(painter, ostyle, index);
}

QWidget* TableItemNumberDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QLineEdit *editor = new QLineEdit(parent);
    return editor;
}

void TableItemNumberDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit *lineedit = static_cast<QLineEdit*>(editor);
    QDoubleValidator *validator = new QDoubleValidator(lineedit);
    validator->setNotation(QDoubleValidator::StandardNotation);
    validator->setRange(0.00, 100.00, 2);
    lineedit->setValidator(validator);
    lineedit->setText(index.data(Qt::DisplayRole).toString());
}

void TableItemNumberDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *lineedit = static_cast<QLineEdit*>(editor);
    model->setData(index, lineedit->text());
}

void TableItemNumberDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
