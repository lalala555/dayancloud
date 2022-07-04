/***********************************************************************
* Module:  FloatLogPanel.h
* Author:  hqr
* Modified: 2016/11/14 14:59:46
* Purpose: Declaration of the class
***********************************************************************/
#ifndef FLOATLOGPANEL_H
#define FLOATLOGPANEL_H

#include "LayerWidget.h"
#include "kernel/trans_engine_define.h"

namespace Ui
{
class FloatLogPanel;
};


class FloatLogPanel : public LayerWidget
{
    Q_OBJECT

public:
    FloatLogPanel(QWidget *parent = 0);
    ~FloatLogPanel();

    void appendLog(const QString& text, int page);
    void updateWidgetGeometry(QObject* obj);
private:
    virtual bool eventFilter(QObject* obj, QEvent* event);
protected:
    virtual void changeEvent(QEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void showEvent(QShowEvent* event);
    virtual void paintEvent(QPaintEvent *p);
signals:
    void emitCloseBtnClicked();
private:
    Ui::FloatLogPanel *ui;
    int m_width;
    int m_height;
};

#endif // FLOATLOGPANEL_H
