#ifndef ICONLINEEDIT_H
#define ICONLINEEDIT_H

#include <QLineEdit>
#include <QPaintEvent>
#include <QPixmap>
#include <QAction>

class IconLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    IconLineEdit(QWidget *parent = 0);
    ~IconLineEdit();
    void setIcon(const QPixmap& pixmap);
    void setEye(const QIcon& icon);
    void supportInputMethod(bool support) 
    {
        m_supportIM = support;
    }

signals:
    void focussed(bool hasFocus);

private slots:
    void onRevealTriggered();

protected:
    virtual void focusOutEvent(QFocusEvent *event);
    virtual void focusInEvent(QFocusEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual QVariant inputMethodQuery(Qt::InputMethodQuery query) const;

private:
    QPixmap m_pixmap;
    QAction* m_revealAction;
    bool m_supportIM;
};

#endif // ICONLINEEDIT_H
