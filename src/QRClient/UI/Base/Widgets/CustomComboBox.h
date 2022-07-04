#ifndef CUSTOMCOMBOBOX_H
#define CUSTOMCOMBOBOX_H

#include <QComboBox>

class CustomComboBox : public QComboBox
{
    Q_OBJECT

public:
    CustomComboBox(QWidget *parent = 0);
    ~CustomComboBox();

private:
    virtual void wheelEvent(QWheelEvent *e) Q_DECL_OVERRIDE {
        if(hasFocus())
            QComboBox::wheelEvent(e);
        else
            e->ignore();
    }
    virtual void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE {
        if(hasFocus())
            QComboBox::keyPressEvent(e);
    }

};

#endif // CUSTOMCOMBOBOX_H
