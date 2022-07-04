#include "stdafx.h"
#include "MultiSelectComboBox.h"
#include <QCheckBox>
#include <QEvent>

MultiSelectComboBox::MultiSelectComboBox(QWidget* parent)
    : QComboBox(parent)
    , m_listWidget(new QListWidget(this))
    , m_lineEdit(new QLineEdit(this))
{
    m_lineEdit->setReadOnly(true);
    m_lineEdit->installEventFilter(this);

    setModel(m_listWidget->model());
    setView(m_listWidget);
    setLineEdit(m_lineEdit);

    connect(this, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &MultiSelectComboBox::itemClicked);
}

void MultiSelectComboBox::itemClicked(int index)
{
    QWidget* widget = m_listWidget->itemWidget(m_listWidget->item(index));
    QCheckBox* checkBox = static_cast<QCheckBox *>(widget);
    checkBox->setChecked(!checkBox->isChecked());
}

void MultiSelectComboBox::addItem(const QString& text, const QVariant& userData)
{
    QListWidgetItem* listWidgetItem = new QListWidgetItem(m_listWidget);
    listWidgetItem->setData(Qt::UserRole, userData);

    QCheckBox* checkBox = new QCheckBox(this);
    checkBox->setText(text);
    connect(checkBox, &QCheckBox::stateChanged, this, &MultiSelectComboBox::stateChanged);

    m_listWidget->addItem(listWidgetItem);
    m_listWidget->setItemWidget(listWidgetItem, checkBox);
}

void MultiSelectComboBox::stateChanged(int state)
{
    Q_UNUSED(state);
    QString selectedData;

    int count = m_listWidget->count();
    for(int i = 0; i < count; ++i) {
        QWidget *widget = m_listWidget->itemWidget(m_listWidget->item(i));
        QCheckBox *checkBox = static_cast<QCheckBox *>(widget);

        if(checkBox->isChecked()) {
            selectedData.append(checkBox->text()).append(";");
        }
    }
    if(selectedData.endsWith(";")) {
        selectedData.remove(selectedData.count() - 1, 1);
    }
    if(!selectedData.isEmpty()) {
        m_lineEdit->setText(selectedData);
    } else {
        m_lineEdit->clear();
    }

    m_lineEdit->setToolTip(selectedData);
}

QStringList MultiSelectComboBox::currentText()
{
    QStringList text;
    if(!m_lineEdit->text().isEmpty()) {
        text = m_lineEdit->text().split(';');
    }
    return text;
}

void MultiSelectComboBox::setCurrentText(const QStringList& text)
{
    int count = m_listWidget->count();
    for(int i = 0; i < count; ++i) {
        QWidget* widget = m_listWidget->itemWidget(m_listWidget->item(i));
        QCheckBox* checkBox = static_cast<QCheckBox*>(widget);
        checkBox->setChecked(text.contains(checkBox->text()));
    }
}

void MultiSelectComboBox::clear()
{
    m_listWidget->clear();
}

void MultiSelectComboBox::wheelEvent(QWheelEvent* event)
{
    Q_UNUSED(event);
}

void MultiSelectComboBox::keyPressEvent(QKeyEvent* event)
{
    Q_UNUSED(event);
}

bool MultiSelectComboBox::eventFilter(QObject* object, QEvent* event)
{
    if(object == m_lineEdit && event->type() == QEvent::MouseButtonRelease) {
        showPopup();
        return false;
    }
    return false;
}

QStringList MultiSelectComboBox::selectedItemDatas()
{
    QStringList datas;
    int count = m_listWidget->count();
    for (int i = 0; i < count; ++i) {
        QListWidgetItem *item = m_listWidget->item(i);
        QWidget* widget = m_listWidget->itemWidget(item);
        QCheckBox* checkBox = static_cast<QCheckBox*>(widget);
        if (checkBox != nullptr && checkBox->isChecked()) {
            QString data = item->data(Qt::UserRole).toString();
            datas.append(data);
        }
    }

    return datas;
}