#pragma once

#include <QWidget>
#include "ui_BlockInfoWidget.h"

class BlockInfoWidget : public QWidget
{
    Q_OBJECT

public:
    BlockInfoWidget(QWidget *parent = Q_NULLPTR);
    ~BlockInfoWidget();
    void setBlockIcon(const QString& icon);
    void setBlockName(const QString& name);
    void setBlockFilePath(const QString& path);
    void setItemRow(int row) { m_itemRow = row; }
    QString getBlockFilePath() { return m_filePath; }
    void setWidgetItem(QListWidgetItem* item) { m_widgeItem = item; }

private slots:
    void onWidgetClicked();
    void onDeleteBtnClicked();

signals:
    void deleteItem(QListWidgetItem*);

protected:
    virtual void paintEvent(QPaintEvent* ev);
    virtual void enterEvent(QEvent *e);
    virtual void leaveEvent(QEvent *e);

private:
    Ui::BlockInfoWidget ui;
    QString m_filePath;
    QString m_blockName;
    QPushButton* m_delete;
    int m_itemRow;
    QListWidgetItem* m_widgeItem;
};
