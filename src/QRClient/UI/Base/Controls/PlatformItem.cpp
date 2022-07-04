#include "stdafx.h"
#include "PlatformItem.h"

PlatformItem::PlatformItem(const QString& name, const QIcon& state, QWidget *parent)
    : QWidget(parent)
    , m_name(name)
    , m_state(state)
{
    ui.setupUi(this);
    ui.btnPlatform->setText(name);
    ui.labelState->setPixmap(state.pixmap(30, 30));
    ui.labelState->setToolTip(name);
    
    connect(ui.btnPlatform, SIGNAL(toggled(bool)), this, SLOT(onItemClicked(bool)));
}

PlatformItem::PlatformItem(int platform, const QString& name, const QString& state, QWidget *parent)
    : QWidget(parent)
    , m_platformId(platform)
{
    ui.setupUi(this);
    
    ui.btnPlatform->setText(name);
    ui.btnPlatform->setToolTip(name);
    ui.labelState->setText(state);
    ui.labelState->setToolTip(name);

    connect(ui.btnPlatform, SIGNAL(toggled(bool)), this, SLOT(onItemClicked(bool)));
    connect(ui.labelState, SIGNAL(clicked()), ui.btnPlatform, SLOT(click()));
}

PlatformItem::~PlatformItem()
{

}

bool PlatformItem::isChecked()
{
    return ui.btnPlatform->isChecked();
}

void PlatformItem::onItemClicked(bool checked)
{
    if(checked){
        emit itemChecked(m_platformId);
    }
}

void PlatformItem::setChecked(bool checked)
{
    ui.btnPlatform->setChecked(checked);
}

QRadioButton* PlatformItem::getBtnObj()
{
    return ui.btnPlatform;
}