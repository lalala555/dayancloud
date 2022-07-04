#ifndef UPDATEFORM_H
#define UPDATEFORM_H

#include "Dialog.h"
#include "ui_UpdateForm.h"

class UpdateForm : public Dialog
{
    Q_OBJECT

public:
    UpdateForm(QWidget *parent = 0);
    ~UpdateForm();

    void showUpdate();
    void initUi();
private slots:
    void slotButtonOK();
    void slotCancel();
protected:
    virtual void changeEvent(QEvent *event);
private:
    Ui::UpdateFormClass ui;
};

#endif // UPDATEFORM_H
