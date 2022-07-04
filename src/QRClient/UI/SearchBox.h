/***********************************************************************
* Module:  SearchBox.h
* Author:  hqr
* Modified: 2016/11/14 15:02:15
* Purpose: Declaration of the class
***********************************************************************/
#ifndef SEARCHBOX_H
#define SEARCHBOX_H

#include <QWidget>

namespace Ui
{
class SearchBoxClass;
}

class SearchBox : public QWidget
{
    Q_OBJECT

public:
    SearchBox(QWidget *parent = 0);
    ~SearchBox();

    void reset();
    void setSearchHolderTxt(const QString& txt);
    void setSearchTooltip(const QString& txt);
    void setRoundRectStyle();
    void setEditFocus();
    QString getSearchContent();
private:
    void updateSeacherBox();
signals:
    void searchTriggered(QString, QString);
    void lostFocus();

private slots:
    void onSearchTriggered();
    void onReturnPressed();
    void onSearchContentChanged(const QString &text);
    void onEditLostFocus();
protected:
    virtual void changeEvent(QEvent *event);
private:
    Ui::SearchBoxClass* ui;
    int m_nType;
    QRect m_rect;
    QString m_strstart;
    QString m_strend;
};

#endif // SEARCHBOX_H
