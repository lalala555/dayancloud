#include "stdafx.h"
#include "MuiltFileDialog.h"
#include <QListView>
#include <QTreeView>
#include <QDialogButtonBox>

MuiltFileDialog::MuiltFileDialog(QWidget *parent)
    : QFileDialog(parent)
{
    this->setOption(QFileDialog::DontUseNativeDialog, true);

    //֧�ֶ�ѡ
    QListView *pListView = this->findChild<QListView*>("listView");
    if (pListView)
        pListView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    QTreeView *pTreeView = this->findChild<QTreeView*>();
    if (pTreeView)
        pTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    QDialogButtonBox *pButton = this->findChild<QDialogButtonBox *>("buttonBox");

    disconnect(pButton, SIGNAL(accepted()), this, SLOT(accept()));//ʹ����ʧЧ
    connect(pButton, SIGNAL(accepted()), this, SLOT(onChiose()));//�ĳ��Լ��Ĳ�
}

MuiltFileDialog::~MuiltFileDialog()
{
}

void MuiltFileDialog::onChiose()
{
    QDialog::accept();
}
