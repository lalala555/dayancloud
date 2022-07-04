#ifndef MESSAGEBOXATBLOCK_H
#define MESSAGEBOXATBLOCK_H

#include "Dialog.h"

namespace Ui
{
class CMessageBoxATBlock;
}

class CMessageBoxATBlock: public Dialog
{
    Q_OBJECT

public:

public:
    explicit CMessageBoxATBlock(QWidget *parent = 0);
    ~CMessageBoxATBlock();

    void setTitle(const QString &title);
    void setOkBtnText(const QString& text);
    void setCancelText(const QString& text);
	bool getMerageOption();

private slots:
    void on_btnOk_clicked();
	void on_btnCancel_clicked();
    
signals:
    void linkClicked(const QString& link);

protected:
    bool eventFilter(QObject* obj, QEvent* event);

private:
    Ui::CMessageBoxATBlock *ui;

	bool m_isMerage;
};

#endif // MESSAGEBOXATBLOCK_H
