#ifndef EDITABLECOMBOBOX_H
#define EDITABLECOMBOBOX_H

#include <QComboBox>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>

class LineEdit;
//////////////////////////////////////////////////////////////////////////
class ComboBoxItem : public QWidget
{
    Q_OBJECT

public:
    enum BtnPos{
        Normal,
        Button_Head_Icon,
        Button_Tail_Icon,
    };

public:
    ComboBoxItem(QWidget *parent = 0);
    ComboBoxItem(const QString& txt, const QVariant& data, const QString& iconPath, int btnPos = Normal, QWidget *parent = 0);
    ~ComboBoxItem();
    void setText(const QString& txt);
    void setBtnIcon(const QString& icon);
    void setBtnPosition(int btnPos);
    QString itemText();
    void setBtnVisible(bool visible);
    QVariant itemData();
    static QString elidText(const QString& txt, const QFont& font, int maxWidth = 100);

private:
    void initWidget();

signals:
    void btnClicked(const QString& name = "");

private slots:
    void onBtnClicked();

protected:
    virtual void enterEvent(QEvent *e);
    virtual void leaveEvent(QEvent *e);
    virtual void paintEvent(QPaintEvent *p);

private:
    QLabel*      m_itemText;
    QPushButton* m_pushBtn;
    QString      m_text;
    QString      m_iconPath;
    int          m_btnPos;
    QString      m_btnStyle;
    QHBoxLayout* m_layout;
    QVariant     m_data;
};
//////////////////////////////////////////////////////////////////////////

class ComboBoxEdit : public QWidget
{
    Q_OBJECT

public:
    ComboBoxEdit(QWidget *parent = 0);
    ComboBoxEdit(const QString& addBtnIcon, const QString& subBtnIcon, const QString& cancelBtnIcon, QWidget *parent = 0);
    ~ComboBoxEdit();
    void initBtnStyle();
    void setAddBtnIcon(const QString& addBtnIcon);
    void setSubBtnIcon(const QString& subBtnIcon);
    void setCancelBtnIcon(const QString& cancelBtnIcon);
    void retranslateUi();

protected:
    virtual void paintEvent(QPaintEvent *p);
    virtual void keyPressEvent(QKeyEvent *e);

signals:
    void submitClicked(const QString& projName);
    void cancelClicked();

public slots:
    void onShowButton();
private slots:
    void onHideButton();
    void onSubmitClicked();

private:
    void initUi();

private:
    QPushButton* m_submit;
    QPushButton* m_cancel;
    QPushButton* m_addBtn;
    QHBoxLayout* m_editLayout;
    QHBoxLayout* m_mainLayout;
    LineEdit*    m_lineEdit;
    QString      m_addBtnIcon;
    QString      m_subBtnIcon;
    QString      m_cancelBtnIcon;
    QString      m_btnStyle;
};

//////////////////////////////////////////////////////////////////////////
class EditableComboBox : public QComboBox
{
    Q_OBJECT

public:
    EditableComboBox(QWidget *parent = 0);
    ~EditableComboBox();

    void addBoxItem(const QString& txt, const QVariant& data, const QString& icon = ":/images/main/close_hover.png", int type = ComboBoxItem::Button_Tail_Icon);
    int  currentIndex() const;
    void addEditItem();
    void setCurrentEditText(const QString& text);
    void setCurrentIndex(int index);
    int getCurrentIndex(const QString& text);
    QVariant getItemData(int index);
    void clearItems();
    void hideEidtWidget(int countType);
    void hidePopupWidget();
    void retranslateUi();
    QString getCurrentText();

protected:
    virtual void showPopup();
    virtual void hidePopup();
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void paintEvent(QPaintEvent *p);
    virtual void wheelEvent(QWheelEvent * e);

private slots:
    void onListItemClicked(QListWidgetItem *item);
    void onHighLightItem(int index);

signals:
    void submitProjName(const QString&);
    void deleteProject(const QString&);

private:
    QListWidget*   m_listWidget;
    QLineEdit*     m_boxEdit;
    bool           m_mouseClicked;
    ComboBoxEdit*  m_editWidget;
};

#endif // EDITABLECOMBOBOX_H
