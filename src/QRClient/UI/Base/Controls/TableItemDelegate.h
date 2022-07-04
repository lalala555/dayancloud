#pragma once
#include <QStyledItemDelegate>
#include <QItemDelegate>

class TableColorItemDelegate : public QStyledItemDelegate
{
public:
    TableColorItemDelegate(QObject *parent = 0) : QStyledItemDelegate(parent) {};
    ~TableColorItemDelegate(void) {};
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class TableItemNumberDelegate : public QStyledItemDelegate
{
public:
    TableItemNumberDelegate(QObject *parent = 0) : QStyledItemDelegate(parent) {};
    ~TableItemNumberDelegate(void) {};

public:
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};