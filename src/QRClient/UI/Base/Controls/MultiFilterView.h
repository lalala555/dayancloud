#ifndef MULTIFILTERVIEW_H
#define MULTIFILTERVIEW_H

#include <QMenu>
#include <QCheckBox>
#include "BasePopup.h"
#include "ui_MultiFilterView.h"

class MultiFilterView : public BasePopup
{
    Q_OBJECT

public:
    enum DATA_TYPE {
        TYPE_INT,
        TYPE_STRING,
    };
public:
    MultiFilterView(QWidget *parent = 0);
    ~MultiFilterView();
    void addFilterItem(const QString& txt, const QVariant& data = QVariant(), int defaultState = Qt::Checked);
    void addFilterItem(const QStringList& list, int defaultState = Qt::Checked);
    QList<QVariant> getSelectItemProperty();
    QList<QVariant> getSelectItemsProperty();
    void clearAllItems();
    void setHeaderSectionRect(const QRect& rect);
    void showView();
    void updateItemState(const QVariant& data, int dataType = TYPE_INT);
    void retranslateUi();
    void initSelectAllCheckBox();
    void hideSearchBox();
    void hideOkCancelBtn();
    void setCheckBoxEnableFilter(bool enable);
    void setCustomStyleSheet(const QString& style);
    void setViewSize(int width, int height);

private:
    QCheckBox* createFilterItem(const QString& txt, const QVariant& data = QVariant(), int checkState = Qt::Checked);
    QList<QListWidgetItem*> findAdaptItems(const QString& txt);

private slots:
    void onRevealTriggered();
    void onCheckStateChanged(bool);
    void updateCheckAllBox();
    QList<QCheckBox*> getAllCheckBox();

public slots:
    void updateAllCheckBoxState(int state);

signals:
    void okBtnClickedSignal();

protected:
    virtual void paintEvent(QPaintEvent *p);
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::MultiFilterView ui;
    QAction* m_revealAction;
    QCheckBox* m_checkAll;
    QRect m_sectionRect;
    QMap<QVariant, QCheckBox*> m_items;
    bool m_enable;
    int m_width;
    int m_height;
};

#endif // MULTIFILTERVIEW_H
