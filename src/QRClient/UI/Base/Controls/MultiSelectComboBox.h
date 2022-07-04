/***********************************************************************
* Module:  MultiSelectComboBox.h
* Author:  hqr
* Created: 2021/05/22 14:33:24
* Modifier: hqr
* Modified: 2021/05/22 14:33:24
* Purpose: Declaration of the class
***********************************************************************/
#pragma once

#include <QComboBox>
#include <QListWidget>
#include <QLineEdit>

class MultiSelectComboBox : public QComboBox
{
    Q_OBJECT

public:
    MultiSelectComboBox(QWidget* parent = Q_NULLPTR);
    void addItem(const QString& text, const QVariant& userData = QVariant());
    void setCurrentText(const QStringList& texts);
    QStringList currentText();
    QStringList selectedItemDatas();

public slots:
    void clear();

protected:
    void wheelEvent(QWheelEvent* event) override;
    bool eventFilter(QObject* object, QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    void stateChanged(int state);
    void itemClicked(int index);

private:
    QListWidget* m_listWidget;
    QLineEdit* m_lineEdit;
};
