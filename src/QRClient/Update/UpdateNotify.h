#ifndef UPDATENOTIFY_H
#define UPDATENOTIFY_H

#include "Notification.h"

namespace Ui
{
class UpdateNotify;
};

class UpdateNotify : public Notification
{
    Q_OBJECT

public:
    explicit UpdateNotify(QWidget *parent = 0);
    ~UpdateNotify();

    void showUpdate();

private slots:
    void updateNow();

protected:
    virtual void closeEvent(QCloseEvent * event);
    virtual void changeEvent(QEvent *event);
private:
    Ui::UpdateNotify *ui;
};

#endif // UPDATENOTIFY_H
